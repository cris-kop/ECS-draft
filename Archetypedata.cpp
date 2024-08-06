#include "Archetypedata.h"

#include "ArchetypeStorageFactory.h"


// Constructor

ArchetypeData::ArchetypeData(const ComponentSet &pComponentSet, ArchetypeStorageFactory *pStorageFactoryPtr) : mComponentSet(pComponentSet), mStorageFactoryPtr(pStorageFactoryPtr)
{
	CreateStorage();
}

// Destructor

ArchetypeData::~ArchetypeData()
{
	for(auto && storage : mStorage)
	{
		mStorageFactoryPtr->Destroy(storage.second);
	}
}

// Copy constructor

ArchetypeData::ArchetypeData(const ArchetypeData& pOther)
{
	mEntityIds = pOther.mEntityIds;
	mComponentSet = pOther.mComponentSet;

	mStorageFactoryPtr = pOther.mStorageFactoryPtr;

	// recreate empty storage
	CreateStorage();

	// copy data from other storage to current
	for(auto && storage : mStorage)
	{
		auto it = pOther.mStorage.find(storage.first);
		if(it != pOther.mStorage.end())
		{
			storage.second->CopyAllComponentsFromOtherStorage(it->second);
		}
	}
}

// Move constructor

ArchetypeData::ArchetypeData(ArchetypeData&& pOther) noexcept
{
	mEntityIds = pOther.mEntityIds;
	mComponentSet = pOther.mComponentSet;

	mStorageFactoryPtr = pOther.mStorageFactoryPtr;

	// move storage pointers to new object
	mStorage = pOther.mStorage;

	for(auto && storage : pOther.mStorage)
	{
		storage.second = nullptr;
	}
}


// The rest

void ArchetypeData::AddEntityIdForAddedRow(const unsigned int pEntityId)
{
	mEntityIds.emplace_back(pEntityId);
}

int ArchetypeData::DeleteRow(const unsigned int pIndex)
{
	if(pIndex >= mEntityIds.size())	// invalid index
	{
		return -1;
	}
	
	int movedEntityId = -1;

	// delete all contents for deleted row
	for(auto && storage : mStorage)
	{
		storage.second->DeleteComponent(pIndex);
	}

	// copy components to freed index, if it was not the last row
	if(pIndex != mEntityIds.size() - 1)
	{
		mEntityIds[pIndex] = mEntityIds.back();
		movedEntityId = mEntityIds[pIndex];
	}
	mEntityIds.pop_back();
	return movedEntityId;
}

int ArchetypeData::CopyRow(const unsigned int pRowIndex, const unsigned pTargetEntityId)
{
	if(pRowIndex >= mEntityIds.size())
	{
		return -1;
	}
	mEntityIds.emplace_back(pTargetEntityId);

	for(auto && storage : mStorage)
	{
		storage.second->CopyComponentFromOtherStorage(storage.second, pRowIndex);
	}
	return static_cast<int>(mEntityIds.size()) - 1;		// new row index*/
}

void ArchetypeData::CreateStorage()
{
	for(size_t i=0;i<64;++i)
	{		
		if((static_cast<uint64_t>(mComponentSet) >> i) & static_cast<uint64_t>(1))
		{
			ComponentSet componentType = static_cast<ComponentSet>(static_cast<uint64_t>(1) << i);
			mStorage[componentType] = mStorageFactoryPtr->Create(componentType);
		}
	}
}