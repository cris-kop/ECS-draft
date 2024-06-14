#include "EntityAdmin.h"

EntityAdmin::EntityAdmin()
{
	mLastEntityId = 0;
}

EntityAdmin::~EntityAdmin()
{
	for(auto && system : mSystems)
	{
		delete system;
	}
}

void EntityAdmin::Init()
{
	ISystem *worldPropSystem = new WorldPropsSystem(ComponentSet::Transform);
	ISystem *cameraSystem = new CameraSystem(ComponentSet::Transform | ComponentSet::Camera);

	mSystems.emplace_back(worldPropSystem);
	mSystems.emplace_back(cameraSystem);

	mEntities.emplace(mLastEntityId, 0);		// reserved

	// Register components
	mArchetypeStorageFactory.Register<TransformComponent>();
	mArchetypeStorageFactory.Register<CameraComponent>();
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
	mArchetypesData.emplace_back(pComponentSet, &mArchetypeStorageFactory);
	return static_cast<unsigned int>(mArchetypesData.size()) - 1;
}

int EntityAdmin::AddWorldProp(const Vector3f &pPos, const Vector3f &pRot, const Vector3f &pScale)
{
	++mLastEntityId;
	mEntities.emplace(mLastEntityId, mLastEntityId);
	
	TransformComponent myTransform(pPos, pRot, pScale);
	if(!AttachComponent(myTransform, mLastEntityId))
	{
		return -1;
	}
	return static_cast<int>(mEntities.size()) - 1;
}

int EntityAdmin::AddCamera(const Vector3f &pPos, const Vector3f &pRot, const Vector3f &pScale, const Vector3f &pLookAt, const Vector3f &pYawPitchRoll)
{
	++mLastEntityId;
	mEntities.emplace(mLastEntityId, mLastEntityId);

	TransformComponent myTransform(pPos, pRot, pScale);
	CameraComponent myCamera(pLookAt, pYawPitchRoll);
	
	if(!AttachComponent(myTransform, mLastEntityId))
	{
		return -1;
	}
	if(!AttachComponent(myCamera, mLastEntityId))
	{
		return -1;
	}
	return static_cast<int>(mEntities.size()) - 1;
}

bool EntityAdmin::DeleteEntity(const unsigned int pEntityId)
{
	std::unordered_map<unsigned int, Entity>::const_iterator entityIt = mEntities.find(pEntityId);
	if(entityIt == mEntities.end())
	{
		return false;
	}

	unsigned int archetypeIndex = GetArchetypeDataIndex(entityIt->second.GetComponentSet());

	// remove archetype components data
	unsigned int oldRowIndex = entityIt->second.GetRowIndex();
	if(oldRowIndex != -1)
	{
		int movedEntityId = mArchetypesData[archetypeIndex].DeleteRow(entityIt->second.GetRowIndex());
		if(movedEntityId != -1)
		{
			std::unordered_map<unsigned int, Entity>::iterator movedEntityIt = mEntities.find(movedEntityId);
			if(movedEntityIt == mEntities.end())
			{
				return false;
			}
			movedEntityIt->second.SetRowIndex(oldRowIndex);
		}
	}

	mEntities.erase(pEntityId);
	return true;
}

int EntityAdmin::DuplicateEntity(const unsigned int pSourceEntityId)
{
	std::unordered_map<unsigned int, Entity>::const_iterator sourceEntityIt = mEntities.find(pSourceEntityId);
	if(sourceEntityIt == mEntities.end())
	{
		return false;
	}
	
	++mLastEntityId;
	Entity newEntity(mLastEntityId, sourceEntityIt->second.GetRowIndex(), sourceEntityIt->second.GetComponentSet());

	// copy components
	unsigned int archetype = GetArchetypeDataIndex(sourceEntityIt->second.GetComponentSet());
	newEntity.SetRowIndex(mArchetypesData[archetype].CopyRow(sourceEntityIt->second.GetRowIndex(), mLastEntityId));

	mEntities.try_emplace(mLastEntityId, newEntity);
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

const Entity* EntityAdmin::GetEntity(const unsigned int pEntityId)
{
	std::unordered_map<unsigned int, Entity>::const_iterator entityIt = mEntities.find(pEntityId);
	if(entityIt == mEntities.end())
	{
		return nullptr;
	}
	return &entityIt->second;
}

void EntityAdmin::UpdateRowIndices(const unsigned int pEntityId, const int pOldRowIndex, const int pOldArchetypeIndex, const unsigned int pNewRowIndex, const unsigned int pNewArchetypeIndex)
{
	// no validation
	std::unordered_map<unsigned int, Entity>::iterator entityIt = mEntities.find(pEntityId);
	if(entityIt == mEntities.end())
	{
		return;
	}
	
	entityIt->second.SetRowIndex(pNewRowIndex);
	mArchetypesData[pNewArchetypeIndex].AddEntityIdForAddedRow(pEntityId);

	if(pOldArchetypeIndex != -1)
	{
		int movedEntityId = mArchetypesData[pOldArchetypeIndex].DeleteRow(pOldRowIndex);
		if(movedEntityId != -1)
		{
			std::unordered_map<unsigned int, Entity>::iterator movedEntityIt = mEntities.find(movedEntityId);
			if(movedEntityIt == mEntities.end())
			{
				return;
			}
			movedEntityIt->second.SetRowIndex(pOldRowIndex);
		}
	}
}
