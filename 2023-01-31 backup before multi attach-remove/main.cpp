#include <Windows.h>
#include <string>
#include <vector>
#include <algorithm>

#include "Archetypedata.h"
#include "Components.h"
#include "ComponentSet.h"
#include "Entity.h"
#include "Systems.h"
#include "vector3.h"


struct EntityAdmin
{
	std::vector<Entity>				mEntities;
	std::vector<ArchetypeData>		mArchetypesData;
	std::vector<ISystem*>			mSystems;
	
	void Init()
	{
		ISystem *worldPropSystem = new WorldPropsSystem(ComponentSet::Transform);
		ISystem *cameraSystem = new CameraSystem(ComponentSet::Transform | ComponentSet::Camera);

		mSystems.emplace_back(worldPropSystem);
		mSystems.emplace_back(cameraSystem);

		mEntities.emplace_back(0);	// reserved
	}

	unsigned int GetArchetypeDataIndex(const ComponentSet pComponentSet)
	{
		for(size_t index=0;index<mArchetypesData.size();++index)
		{
			if(mArchetypesData[index].mComponentSet == pComponentSet)
			{
				return static_cast<unsigned int>(index);
			}
		}
		// doesn't exist, create new one
		mArchetypesData.emplace_back(pComponentSet);
		return static_cast<unsigned int>(mArchetypesData.size()) - 1;
	}

	bool AttachComponent(const ComponentSet pNewComponent, void *pComponentData, unsigned int pEntityIndex)
	{
		return RemoveAttachComponent(pNewComponent, pComponentData, pEntityIndex);
	}

	bool RemoveComponent(const ComponentSet pComponent, unsigned int pEntityIndex)
	{
		return RemoveAttachComponent(pComponent, nullptr, pEntityIndex);
	}

	// add 'logical archetypes'
	int AddWorldProp(const Vector3f &pPos, const Vector3f &pRot, const Vector3f &pScale)
	{
		mEntities.emplace_back(static_cast<unsigned int>(mEntities.size()));
		
		TransformComponent myTransform(pPos, pRot, pScale);
		AttachComponent(ComponentSet::Transform, (void*)&myTransform, static_cast<unsigned int>(mEntities.size() - 1));

		return static_cast<int>(mEntities.size()) - 1;
	}
	int AddCamera(const Vector3f &pPos, const Vector3f &pRot, const Vector3f &pScale, const Vector3f &pLookAt, const Vector3f &pYawPitchRoll)
	{
		mEntities.emplace_back(static_cast<unsigned int>(mEntities.size()));

		TransformComponent myTransform(pPos, pRot, pScale);
		AttachComponent(ComponentSet::Transform, (void*)&myTransform, static_cast<unsigned int>(mEntities.size() - 1));
		CameraComponent myCamera(pLookAt, pYawPitchRoll);
		AttachComponent(ComponentSet::Camera, (void*)&myCamera, static_cast<unsigned int>(mEntities.size() - 1));

		return static_cast<int>(mEntities.size()) - 1;
	}

	// deleting an entity
	bool DeleteEntity(const unsigned int pEntityIndex)
	{
		if(pEntityIndex >= mEntities.size()) 
		{
			return false;
		}
		auto && entity  = mEntities[pEntityIndex];
		unsigned int archetypeIndex = GetArchetypeDataIndex(entity.GetComponentSet());

		// remove archetype components data
		unsigned int oldRowIndex = entity.GetRowIndex();
		if(oldRowIndex != -1)
		{
			int movedEntityId = mArchetypesData[archetypeIndex].DeleteRow(entity.GetRowIndex());
			if(movedEntityId != -1)
			{
				mEntities[movedEntityId].SetRowIndex(oldRowIndex);
			}
		}

		if(pEntityIndex != static_cast<unsigned int>(mEntities.size() - 1))
		{
			mEntities[pEntityIndex] = mEntities.back();
			mEntities[pEntityIndex].SetGlobalIndex(pEntityIndex);
		}
		mEntities.pop_back();
		return true;
	}

	void UpdateSystems()
	{
		for(auto && archeType : mArchetypesData)
		{
			for(auto && system : mSystems)
			{
				if(Contains(archeType.mComponentSet, system->mComponentSet))
				{
					system->Update(archeType);
				}
			}
		}
	}

	// TODO: template this? or specialized templated getter?
	TransformComponent GetTransformComponent(const unsigned int pEntityIndex)
	{
		if(pEntityIndex < static_cast<unsigned int>(mEntities.size()))
		{
			auto && entity = mEntities[pEntityIndex];
			unsigned int archetypeIndex = GetArchetypeDataIndex(entity.GetComponentSet());
			if(Contains(entity.GetComponentSet(), ComponentSet::Transform))
			{
				return mArchetypesData[archetypeIndex].mTransforms[mEntities[pEntityIndex].GetRowIndex()];
			}
		}
		return TransformComponent();
	}

private:
	bool RemoveAttachComponent(const ComponentSet pComponent, void *pComponentData, unsigned int pEntityIndex)
	{
		// currently supporting attaching/removing 1 component at a time
		if(!SetHasOneComponent(pComponent))
		{
			return false;
		}

		if(pEntityIndex >= mEntities.size()) 
		{
			return false;
		}

		auto && entity = mEntities[pEntityIndex];
		
		ComponentSet componentSet = entity.GetComponentSet();
		ComponentSet setToUpdate = componentSet;
		unsigned int oldArchetypeIndex = GetArchetypeDataIndex(componentSet);
		
		if(pComponentData != nullptr)		// attach
		{
			componentSet |= pComponent;
		}
		else								// remove
		{
			componentSet &= ~pComponent;
		}
		entity.SetComponentSet(componentSet);

		unsigned int newArchetypeIndex = GetArchetypeDataIndex(componentSet);

		if(pComponentData == nullptr)
		{
			setToUpdate = entity.GetComponentSet();		// when removing, only copy the new components from set
		}

		CopyComponentsBetweenArchetypes(setToUpdate, pEntityIndex, oldArchetypeIndex, newArchetypeIndex);
		
		// remove row from old archetype and update moved rowIndex for entity taking the place
		unsigned int oldRowIndex = entity.GetRowIndex();
		if(oldRowIndex != -1)
		{
			int movedEntityId = mArchetypesData[oldArchetypeIndex].DeleteRow(entity.GetRowIndex());
			if(movedEntityId != -1)
			{
				mEntities[movedEntityId].SetRowIndex(oldRowIndex);
			}
		}

		// add new component to the row in new archetype data
		if(pComponentData != nullptr)
		{
			AddComponentToArchetype(pComponent, pComponentData, newArchetypeIndex);
		}

		mArchetypesData[newArchetypeIndex].mGlobalEntityIds.emplace_back(pEntityIndex);
		entity.SetRowIndex(static_cast<unsigned int>(mArchetypesData[newArchetypeIndex].mGlobalEntityIds.size()) - 1);
	}

	void AddComponentToArchetype(const ComponentSet &pComponent, void *pComponentData, const unsigned int pArchetypeIndex)
	{
		if(pComponent == ComponentSet::Transform)
		{
			mArchetypesData[pArchetypeIndex].mTransforms.emplace_back(*reinterpret_cast<TransformComponent*>(pComponentData));
		}
		if(pComponent == ComponentSet::Camera)
		{
			mArchetypesData[pArchetypeIndex].mCameras.emplace_back(*reinterpret_cast<CameraComponent*>(pComponentData));
		}
	}

	void CopyComponentsBetweenArchetypes(const ComponentSet &pComponent, const unsigned int pEntityIndex, const unsigned int pSourceArchetype, const unsigned int pTargetArchetype)
	{
		auto && entity = mEntities[pEntityIndex];

		if(SetHasComponent(pComponent, ComponentSet::Transform))
		{
			mArchetypesData[pTargetArchetype].mTransforms.emplace_back(mArchetypesData[pSourceArchetype].mTransforms[entity.GetRowIndex()]);
		}
		if(SetHasComponent(pComponent, ComponentSet::Camera))
		{
			mArchetypesData[pTargetArchetype].mCameras.emplace_back(mArchetypesData[pSourceArchetype].mCameras[entity.GetRowIndex()]);
		}
	}

};



int WINAPI WinMain(HINSTANCE hInstance,
				   HINSTANCE hPrevInstance,
				   LPSTR lpCmdLine,
				   int nCmdShow)
{
	EntityAdmin myAdmin;
	myAdmin.Init();

	myAdmin.AddWorldProp(Vector3f(0.0f, 5.0f, 1.0f), Vector3f(90.0f, 0.0f, 0.0f), Vector3f(1.0f, 1.5f, 1.0f));
	myAdmin.AddWorldProp(Vector3f(1.0f, 5.0f, 3.0f), Vector3f(0.0f, 90.0f, 0.0f), Vector3f(1.0f, 1.0f, 0.5f));
	int entityId = myAdmin.AddWorldProp(Vector3f(2.0f, 5.0f, 5.0f), Vector3f(0.0f, 0.0f, 90.0f), Vector3f(0.75f, 1.0f, 1.5f));

	int otherEntityId = myAdmin.AddCamera(Vector3f(1.0f, 5.0f, 3.0f), Vector3f(0.0f, 90.0f, 0.0f), Vector3f(1.0f, 1.0f, 0.5f), Vector3f(0.0f, 0.0f, 1.0f), Vector3f(0.0f, 45.0f, 0.0f));

	myAdmin.UpdateSystems();

	TransformComponent testGetter = myAdmin.GetTransformComponent(entityId);

//	myAdmin.DeleteEntity(entityId);

	myAdmin.RemoveComponent(ComponentSet::Transform, otherEntityId);
//	myAdmin.RemoveComponent(ComponentSet::Camera, otherEntityId);

	// ADD UNIT TESTS!!!!!!
	// VALIDATE GET COMPONENT WITH THEIR VALUES, ALSO ENTITY IDs


	return 0;
}
