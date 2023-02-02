#ifndef ENTITY_ADMIN_H
#define ENTITY_ADMIN_H

#include "ComponentSet.h"
#include "Components.h"
#include "Entity.h"
#include "Systems.h"

#include <vector>


struct ComponentData
{
	ComponentSet ComponentType;
	void *Data;

	ComponentData(const ComponentSet pComponentSet, void *pData) : ComponentType(pComponentSet), Data(pData) { }
};


struct EntityAdmin
{
	std::vector<Entity>				mEntities;
	std::vector<ArchetypeData>		mArchetypesData;
	std::vector<ISystem*>			mSystems;
	
	void Init();

	unsigned int GetArchetypeDataIndex(const ComponentSet pComponentSet);
	bool AttachComponents(std::vector<ComponentData> *pComponentData, unsigned int pEntityIndex);
	bool RemoveComponents(const ComponentSet pComponents, unsigned int pEntityIndex);

	// add 'logical archetypes'
	int AddWorldProp(const Vector3f &pPos, const Vector3f &pRot, const Vector3f &pScale);
	int AddCamera(const Vector3f &pPos, const Vector3f &pRot, const Vector3f &pScale, const Vector3f &pLookAt, const Vector3f &pYawPitchRoll);

	// deleting an entity
	bool DeleteEntity(const unsigned int pEntityIndex);

	void UpdateSystems();

	// fetch single component
	template<typename T> ComponentSet GetComponentType()				{	return ComponentSet::None;			}

	template<>	ComponentSet GetComponentType<TransformComponent>()		{	return ComponentSet::Transform;		}
	template<>	ComponentSet GetComponentType<CameraComponent>()		{	return ComponentSet::Camera;		}

	template<typename T>
	T* GetComponent(const unsigned int pEntityIndex)
	{
		auto && entity = mEntities[pEntityIndex];
		unsigned int archetypeIndex = GetArchetypeDataIndex(entity.GetComponentSet());
		
		ComponentSet componentType = GetComponentType<T>();
		
		if(Contains(entity.GetComponentSet(), componentType))
		{
			switch(componentType)
			{
				case ComponentSet::Transform:
					return reinterpret_cast<T*>(&mArchetypesData[archetypeIndex].mTransforms[mEntities[pEntityIndex].GetRowIndex()]);
				case ComponentSet::Camera:
					return reinterpret_cast<T*>(&mArchetypesData[archetypeIndex].mCameras[mEntities[pEntityIndex].GetRowIndex()]);
			}
		}
		return nullptr;
	}

private:
	bool RemoveAttachComponents(const ComponentSet pComponents, std::vector<ComponentData> *pComponentData, unsigned int pEntityIndex);

	void AddComponentsToArchetype(const ComponentSet &pComponent, std::vector<ComponentData> *pComponentData, const unsigned int pArchetypeIndex);
	void CopyComponentsBetweenArchetypes(const ComponentSet &pComponent, const unsigned int pEntityIndex, const unsigned int pSourceArchetype, const unsigned int pTargetArchetype);
};

#endif