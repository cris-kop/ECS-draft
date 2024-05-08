#include "EntityAdmin.h"

#include <cassert>

EntityAdmin::EntityAdmin()
{
	mLastEntityId = 0;
}

void EntityAdmin::Init()
{
	ISystem *worldPropSystem = new WorldPropsSystem(ComponentSet::Transform);
	ISystem *cameraSystem = new CameraSystem(ComponentSet::Transform | ComponentSet::Camera);

	mSystems.emplace_back(worldPropSystem);
	mSystems.emplace_back(cameraSystem);

	mEntities.insert(std::make_pair(mLastEntityId, Entity(0)));		// reserved
}

unsigned int EntityAdmin::GetArchetypeDataIndex(const ComponentSet pComponentSet)
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

int EntityAdmin::AddWorldProp(const Vector3f &pPos, const Vector3f &pRot, const Vector3f &pScale)
{
	++mLastEntityId;
	Entity newEntity(mLastEntityId);
	newEntity.SetComponentSet(ComponentSet::Transform);

	TransformComponent myTransform(pPos, pRot, pScale);

	// find the right Archetype index, using componentset
	int archetypeIndex = GetArchetypeDataIndex(ComponentSet::Transform);
	newEntity.SetRowIndex(AddComponentToArchetype(myTransform, archetypeIndex));
	mArchetypesData[archetypeIndex].AddEntityIdForAddedRow(mLastEntityId);

	mEntities.insert(std::make_pair(mLastEntityId, newEntity));
	return static_cast<int>(mEntities.size()) - 1;
}

int EntityAdmin::AddCamera(const Vector3f &pPos, const Vector3f &pRot, const Vector3f &pScale, const Vector3f &pLookAt, const Vector3f &pYawPitchRoll)
{
	++mLastEntityId;
	Entity newEntity(mLastEntityId);
	newEntity.SetComponentSet(ComponentSet::Transform | ComponentSet::Camera);

	TransformComponent myTransform(pPos, pRot, pScale);
	CameraComponent myCamera(pLookAt, pYawPitchRoll);
	
	int archetypeIndex = GetArchetypeDataIndex(ComponentSet::Transform | ComponentSet::Camera);
	newEntity.SetRowIndex(AddComponentToArchetype(myTransform, archetypeIndex));
	AddComponentToArchetype(myCamera, archetypeIndex);
	mArchetypesData[archetypeIndex].AddEntityIdForAddedRow(mLastEntityId);

	mEntities.insert(std::make_pair(mLastEntityId, newEntity));
	return static_cast<int>(mEntities.size()) - 1;
}

bool EntityAdmin::DeleteEntity(const unsigned int pEntityId)
{
	if(pEntityId > mLastEntityId) 
	{
		return false;
	}
	
	std::unordered_map<unsigned int, Entity>::iterator it = mEntities.find(pEntityId);
	if(it == mEntities.end())
	{
		return false;
	}

	Entity &entity = it->second;
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

	mEntities.erase(pEntityId);
	return true;
}

int EntityAdmin::DuplicateEntity(const unsigned int pSourceEntityId)
{
	std::unordered_map<unsigned int, Entity>::iterator it = mEntities.find(pSourceEntityId);
	if(it == mEntities.end())
	{
		return -1;
	}

	auto && sourceEntity = it->second;
	
	++mLastEntityId;
	mEntities.insert(std::make_pair(mLastEntityId, Entity(mEntities[pSourceEntityId], mLastEntityId)));

	// copy components
	unsigned int archetype = GetArchetypeDataIndex(sourceEntity.GetComponentSet());
	mEntities[mLastEntityId].SetRowIndex(mArchetypesData[archetype].CopyRow(sourceEntity.GetRowIndex(), mLastEntityId));
	
	return mLastEntityId;
}

void EntityAdmin::UpdateSystems()
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

// PRIVATE

const Entity& EntityAdmin::GetEntity(unsigned int pId)
{
	return mEntities[pId];
}

/*
bool EntityAdmin::RemoveAttachComponents(const ComponentSet pComponents, std::vector<ComponentData> *pComponentData, unsigned int pEntityId)
{
	if(pEntityId > mLastEntityId) 
	{
		return false;
	}

	auto && entity = mEntities[pEntityId];

	ComponentSet componentSet = entity.GetComponentSet();
	ComponentSet setToUpdate = componentSet;
	unsigned int oldArchetypeIndex = GetArchetypeDataIndex(componentSet);
		
	if(pComponentData != nullptr)		// attach
	{
		componentSet |= pComponents;
	}
	else								// remove
	{
		componentSet &= ~pComponents;
	}
	entity.SetComponentSet(componentSet);

	unsigned int newArchetypeIndex = GetArchetypeDataIndex(componentSet);

	if(pComponentData == nullptr)
	{
		setToUpdate = entity.GetComponentSet();		// when removing, only copy the new components from set
	}

	CopyComponentsBetweenArchetypes(setToUpdate, pEntityId, oldArchetypeIndex, newArchetypeIndex);
		
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
		AddComponentsToArchetype(pComponents, pComponentData, newArchetypeIndex);
	}

	mArchetypesData[newArchetypeIndex].mEntityIds.emplace_back(pEntityId);
	entity.SetRowIndex(static_cast<unsigned int>(mArchetypesData[newArchetypeIndex].mEntityIds.size()) - 1);

	return true;
}*/

// HELPERS
/*
void EntityAdmin::AddComponentsToArchetype(const ComponentSet &pComponent, std::vector<ComponentData> *pComponentData, const unsigned int pArchetypeIndex)
{
	for(auto && component : *pComponentData)
	{	
		if(component.ComponentType == ComponentSet::Transform)
		{
			mArchetypesData[pArchetypeIndex].mTransforms.emplace_back(*reinterpret_cast<TransformComponent*>(component.Data));
		}
		if(component.ComponentType == ComponentSet::Camera)
		{
			mArchetypesData[pArchetypeIndex].mCameras.emplace_back(*reinterpret_cast<CameraComponent*>(component.Data));
		}
	}
}*/
/*
void EntityAdmin::CopyComponentsBetweenArchetypes(const ComponentSet &pComponent, const unsigned int pEntityId, const unsigned int pSourceArchetype, const unsigned int pTargetArchetype)
{
	if(pEntityId > mLastEntityId) 
	{
		return;
	}
	
	auto && entity = mEntities[pEntityId];

	if(SetHasComponent(pComponent, ComponentSet::Transform))
	{
		mArchetypesData[pTargetArchetype].mTransforms.emplace_back(mArchetypesData[pSourceArchetype].mTransforms[entity.GetRowIndex()]);
	}
	if(SetHasComponent(pComponent, ComponentSet::Camera))
	{
		mArchetypesData[pTargetArchetype].mCameras.emplace_back(mArchetypesData[pSourceArchetype].mCameras[entity.GetRowIndex()]);
	}
}*/
