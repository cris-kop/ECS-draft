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
};
