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

	//Entity(const unsigned int pGlobalId) : mGlobalId(pGlobalId), mRowIndex(-1) { }
	// std::unordered_map<unsigned int, Entity>	mEntities;

	mEntities.insert(std::make_pair(mLastEntityId, Entity(0)));		// reserved

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
	Entity newEntity(mLastEntityId);

	mEntities.insert(std::make_pair(mLastEntityId, newEntity));

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
	Entity newEntity(mLastEntityId);

	TransformComponent myTransform(pPos, pRot, pScale);
	CameraComponent myCamera(pLookAt, pYawPitchRoll);
	
	mEntities.insert(std::make_pair(mLastEntityId, newEntity));
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
	if(!EntityExists(pEntityId))
	{
		return false;
	}
	Entity &entity = mEntities[pEntityId];

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
	if(!EntityExists(pSourceEntityId))
	{
		return false;
	}
	Entity &sourceEntity = mEntities[pSourceEntityId];
	
	++mLastEntityId;
	mEntities.insert(std::make_pair(mLastEntityId, Entity(mLastEntityId, sourceEntity.GetRowIndex(), sourceEntity.GetComponentSet())));


/*	std::unordered_map<unsigned int, Entity>	mEntities;

	Entity(const unsigned int pGlobalId, const unsigned int pRowIndex, const ComponentSet pComponentSet) : mGlobalId(pGlobalId), mRowIndex(pRowIndex), mComponentSet(pComponentSet) { }

	mEntities.insert(std::make_pair(mLastEntityId, Entity(mLastEntityId, sourceEntity.GetRowIndex(), sourceEntity.GetComponentSet())));*/


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

const Entity& EntityAdmin::GetEntity(const unsigned int pEntityId)
{
	return mEntities[pEntityId];
}


bool EntityAdmin::EntityExists(const unsigned int pEntityId) const
{
	std::unordered_map<unsigned int, Entity>::const_iterator entityIt = mEntities.find(pEntityId);
	if(entityIt == mEntities.end())
	{
		return false;
	}
	return true;
}

void EntityAdmin::UpdateRowIndices(const unsigned int pEntityId, const int pOldRowIndex, const int pOldArchetypeIndex, const unsigned int pNewRowIndex, const unsigned int pNewArchetypeIndex)
{
	// no validation
	mEntities[pEntityId].SetRowIndex(pNewRowIndex);
	mArchetypesData[pNewArchetypeIndex].AddEntityIdForAddedRow(pEntityId);

	if(pOldArchetypeIndex != -1)
	{
		int movedEntityId = mArchetypesData[pOldArchetypeIndex].DeleteRow(pOldRowIndex);
		if(movedEntityId != -1)
		{
			mEntities[movedEntityId].SetRowIndex(pOldRowIndex);
		}
	}
}
