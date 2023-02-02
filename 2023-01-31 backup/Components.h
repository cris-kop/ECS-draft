#ifndef COMPONENTS_H
#define COMPONENTS_H

#include "vector3.h"

struct TransformComponent
{
	Vector3f mPos;
	Vector3f mRot;
	Vector3f mScale;

	Vector3f mPosRotScale;	// just illustrative

	TransformComponent() { }
	TransformComponent(const Vector3f &pPos, const Vector3f &pRot, const Vector3f &pScale)
		: mPos(pPos), mRot(pRot), mScale (pScale) { }
};

struct CameraComponent
{
	Vector3f mLookAt;
	Vector3f mYawPitchRoll;

	Vector3f mLookAtPlusPosRotScale;		// just illustrative

	CameraComponent() { }
	CameraComponent(const Vector3f &pLookAt, const Vector3f &pYawPitchRoll)
		: mLookAt(pLookAt), mYawPitchRoll(pYawPitchRoll) { }
};

#endif