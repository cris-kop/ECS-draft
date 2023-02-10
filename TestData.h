#include <vector>
#include "vector3.h"

// initial test values and comparison values for 'unit tests'

static const std::vector<Vector3f> worldPropPos =
{
	Vector3f(0.0f, 5.0f, 1.0f),
	Vector3f(1.0f, 5.0f, 3.0f),
	Vector3f(2.0f, 5.0f, 5.0f)
};

static const std::vector<Vector3f> worldPropRot =
{
	Vector3f(90.0f, 0.0f, 0.0f),
	Vector3f(0.0f, 90.0f, 0.0f),
	Vector3f(0.0f, 0.0f, 90.0f)
};

static const std::vector<Vector3f> worldPropScale =
{
	Vector3f(1.0f, 1.5f, 1.0f),
	Vector3f(1.0f, 1.0f, 0.5f),
	Vector3f(0.75f, 1.0f, 1.5f)
};

static const Vector3f cameraPos = Vector3f(1.0f, 5.0f, 3.0f);
static const Vector3f cameraRot = Vector3f(0.0f, 90.0f, 0.0f);
static const Vector3f cameraScale = Vector3f(1.0f, 1.0f, 0.5f);
static const Vector3f cameraLookAt = Vector3f(0.0f, 0.0f, 1.0f);
static const Vector3f cameraYawPitchRoll = Vector3f(0.0f, 45.0f, 0.0f);

// validating results

static const std::vector<Vector3f> worldPropPosRotScale =
{
	Vector3f(worldPropPos[0] + worldPropRot[0] + worldPropScale[0]),
	Vector3f(worldPropPos[1] + worldPropRot[1] + worldPropScale[1]),
	Vector3f(worldPropPos[2] + worldPropRot[2] + worldPropScale[2])
};

static const Vector3f cameraPosRotScale = cameraPos + cameraRot + cameraScale;
static const Vector3f cameraLookAtPlusPosRotScale = cameraPosRotScale + cameraLookAt;