#ifndef ENTITY_ADMIN_H
#define ENTITY_ADMIN_H

#include "ComponentSet.h"
#include "Components.h"
#include "Entity.h"
#include "Systems.h"
#include "ComponentMappings.h"
#include "ArchetypeStorageFactory.h"

#include <vector>
#include <unordered_map>
#include <typeindex>

struct EntityAdmin
{
	EntityAdmin();
	
	bool EntityExists(const unsigned int pEntityId)			const;

	const Entity& GetEntity(const unsigned int pEntityId);
	void Init();

	unsigned int GetArchetypeDataIndex(const ComponentSet pComponentSet);

	// add 'logical archetypes' - testing purpose
	int AddWorldProp(const Vector3f &pPos, const Vector3f &pRot, const Vector3f &pScale);
	int AddCamera(const Vector3f &pPos, const Vector3f &pRot, const Vector3f &pScale, const Vector3f &pLookAt, const Vector3f &pYawPitchRoll);

	// Attach-remove components from entity
	template<typename T>
	bool AttachComponent(T comp, const unsigned int pEntityId)
	{
		if(!EntityExists(pEntityId))
		{
			return false;
		}
		Entity &entity = mEntities[pEntityId];

		ComponentSet oldSet = entity.GetComponentSet();
		int oldRowIndex = entity.GetRowIndex();
		unsigned int oldArchetypeIndex = GetArchetypeDataIndex(oldSet);
		
		ComponentSet newComponentType = GetComponentType<T>();
		if(newComponentType == ComponentSet::None)
		{
			return false;
		}

		if(Contains(oldSet, newComponentType))
		{
			return false;
		}

		ComponentSet newSet = oldSet | newComponentType;
		entity.SetComponentSet(newSet);
		unsigned int newArchetypeIndex = GetArchetypeDataIndex(entity.GetComponentSet());

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
		if(!EntityExists(pEntityId))
		{
			return false;
		}
		Entity &entity = mEntities[pEntityId];

		ComponentSet oldSet = entity.GetComponentSet();
		int oldRowIndex = entity.GetRowIndex();
		unsigned int oldArchetypeIndex = GetArchetypeDataIndex(oldSet);

		if(!Contains(oldSet, pComponentType))
		{
			return false;
		}

		ComponentSet newSet = oldSet & ~pComponentType;
		entity.SetComponentSet(newSet);
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
		if(!EntityExists(pEntityId))
		{
			return nullptr;
		}
		Entity &entity = mEntities[pEntityId];

		ComponentSet componentType = GetComponentType<T>();
		if(componentType == ComponentSet::None)
		{
			return nullptr;
		}

		unsigned int archetypeIndex = GetArchetypeDataIndex(entity.GetComponentSet());	

		auto it = mArchetypesData[archetypeIndex].mStorage.find(componentType);
		if(it == mArchetypesData[archetypeIndex].mStorage.end())
		{
			return nullptr;
		}
		ComponentStorage *baseStorage = it->second;
		ActualStorage<T> *actualStorage = static_cast<ActualStorage<T>*>(baseStorage);
		
		return &actualStorage->actualVector[entity.GetRowIndex()];
	}

	std::unordered_map<unsigned int, Entity>	mEntities;
	
	std::vector<ArchetypeData>		mArchetypesData;
	std::vector<ISystem*>			mSystems;

private:
	template<typename T>
	int AddComponentToArchetype(T comp, const unsigned int pArchetypeIndex)
	{
		ComponentSet componentType = GetComponentType<T>();
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
		if(!EntityExists(pEntityId))
		{
			return -1;
		}
		Entity &entity = mEntities[pEntityId];
		
		unsigned int sourceRowIndex = entity.GetRowIndex();
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
				ComponentSet componentType = static_cast<ComponentSet>(1 << i);

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

	void UpdateRowIndices(const unsigned int pEntityId, const unsigned int pOldRowIndex, const unsigned int pOldArchetypeIndex, const unsigned int pNewRowIndex, const unsigned int pNewArchetypeIndex);
	
	ArchetypeStorageFactory mArchetypeStorageFactory;
	unsigned int mLastEntityId;
};



#endif