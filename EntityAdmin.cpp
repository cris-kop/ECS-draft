#include "EntityAdmin.h"

EntityAdmin::EntityAdmin()
{
	// nothing to do
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

	mLastEntityId = 0;
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

int EntityAdmin::CopyComponentsBetweenArchetypes(const unsigned int pEntityId, const unsigned int pSourceArchetype, const unsigned int pTargetArchetype)
{
	std::unordered_map<unsigned int, Entity>::const_iterator entityIt = mEntities.find(pEntityId);
	if(entityIt == mEntities.end())
	{
		return -1;
	}
		
	unsigned int sourceRowIndex = entityIt->second.GetRowIndex();
	if(sourceRowIndex == -1)
	{
		return -1;
	}

	unsigned int destRowIndex = 0;

	ComponentSet sourceSet = mArchetypesData[pSourceArchetype].mComponentSet;
	ComponentSet targetSet = mArchetypesData[pTargetArchetype].mComponentSet;

	for(size_t i=0;i<64;++i)
	{		
		if((static_cast<uint64_t>(sourceSet) >> i) & 1)
		{
			ComponentSet componentType = static_cast<ComponentSet>(static_cast<uint64_t>(1) << i);

			if(SetHasComponent(targetSet, componentType))
			{
				// copy component
				auto srcIt = mArchetypesData[pSourceArchetype].mStorage.find(componentType);
				if(srcIt == mArchetypesData[pSourceArchetype].mStorage.end())
				{
					return -1;
				}
				ComponentStorage *sourceStorage = srcIt->second; 

				auto destIt = mArchetypesData[pTargetArchetype].mStorage.find(componentType);
				if(destIt == mArchetypesData[pTargetArchetype].mStorage.end())
				{
					return -1;
				}
				ComponentStorage *destStorage = destIt->second; 

				if(destStorage == nullptr)
				{
					return -1;	// forgot to register component type?
				}

				destRowIndex = destStorage->CopyComponentFromOtherStorage(sourceStorage, sourceRowIndex);
			}
		}
	}
	return destRowIndex;
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
