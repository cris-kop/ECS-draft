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

	int DeleteRow(const unsigned int pIndex);
};

#endif