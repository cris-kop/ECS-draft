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
		std::span<TransformComponent> transforms = pComponentsData.Get<TransformComponent>();
		std::span<CameraComponent> cameras = pComponentsData.Get<CameraComponent>();
		
		for(size_t index=0;index<transforms.size();++index)
		{
			cameras[index].mLookAtPlusPosRotScale = cameras[index].mLookAt + transforms[index].mPosRotScale;
		}
	}
}