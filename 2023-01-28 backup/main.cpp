#include <Windows.h>
#include <string>
#include <vector>
#include <bitset>
#include <unordered_map>
#include <algorithm>
#include "main.h"
#include "ComponentSet.h"

struct Entity
{
	Entity(const unsigned int pGlobalId) : mGlobalId(pGlobalId), mDataVectorIndex(-1) { }
	
	ComponentSet GetComponentSet()
	{
		return mComponentSet;
	}
	int GetDataVectorIndex()
	{
		return mDataVectorIndex;
	}
	void SetDataVectorIndex(const unsigned int pIndex)
	{
		mDataVectorIndex = pIndex;
	}
	void SetComponentSet(const ComponentSet pNewSet)
	{
		mComponentSet = pNewSet;
	}

	ComponentSet mComponentSet = ComponentSet::None;

	size_t	mGlobalId;
	int		mDataVectorIndex;
};

struct TransformComponent
{
	Vector3f mPos;
	Vector3f mRot;
	Vector3f mScale;

	Vector3f mPosRotScale;	// just illustrative

	TransformComponent() { }
	TransformComponent(const Vector3f &pPos, const Vector3f &pRot, const Vector3f &pScale)
		: mPos(pPos), mRot(pRot), mScale (pScale) { }
};

struct CameraComponent
{
	Vector3f mLookAt;
	Vector3f mYawPitchRoll;

	Vector3f mLookAtPlusPosRotScale;		// just illustrative

	CameraComponent() { }
	CameraComponent(const Vector3f &pLookAt, const Vector3f &pYawPitchRoll)
		: mLookAt(pLookAt), mYawPitchRoll(pYawPitchRoll) { }
};

struct ArchetypeData
{
	ArchetypeData(const ComponentSet &pComponentSet) : mComponentSet(pComponentSet) { }
	
	std::vector<unsigned int>		mGlobalEntityIds;
	std::vector<TransformComponent>	mTransforms;
	std::vector<CameraComponent>	mCameras;

	ComponentSet mComponentSet;

	int DeleteRow(const unsigned int pIndex)
	{
		if(pIndex > mGlobalEntityIds.size())	// invalid index
		{
			return -1;
		}
		
		int movedEntityId = -1;
		if(pIndex != static_cast<unsigned int>(mGlobalEntityIds.size()) - 1)
		{
			// move last item to now free index
			if(mTransforms.size() > 0)
			{
				mTransforms[pIndex] = mTransforms.back();
			}
			if(mCameras.size() > 0)
			{
				mCameras[pIndex] = mCameras.back();
			}

			movedEntityId = mGlobalEntityIds[pIndex];
			mGlobalEntityIds[pIndex] = mGlobalEntityIds.back();
		}
		
		if(mTransforms.size() > 0)
		{
			mTransforms.pop_back();
		}
		if(mCameras.size() > 0)
		{
			mCameras.pop_back();
		}
		mGlobalEntityIds.pop_back();
		return movedEntityId;
	}
};

struct ISystem
{
	ISystem(const ComponentSet pComponentSet) : mComponentSet(pComponentSet) { }

	virtual void Update(ArchetypeData &pComponentsData) = 0;

	ComponentSet mComponentSet;
};


struct WorldPropsSystem : public ISystem
{
	WorldPropsSystem(const ComponentSet pComponentSet) : ISystem(pComponentSet) { }

	void Update(ArchetypeData &pComponentsData)
	{
		for(auto && transform : pComponentsData.mTransforms)
		{
			transform.mPosRotScale = transform.mPos + transform.mRot + transform.mScale;
		}
	}
};

struct CameraSystem : public ISystem
{
	CameraSystem(const ComponentSet pComponentSet) : ISystem(pComponentSet) { }

	void Update(ArchetypeData &pComponentsData)
	{
		for(size_t index=0;index<pComponentsData.mCameras.size();++index)
		{
			pComponentsData.mCameras[index].mLookAtPlusPosRotScale = pComponentsData.mCameras[index].mLookAt + pComponentsData.mTransforms[index].mPosRotScale;
		}
	}
};

struct EntityAdmin
{
	std::vector<Entity>				mEntities;
	std::vector<ArchetypeData>		mArchetypesData;

	std::vector<ISystem*>			mSystems;
	
	void Init()
	{
		mArchetypesData.emplace_back(ComponentSet::Transform);
		mArchetypesData.emplace_back(ComponentSet::Transform | ComponentSet::Camera);

		ISystem *worldPropSystem = new WorldPropsSystem(ComponentSet::Transform);
		ISystem *cameraSystem = new CameraSystem(ComponentSet::Transform | ComponentSet::Camera);

		mSystems.emplace_back(worldPropSystem);
		mSystems.emplace_back(cameraSystem);

		mEntities.emplace_back(0);	// reserve
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
		// todo: check if pNewComponent only contains 1 component
		if(pEntityIndex >= mEntities.size()) 
		{
			return false;
		}
		auto && entity = mEntities[pEntityIndex];
		
		ComponentSet newSet = entity.GetComponentSet();
		ArchetypeData tempData(newSet);

		unsigned int oldArchetypeIndex = GetArchetypeDataIndex(newSet);
		newSet |= pNewComponent;
		unsigned int newArchetypeIndex = GetArchetypeDataIndex(newSet);

		// copy archetype data from old archetype data to new archetype data
		if(SetHasComponent(entity.GetComponentSet(), ComponentSet::Transform))
		{
			mArchetypesData[newArchetypeIndex].mTransforms.emplace_back(mArchetypesData[oldArchetypeIndex].mTransforms[entity.GetDataVectorIndex()]);
		}
		if(SetHasComponent(entity.GetComponentSet(), ComponentSet::Camera))
		{
			mArchetypesData[newArchetypeIndex].mCameras.emplace_back(mArchetypesData[oldArchetypeIndex].mCameras[entity.GetDataVectorIndex()]);
		}

		entity.SetComponentSet(newSet);

		if(entity.GetDataVectorIndex() != -1)
		{
			tempData = mArchetypesData[oldArchetypeIndex];

			int movedEntityId = mArchetypesData[oldArchetypeIndex].DeleteRow(entity.GetDataVectorIndex());
			if(movedEntityId != -1)
			{
				mEntities[movedEntityId].SetDataVectorIndex(pEntityIndex);
			}
		}
		
		// add new component to the row in new archetype data
		if(pNewComponent == ComponentSet::Transform)
		{
			mArchetypesData[newArchetypeIndex].mTransforms.emplace_back(*reinterpret_cast<TransformComponent*>(pComponentData));
		}
		if(pNewComponent == ComponentSet::Camera)
		{
			mArchetypesData[newArchetypeIndex].mCameras.emplace_back(*reinterpret_cast<CameraComponent*>(pComponentData));
		}
		mArchetypesData[newArchetypeIndex].mGlobalEntityIds.emplace_back(pEntityIndex);
		entity.SetDataVectorIndex(static_cast<unsigned int>(mArchetypesData[newArchetypeIndex].mGlobalEntityIds.size()) - 1);
		return true;
	}

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

	void UpdateSystems()
	{
		for(auto && archeType : mArchetypesData)
		{
			for(auto && system : mSystems)
			{
				if(Contains(system->mComponentSet, archeType.mComponentSet))
				{
					system->Update(archeType);
				}
			}
		}
	}

	TransformComponent GetTransformComponent(const unsigned int pEntityIndex)
	{
		if(pEntityIndex < static_cast<unsigned int>(mEntities.size()))
		{
			auto && entity = mEntities[pEntityIndex];
			unsigned int archetypeIndex = GetArchetypeDataIndex(entity.GetComponentSet());
			if(Contains(entity.GetComponentSet(), ComponentSet::Transform))
			{
				return mArchetypesData[archetypeIndex].mTransforms[mEntities[pEntityIndex].mDataVectorIndex];
			}
		}
		return TransformComponent();
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

	myAdmin.AddCamera(Vector3f(1.0f, 5.0f, 3.0f), Vector3f(0.0f, 90.0f, 0.0f), Vector3f(1.0f, 1.0f, 0.5f), Vector3f(0.0f, 0.0f, 1.0f), Vector3f(0.0f, 45.0f, 0.0f));

	myAdmin.UpdateSystems();

	TransformComponent testGetter = myAdmin.GetTransformComponent(entityId);

	return 0;
}
