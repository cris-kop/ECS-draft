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

	TransformComponent myTransform(pPos, pRot, pScale);

	mEntities.insert(std::make_pair(mLastEntityId, newEntity));

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
