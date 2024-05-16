#ifndef ARCHETYPEDATA_H
#define ARCHETYPEDATA_H

#include "Components.h"
#include "ComponentSet.h"

#include <vector>
#include <map>

struct ArchetypeStorageFactory;


struct ComponentStorage
{
	virtual unsigned int CopyComponentFromOtherStorage(ComponentStorage *pSourceStorage, unsigned int pSourceIndex) = 0;
	virtual void DeleteComponent(const unsigned int pIndex) = 0;
};

template<typename T>
struct ActualStorage : ComponentStorage
{
	std::vector<T>	actualVector;

	unsigned int CopyComponentFromOtherStorage(ComponentStorage *pSourceStorage, unsigned int pSourceIndex)
	{
		ActualStorage<T> *sourceStorage = static_cast<ActualStorage<T>*>(pSourceStorage);		

		if(pSourceIndex >= sourceStorage->actualVector.size())
		{
			return -1;
		}

		actualVector.emplace_back(sourceStorage->actualVector[pSourceIndex]);
		return static_cast<unsigned int>(actualVector.size()) - 1;
	}

	void DeleteComponent(const unsigned int pIndex)
	{
		ActualStorage<T> *sourceStorage = static_cast<ActualStorage<T>*>(this);		
		if(pIndex < actualVector.size() - 1)
		{
			actualVector[pIndex] = actualVector.back();
		}
		actualVector.pop_back();
	}
};

struct ArchetypeData
{
	ArchetypeData(const ComponentSet &pComponentSet, ArchetypeStorageFactory *storageFactoryPtr);
	
	std::vector<unsigned int>		mEntityIds;

	std::map<ComponentSet, ComponentStorage*>	mStorage;

	ComponentSet				mComponentSet;

	void AddEntityIdForAddedRow(const unsigned int pEntityId);
	int DeleteRow(const unsigned int pIndex);
	int CopyRow(const unsigned int pRowIndex, const unsigned pTargetEntityId);
};


#endif