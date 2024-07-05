#ifndef ENTITY_ADMIN_H
#define ENTITY_ADMIN_H

#include "ComponentSet.h"
#include "Components.h"
#include "Entity.h"
#include "Systems.h"
#include "ArchetypeStorageFactory.h"

#include <vector>
#include <unordered_map>

struct EntityAdmin
{
	EntityAdmin();
	~EntityAdmin();
	
	const Entity* GetEntity(const unsigned int pEntityId);
	void Init();

	unsigned int GetArchetypeDataIndex(const ComponentSet pComponentSet);

	// add 'logical archetypes' - testing purpose
	int AddWorldProp(const Vector3f &pPos, const Vector3f &pRot, const Vector3f &pScale);
	int AddCamera(const Vector3f &pPos, const Vector3f &pRot, const Vector3f &pScale, const Vector3f &pLookAt, const Vector3f &pYawPitchRoll);

	// Attach-remove components from entity
	template<typename T>
	bool AttachComponent(T comp, const unsigned int pEntityId)
	{
		std::unordered_map<unsigned int, Entity>::iterator entityIt = mEntities.find(pEntityId);
		if(entityIt == mEntities.end())
		{
			return false;
		}

		ComponentSet oldSet = entityIt->second.GetComponentSet();
		int oldRowIndex = entityIt->second.GetRowIndex();

		int oldArchetypeIndex = -1;
		if(oldSet != ComponentSet::None)
		{
			oldArchetypeIndex = GetArchetypeDataIndex(oldSet);
		}
		
		ComponentSet newComponentType = T::sType;
		if(newComponentType == ComponentSet::None)
		{
			return false;
		}

		if(Contains(oldSet, newComponentType))
		{
			return false;
		}

		ComponentSet newSet = oldSet | newComponentType;
		entityIt->second.SetComponentSet(newSet);
		unsigned int newArchetypeIndex = GetArchetypeDataIndex(entityIt->second.GetComponentSet());

		int newRowIndex = AddComponentToArchetype(comp, newArchetypeIndex);

		if(oldSet != ComponentSet::None)
		{
			CopyComponentsBetweenArchetypes(pEntityId, oldArchetypeIndex, newArchetypeIndex);
		}
	
		UpdateRowIndices(pEntityId, oldRowIndex, oldArchetypeIndex, newRowIndex, newArchetypeIndex);
		return true;
	}

	// Remove component(s) from entity
	bool RemoveComponent(const ComponentSet pComponentType, const unsigned int pEntityId)
	{
		std::unordered_map<unsigned int, Entity>::iterator entityIt = mEntities.find(pEntityId);
		if(entityIt == mEntities.end())
		{
			return false;
		}

		ComponentSet oldSet = entityIt->second.GetComponentSet();
		int oldRowIndex = entityIt->second.GetRowIndex();
		unsigned int oldArchetypeIndex = GetArchetypeDataIndex(oldSet);

		if(!Contains(oldSet, pComponentType))
		{
			return false;
		}

		ComponentSet newSet = oldSet & ~pComponentType;
		entityIt->second.SetComponentSet(newSet);
		unsigned int newArchetypeIndex = GetArchetypeDataIndex(newSet);

		int newRowIndex = CopyComponentsBetweenArchetypes(pEntityId, oldArchetypeIndex, newArchetypeIndex);

		UpdateRowIndices(pEntityId, oldRowIndex, oldArchetypeIndex, newRowIndex, newArchetypeIndex);	
		return true;
	}

	// delete or duplicate entity
	bool DeleteEntity(const unsigned int pEntityId);
	int DuplicateEntity(const unsigned int pSourceEntityId);

	void UpdateSystems();

	template<typename T>
	T* GetComponent(const unsigned int pEntityId)
	{
		std::unordered_map<unsigned int, Entity>::const_iterator entityIt = mEntities.find(pEntityId);
		if(entityIt == mEntities.end())
		{
			return nullptr;
		}

		ComponentSet componentType = T::sType;
		if(componentType == ComponentSet::None)
		{
			return nullptr;
		}

		unsigned int archetypeIndex = GetArchetypeDataIndex(entityIt->second.GetComponentSet());	

		auto it = mArchetypesData[archetypeIndex].mStorage.find(componentType);
		if(it == mArchetypesData[archetypeIndex].mStorage.end())
		{
			return nullptr;
		}
		ComponentStorage *baseStorage = it->second;
		ActualStorage<T> *actualStorage = static_cast<ActualStorage<T>*>(baseStorage);
		
		return &actualStorage->actualVector[entityIt->second.GetRowIndex()];
	}

	std::unordered_map<unsigned int, Entity>	mEntities;
	
	std::vector<ArchetypeData>		mArchetypesData;
	std::vector<ISystem*>			mSystems;
	std::vector<std::vector<int>>	mSystemArchetypeMatches;

private:
	template<typename T>
	int AddComponentToArchetype(T comp, const unsigned int pArchetypeIndex)
	{
		ComponentSet componentType = T::sType;
		if(componentType == ComponentSet::None)
		{
			return -1;
		}

		auto it = mArchetypesData[pArchetypeIndex].mStorage.find(componentType);
		if(it == mArchetypesData[pArchetypeIndex].mStorage.end())
		{
			return -1;	// forgot to register component type?
		}
		ComponentStorage *baseStorage = it->second;

		ActualStorage<T> *actualStorage = static_cast<ActualStorage<T>*>(baseStorage);
		actualStorage->actualVector.emplace_back(comp);

		return static_cast<int>(actualStorage->actualVector.size()) - 1;
	}
	
	int CopyComponentsBetweenArchetypes(const unsigned int pEntityId, const unsigned int pSourceArchetype, const unsigned int pTargetArchetype)
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

	void UpdateRowIndices(const unsigned int pEntityId, const int pOldRowIndex, const int pOldArchetypeIndex, const unsigned int pNewRowIndex, const unsigned int pNewArchetypeIndex);
	
	ArchetypeStorageFactory mArchetypeStorageFactory;
	unsigned int mLastEntityId;
};



#endif