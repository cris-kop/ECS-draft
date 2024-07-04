#ifndef ARCHETYPEDATA_H
#define ARCHETYPEDATA_H

#include "Components.h"
#include "ComponentSet.h"
#include "SpansRange.h"

#include <vector>
#include <map>
#include <span>
#include <cassert>

struct ArchetypeStorageFactory;


struct ComponentStorage
{
	virtual ~ComponentStorage() { };

	virtual unsigned int CopyComponentFromOtherStorage(ComponentStorage *pSourceStorage, unsigned int pSourceIndex) = 0;
	virtual void CopyAllComponentsFromOtherStorage(ComponentStorage *pSourceStorage) = 0;

	virtual void DeleteComponent(const unsigned int pIndex) = 0;
};

template<typename T>
struct ActualStorage : ComponentStorage
{
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

	void CopyAllComponentsFromOtherStorage(ComponentStorage *pSourceStorage)
	{
		ActualStorage<T> *sourceStorage = static_cast<ActualStorage<T>*>(pSourceStorage);
		actualVector.reserve(sourceStorage->actualVector.size());

		actualVector.insert(actualVector.end(), sourceStorage->actualVector.begin(), sourceStorage->actualVector.end());
	}

	void DeleteComponent(const unsigned int pIndex)
	{
		if(pIndex < actualVector.size() - 1)
		{
			actualVector[pIndex] = actualVector.back();
		}
		actualVector.pop_back();
	}

	std::vector<T>	actualVector;
};

struct ArchetypeData
{
public:
	ArchetypeData(const ComponentSet &pComponentSet, ArchetypeStorageFactory *pStorageFactoryPtr);
	~ArchetypeData();

	ArchetypeData(const ArchetypeData& pOther);				// copy constructor
	ArchetypeData(ArchetypeData&& pOther) noexcept;			// move constructor - TEMP deleted

	ArchetypeData& operator=(const ArchetypeData& other) = delete;		// copy assignment: not allowed
	ArchetypeData& operator=(ArchetypeData&& other) = delete;			// move assignment: not allowed

	template <class T>
	std::span<T> Get()
	{
		ComponentSet componentType = T::sType;

		auto it = mStorage.find(componentType);
		assert(it != mStorage.end());

		ComponentStorage *baseStorage = it->second;
		ActualStorage<T> *actualStorage = static_cast<ActualStorage<T>*>(baseStorage);

		return std::span<T>(actualStorage->actualVector);
	}

	template<class... Ts>
	SpansRange<Ts...> GetSpans()
	{
		return
		{
			Get<Ts>()...
		};
	}

	std::vector<unsigned int>		mEntityIds;

	std::map<ComponentSet, ComponentStorage*>	mStorage;

	ComponentSet				mComponentSet;

	void AddEntityIdForAddedRow(const unsigned int pEntityId);
	int DeleteRow(const unsigned int pIndex);
	int CopyRow(const unsigned int pRowIndex, const unsigned pTargetEntityId);

private:
	ArchetypeStorageFactory *mStorageFactoryPtr;

	void CreateStorage();
};


#endif