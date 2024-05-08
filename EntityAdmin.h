#ifndef ENTITY_ADMIN_H
#define ENTITY_ADMIN_H

#include "ComponentSet.h"
#include "Components.h"
#include "Entity.h"
#include "Systems.h"

#include <vector>
#include <cassert>
#include <unordered_map>
#include <typeindex>

#include <bitset>

static const std::unordered_map<std::type_index, ComponentSet> gCOMPONENT_MAP = 
{
	{ typeid(TransformComponent), ComponentSet::Transform },
	{ typeid(CameraComponent), ComponentSet::Camera }
};

static const std::unordered_map<ComponentSet, std::type_index> gCOMPONENT_MAP_INV = 
{
	{ ComponentSet::Transform, typeid(TransformComponent) },
	{ ComponentSet::Camera, typeid(CameraComponent) }
};



struct EntityAdmin
{
	EntityAdmin();
	
	const Entity& GetEntity(unsigned int pId);

	void Init();

	unsigned int GetArchetypeDataIndex(const ComponentSet pComponentSet);
	//bool AttachComponents(std::vector<ComponentData> *pComponentData, unsigned int pEntityId);
	//bool RemoveComponents(const ComponentSet pComponents, unsigned int pEntityId);

	// add 'logical archetypes'
	int AddWorldProp(const Vector3f &pPos, const Vector3f &pRot, const Vector3f &pScale);
	int AddCamera(const Vector3f &pPos, const Vector3f &pRot, const Vector3f &pScale, const Vector3f &pLookAt, const Vector3f &pYawPitchRoll);

	// delete or duplicate entity
	bool DeleteEntity(const unsigned int pEntityId);
	int DuplicateEntity(const unsigned int pSourceEntityId);

	void UpdateSystems();

	// fetch single component
	template<typename T> ComponentSet GetComponentType()				{	return ComponentSet::None;			}

	template<>	ComponentSet GetComponentType<TransformComponent>()		{	return ComponentSet::Transform;		}
	template<>	ComponentSet GetComponentType<CameraComponent>()		{	return ComponentSet::Camera;		}


	template<typename T>
	T* GetComponent(const unsigned int pEntityId)
	{
		Entity &entity = mEntities[pEntityId];
		if(entity.GetGlobalId() == -1)
		{
			mEntities.erase(pEntityId);
			return nullptr;
		}

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
	int RemoveComponentsFromArchetype(const ComponentSet pComponentSet, const unsigned int pArchetypeIndex, const unsigned int pRowIndex)
	{
		// check if pComponentSet exists in old archetype

		// check if pRowIndex is valid for old archetype

		// define new archetype

		// 'move' to the other archetype, without removed components


		auto it = mArchetypesData[pArchetypeIndex].mStorage.find(componentType);
		if(it == mArchetypesData[pArchetypeIndex].mStorage.end())
		{
			it = mArchetypesData[pArchetypeIndex].mStorage.insert(std::make_pair(componentType, new ActualStorage<T>())).first;
		}
		ComponentStorage *baseStorage = it->second;

		ActualStorage<T> *actualStorage = static_cast<ActualStorage<T>*>(baseStorage);
		actualStorage->actualVector.emplace_back(comp);

		return static_cast<int>(actualStorage->actualVector.size()) - 1;
	}

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
			it = mArchetypesData[pArchetypeIndex].mStorage.insert(std::make_pair(componentType, new ActualStorage<T>())).first;
		}
		ComponentStorage *baseStorage = it->second;

		ActualStorage<T> *actualStorage = static_cast<ActualStorage<T>*>(baseStorage);
		actualStorage->actualVector.emplace_back(comp);

		return static_cast<int>(actualStorage->actualVector.size()) - 1;
	}
	/** TO DO NEW SETUP !!! **/
	/*bool CopyComponentsBetweenArchetypes(const unsigned int pEntityId, const unsigned int pSourceArchetype, const unsigned int pTargetArchetype)
	{
		if(pEntityId > mLastEntityId) 
		{
			return false;
		}
		auto && entity = mEntities[pEntityId];
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

					auto destIt = mArchetypesData[pTargetArchetype].mStorage.find(componentType);
					ComponentStorage *destStorage = destIt->second; 

					destRowIndex = destStorage->CopyComponentFromOtherStorage(sourceStorage, sourceRowIndex);
				}
			}
		}
		entity.SetRowIndex(destRowIndex);
	}*/
	
	unsigned int mLastEntityId;
};

#endif