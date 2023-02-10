#ifndef VECTOR3_H
#define VECTOR3_H

#include <math.h>

static const float FLOAT_EPSILON = 0.001f;

inline bool FloatEqualsRel(const float pA, const float pB, const float pMaxRelativeError)
{
	if(pA == pB)
	{
		return true;
	}
	return fabsf(pA-pB) < pMaxRelativeError;
}

struct Vector3f
{
	float x, y, z;

	Vector3f() : x(0.0f), y(0.0f), z(0.0f) { }
	Vector3f(const float pX, const float pY, const float pZ)
		: x(pX), y(pY), z(pZ) { }

	Vector3f operator + (const Vector3f &pAddVect) const
	{
		Vector3f newVector = *this;
		newVector.x += pAddVect.x;
		newVector.y += pAddVect.y;
		newVector.z += pAddVect.z;
		return newVector;
	}

	bool operator == (const Vector3f &compVect) const
	{
		return FloatEqualsRel(this->x, compVect.x, FLOAT_EPSILON) &&
			FloatEqualsRel(this->y, compVect.y, FLOAT_EPSILON) &&
			FloatEqualsRel(this->z, compVect.z, FLOAT_EPSILON);
	}

	bool operator != (const Vector3f &compVect) const
	{
		return !FloatEqualsRel(this->x, compVect.x, FLOAT_EPSILON) ||
			!FloatEqualsRel(this->y, compVect.y, FLOAT_EPSILON) ||
			!FloatEqualsRel(this->z, compVect.z, FLOAT_EPSILON);
	}
};

#endif
