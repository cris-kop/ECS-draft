#include "Archetypedata.h"

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
			if((static_cast<uint64_t>(mComponentSet) >> i) & 1)
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
	int newRowIndex = 0;
	for(size_t i=0;i<64;++i)
	{		
		if((static_cast<uint64_t>(mComponentSet) >> i) & 1)
		{
			ComponentSet componentType = static_cast<ComponentSet>(1 << i);
			newRowIndex = mStorage[componentType]->CopyComponentFromOtherStorage(mStorage[componentType], pRowIndex);
		}
	}
	return static_cast<int>(mEntityIds.size()) - 1;		// new row index*/
}