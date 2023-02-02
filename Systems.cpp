#include "Systems.h"

void WorldPropsSystem::Update(ArchetypeData &pComponentsData)
{
	for(auto && transform : pComponentsData.mTransforms)
	{
		transform.mPosRotScale = transform.mPos + transform.mRot + transform.mScale;
	}
}

void CameraSystem::Update(ArchetypeData &pComponentsData)
{
	for(size_t index=0;index<pComponentsData.mCameras.size();++index)
	{
		pComponentsData.mCameras[index].mLookAtPlusPosRotScale = pComponentsData.mCameras[index].mLookAt + pComponentsData.mTransforms[index].mPosRotScale;
	}
}
