#include "Archetypedata.h"

int ArchetypeData::DeleteRow(const unsigned int pIndex)
{
	if(pIndex > mEntityIds.size())	// invalid index
	{
		return -1;
	}
		
	int movedEntityId = -1;
		
	// check if it's not the last row
	if(pIndex != static_cast<unsigned int>(mEntityIds.size()) - 1)
	{
		// move last item to now free index
		mEntityIds[pIndex] = mEntityIds.back();

		if(SetHasComponent(mComponentSet, ComponentSet::Transform))
		{
			mTransforms[pIndex] = mTransforms.back();
		}
		if(SetHasComponent(mComponentSet, ComponentSet::Camera))
		{
			mCameras[pIndex] = mCameras.back();
		}
		movedEntityId = mEntityIds[pIndex];
	}

	// remove last index
	mEntityIds.pop_back();
	if(SetHasComponent(mComponentSet, ComponentSet::Transform))
	{
		mTransforms.pop_back();
	}
	if(SetHasComponent(mComponentSet, ComponentSet::Camera))
	{
		mCameras.pop_back();
	}
	return movedEntityId;
}


int ArchetypeData::CopyRow(const unsigned int pRowIndex, const unsigned pTargetEntityId)
{
	if(pRowIndex >= mEntityIds.size())
	{
		return -1;
	}

	mEntityIds.emplace_back(pTargetEntityId);

	if(SetHasComponent(mComponentSet, ComponentSet::Transform))
	{
		mTransforms.emplace_back(mTransforms[pRowIndex]);
	}
	if(SetHasComponent(mComponentSet, ComponentSet::Camera))
	{
		mCameras.emplace_back(mCameras[pRowIndex]);
	}
	return static_cast<int>(mEntityIds.size()) - 1;		// new row index
}