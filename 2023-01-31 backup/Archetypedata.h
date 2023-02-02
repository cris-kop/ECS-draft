#ifndef ARCHETYPEDATA_H
#define ARCHETYPEDATA_H

#include "Components.h"
#include "ComponentSet.h"

#include <vector>


struct ArchetypeData
{
	ArchetypeData(const ComponentSet &pComponentSet) : mComponentSet(pComponentSet) { }
	
	std::vector<unsigned int>		mGlobalEntityIds;
	std::vector<TransformComponent>	mTransforms;
	std::vector<CameraComponent>	mCameras;

	ComponentSet mComponentSet;

	int DeleteRow(const unsigned int pIndex)
	{
		if(pIndex > mGlobalEntityIds.size())	// invalid index
		{
			return -1;
		}
		
		int movedEntityId = -1;
		
		// check if it's not the last row
		if(pIndex != static_cast<unsigned int>(mGlobalEntityIds.size()) - 1)
		{
			// move last item to now free index
			mGlobalEntityIds[pIndex] = mGlobalEntityIds.back();

			if(SetHasComponent(mComponentSet, ComponentSet::Transform))
			{
				mTransforms[pIndex] = mTransforms.back();
			}
			if(SetHasComponent(mComponentSet, ComponentSet::Camera))
			{
				mCameras[pIndex] = mCameras.back();
			}
			movedEntityId = mGlobalEntityIds[pIndex];
		}

		// remove last index
		mGlobalEntityIds.pop_back();
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
};

#endif