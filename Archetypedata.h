#ifndef ARCHETYPEDATA_H
#define ARCHETYPEDATA_H

#include "Components.h"
#include "ComponentSet.h"
#include "SpansRange.h"
#include "ComponentStorage.h"

#include <vector>
#include <map>
#include <span>
#include <cassert>

struct ArchetypeStorageFactory;


struct ArchetypeData
{
public:
	ArchetypeData(const ComponentSet &pComponentSet, ArchetypeStorageFactory *pStorageFactoryPtr);
	~ArchetypeData();

	ArchetypeData(const ArchetypeData& pOther);				// copy constructor
	ArchetypeData(ArchetypeData&& pOther) noexcept;			// move constructor - TEMP deleted

	ArchetypeData& operator=(const ArchetypeData& other) = delete;		// copy assignment: not allowed
	ArchetypeData& operator=(ArchetypeData&& other) = delete;			// move assignment: not allowed

	uint32_t GetNrRows()		const;
	bool ValidateRowCounts()	const;

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