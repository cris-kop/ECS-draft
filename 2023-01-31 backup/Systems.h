#ifndef SYSTEMS_H
#define SYSTEMS_H

#include "Archetypedata.h"


struct ISystem
{
	ISystem(const ComponentSet pComponentSet) : mComponentSet(pComponentSet) { }

	virtual void Update(ArchetypeData &pComponentsData) = 0;

	ComponentSet mComponentSet;
};

struct WorldPropsSystem : public ISystem
{
	WorldPropsSystem(const ComponentSet pComponentSet) : ISystem(pComponentSet) { }

	void Update(ArchetypeData &pComponentsData)
	{
		for(auto && transform : pComponentsData.mTransforms)
		{
			transform.mPosRotScale = transform.mPos + transform.mRot + transform.mScale;
		}
	}
};

struct CameraSystem : public ISystem
{
	CameraSystem(const ComponentSet pComponentSet) : ISystem(pComponentSet) { }

	void Update(ArchetypeData &pComponentsData)
	{
		for(size_t index=0;index<pComponentsData.mCameras.size();++index)
		{
			pComponentsData.mCameras[index].mLookAtPlusPosRotScale = pComponentsData.mCameras[index].mLookAt + pComponentsData.mTransforms[index].mPosRotScale;
		}
	}
};

#endif
