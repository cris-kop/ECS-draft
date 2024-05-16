#ifndef ENTITY_ADMIN_H
#define ENTITY_ADMIN_H

#include "ComponentSet.h"
#include "Components.h"
#include "Entity.h"
#include "Systems.h"
#include "ComponentMappings.h"
#include "ArchetypeStorageFactory.h"

#include <vector>
#include <cassert>
#include <unordered_map>
#include <typeindex>

#include <bitset>

struct EntityAdmin
{
	EntityAdmin();
	
	const Entity& GetEntity(unsigned int pId);

	void Init();

	unsigned int GetArchetypeDataIndex(const ComponentSet pComponentSet);

	// add 'logical archetypes'
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
		Entity &entity = entityIt->second;

		ComponentSet oldSet = entity.GetComponentSet();
		int oldRowIndex = entity.GetRowIndex();
		unsigned int oldArchetypeIndex = GetArchetypeDataIndex(oldSet);

		ComponentSet newComponentType = ComponentSet::None;

		std::unordered_map<std::type_index, ComponentSet>::const_iterator setIt = gCOMPONENT_MAP.find(typeid(T));
		if(setIt != gCOMPONENT_MAP.end())
		{
			newComponentType = setIt->second;
		}
		else
		{
			return false;
		}

		if(Contains(oldSet, newComponentType))
		{
			return false;
		}
		entity.SetComponentSet(oldSet | newComponentType);
		unsigned int newArchetypeIndex = GetArchetypeDataIndex(entity.GetComponentSet());

		int newRowIndex = AddComponentToArchetype(comp, newArchetypeIndex);

		if(oldSet != ComponentSet::None)
		{
			CopyComponentsBetweenArchetypes(pEntityId, oldArchetypeIndex, newArchetypeIndex);
		}
		entity.SetRowIndex(newRowIndex);

		mArchetypesData[newArchetypeIndex].AddEntityIdForAddedRow(pEntityId);

		int movedEntityId = mArchetypesData[oldArchetypeIndex].DeleteRow(oldRowIndex);
		if(movedEntityId != -1)
		{
			mEntities[movedEntityId].SetRowIndex(oldRowIndex);
		}
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
		Entity &entity = entityIt->second;

		ComponentSet oldSet = entity.GetComponentSet();
		int oldRowIndex = entity.GetRowIndex();

		if(!Contains(oldSet, pComponentType))
		{
			return false;
		}

		ComponentSet newSet = oldSet & ~pComponentType;

		unsigned int oldArchetypeIndex = GetArchetypeDataIndex(oldSet);
		unsigned int newArchetypeIndex = GetArchetypeDataIndex(newSet);

		entity.SetComponentSet(newSet);
		entity.SetRowIndex(CopyComponentsBetweenArchetypes(pEntityId, oldArchetypeIndex, newArchetypeIndex));

		mArchetypesData[newArchetypeIndex].AddEntityIdForAddedRow(pEntityId);

		int movedEntityId = mArchetypesData[oldArchetypeIndex].DeleteRow(oldRowIndex);
		if(movedEntityId != -1)
		{
			mEntities[movedEntityId].SetRowIndex(oldRowIndex);
		}
		return true;
	}

	// delete or duplicate entity
	bool DeleteEntity(const unsigned int pEntityId);
	int DuplicateEntity(const unsigned int pSourceEntityId);

	void UpdateSystems();

	template<typename T>
	T* GetComponent(const unsigned int pEntityId)
	{
		std::unordered_map<unsigned int, Entity>::iterator entityIt = mEntities.find(pEntityId);
		if(entityIt == mEntities.end())
		{
			return nullptr;
		}
		
		Entity &entity = entityIt->second;

		unsigned int archetypeIndex = GetArchetypeDataIndex(entity.GetComponentSet());	
		ComponentSet componentType = ComponentSet::None;

		std::unordered_map<std::type_index, ComponentSet>::const_iterator setIt = gCOMPONENT_MAP.find(typeid(T));
		if(setIt != gCOMPONENT_MAP.end())
		{
			componentType = setIt->second;
		}
		else
		{
			return nullptr;
		}

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
		ComponentSet componentType = ComponentSet::None;

		std::unordered_map<std::type_index, ComponentSet>::const_iterator setIt = gCOMPONENT_MAP.find(typeid(T));
		if(setIt != gCOMPONENT_MAP.end())
		{
			componentType = setIt->second;
		}
		else
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
		if(pEntityId > mLastEntityId) 
		{
			return -1;
		}
		auto && entity = mEntities[pEntityId];			// better use .find
		unsigned int sourceRowIndex = entity.GetRowIndex();
		// add check for invalid -1

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
					ComponentStorage *sourceStorage = srcIt->second; 

					// not checking if target storage exist, if not, create
					ComponentStorage *destStorage = mArchetypesData[pTargetArchetype].mStorage[componentType];
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
	
	ArchetypeStorageFactory mArchetypeStorageFactory;

	unsigned int mLastEntityId;
};



#endif