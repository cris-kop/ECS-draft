#include "Systems.h"

void WorldPropsSystem::Update(ArchetypeData &pComponentsData)
{
	if(Contains(pComponentsData.mComponentSet, mComponentSet))
	{
		auto it = pComponentsData.mStorage.find(ComponentSet::Transform);
		if(it != pComponentsData.mStorage.end())
		{
			ComponentStorage *baseStorage = it->second;

			ActualStorage<TransformComponent> *actualStorage = static_cast<ActualStorage<TransformComponent>*>(baseStorage);
			
			for(auto && transform : actualStorage->actualVector)
			{
				transform.mPosRotScale = transform.mPos + transform.mRot + transform.mScale;
			}
		}
	}
}

void CameraSystem::Update(ArchetypeData &pComponentsData)
{
	if(Contains(pComponentsData.mComponentSet, mComponentSet))
	{
		auto transformIt = pComponentsData.mStorage.find(ComponentSet::Transform);
		if(transformIt == pComponentsData.mStorage.end())
		{
			return;	// assert here?
		}

		auto cameraIt = pComponentsData.mStorage.find(ComponentSet::Camera);
		if(cameraIt == pComponentsData.mStorage.end())
		{
			return;	// assert here?
		}

		ComponentStorage *transformBaseStorage = transformIt->second;
		ActualStorage<TransformComponent> *transformStorage = static_cast<ActualStorage<TransformComponent>*>(transformBaseStorage);
			
		ComponentStorage *cameraBaseStorage = cameraIt->second;
		ActualStorage<CameraComponent> *cameraStorage = static_cast<ActualStorage<CameraComponent>*>(cameraBaseStorage);

		for(size_t index=0;index<transformStorage->actualVector.size();++index)
		{
			auto && camera = cameraStorage->actualVector[index];
			auto && transform = transformStorage->actualVector[index];

			camera.mLookAtPlusPosRotScale = camera.mLookAt + transform.mPosRotScale;
		}
	}
}