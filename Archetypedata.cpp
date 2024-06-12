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
	for(size_t i=0;i<64;++i)
	{		
		if((static_cast<uint64_t>(mComponentSet) >> i) & static_cast<uint64_t>(1))
		{	
			ComponentSet componentType = static_cast<ComponentSet>(1 << i);

			auto it = pOther.mStorage.find(componentType);
			if(it != pOther.mStorage.end())
			{
				mStorage[componentType]->CopyAllComponentsFromOtherStorage(it->second);			
			}
		}
	}
}

// Move constructor

ArchetypeData::ArchetypeData(ArchetypeData&& pOther)
{
	mEntityIds = pOther.mEntityIds;
	mComponentSet = pOther.mComponentSet;

	mStorageFactoryPtr = pOther.mStorageFactoryPtr;

	// move storage pointers to new object
	for(size_t i=0;i<64;++i)
	{		
		if((static_cast<uint64_t>(mComponentSet) >> i) & static_cast<uint64_t>(1))
		{
			ComponentSet componentType = static_cast<ComponentSet>(1 << i);

			auto it = pOther.mStorage.find(componentType);
			if(it != pOther.mStorage.end())
			{
				mStorage[componentType] = it->second;
				it->second = nullptr;
			}
		}
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

	// check if it's not the last row
	if(pIndex != mEntityIds.size() - 1)
	{
		mEntityIds[pIndex] = mEntityIds.back();
		
		for(size_t i=0;i<64;++i)
		{		
			if((static_cast<uint64_t>(mComponentSet) >> i) & static_cast<uint64_t>(1))
			{
				ComponentSet componentType = static_cast<ComponentSet>(1 << i);
				mStorage[componentType]->DeleteComponent(pIndex);
			}
		}
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
	for(size_t i=0;i<64;++i)
	{		
		if((static_cast<uint64_t>(mComponentSet) >> i) & static_cast<uint64_t>(1))
		{
			ComponentSet componentType = static_cast<ComponentSet>(1 << i);
			mStorage[componentType]->CopyComponentFromOtherStorage(mStorage[componentType], pRowIndex);
		}
	}
	return static_cast<int>(mEntityIds.size()) - 1;		// new row index*/
}

void ArchetypeData::CreateStorage()
{
	for(size_t i=0;i<64;++i)
	{		
		if((static_cast<uint64_t>(mComponentSet) >> i) & static_cast<uint64_t>(1))
		{
			ComponentSet componentType = static_cast<ComponentSet>(1 << i);
			mStorage[componentType] = mStorageFactoryPtr->Create(componentType);
		}
	}
}