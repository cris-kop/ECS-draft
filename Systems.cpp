#include "Systems.h"

void WorldPropsSystem::Update(ArchetypeData &pComponentsData)
{
	std::span<TransformComponent> transforms = pComponentsData.Get<TransformComponent>();
			
	for(auto && transform : transforms)
	{
		transform.mPosRotScale = transform.mPos + transform.mRot + transform.mScale;
	}
}

void CameraSystem::Update(ArchetypeData &pComponentsData)
{
	SpansRange components = pComponentsData.GetSpans<TransformComponent, CameraComponent>();

	for(auto && [transform, camera] : components)
	{
		camera.mLookAtPlusPosRotScale = camera.mLookAt + transform.mPosRotScale;
	}
}