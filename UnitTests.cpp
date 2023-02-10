#include "UnitTests.h"

#include "Components.h"
#include "Main.h"
#include "TestData.h"
#include <windows.h>

namespace UnitTests
{

// UNIT TESTS - VALIDATING RESULTS

bool AttachComponents()
{
	bool passed = true;

	for(size_t index=0;index<worldPropPos.size();++index)
	{
		if(myAdmin.GetComponent<TransformComponent>(entityIds[index])->mPos != worldPropPos[index])		{ passed = false;	}
		if(myAdmin.GetComponent<TransformComponent>(entityIds[index])->mRot != worldPropRot[index])		{ passed = false;	}
		if(myAdmin.GetComponent<TransformComponent>(entityIds[index])->mScale != worldPropScale[index])	{ passed = false;	}
	}

	if(myAdmin.GetComponent<TransformComponent>(entityIds[3])->mPos != cameraPos)		{ passed = false;	}
	if(myAdmin.GetComponent<TransformComponent>(entityIds[3])->mRot != cameraRot)		{ passed = false;	}
	if(myAdmin.GetComponent<TransformComponent>(entityIds[3])->mScale != cameraScale)	{ passed = false;	}

	if(myAdmin.GetComponent<CameraComponent>(entityIds[3])->mLookAt != cameraLookAt)				{ passed = false;	}
	if(myAdmin.GetComponent<CameraComponent>(entityIds[3])->mYawPitchRoll != cameraYawPitchRoll)	{ passed = false;	}
	
	LogPassed(passed, "AttachComponents");
	return passed;
}

bool SystemUpdate()
{
	bool passed = true;

	for(size_t index=0;index<worldPropPos.size();++index)
	{
		if(myAdmin.GetComponent<TransformComponent>(entityIds[index])->mPosRotScale != worldPropPosRotScale[index])	{	passed = false;	}
	}

	if(myAdmin.GetComponent<TransformComponent>(entityIds[3])->mPosRotScale != cameraPosRotScale)	{ passed = false;	}
	if(myAdmin.GetComponent<CameraComponent>(entityIds[3])->mLookAtPlusPosRotScale != cameraLookAtPlusPosRotScale)	{ passed = false;	}

	LogPassed(passed, "Systems update");
	return passed;
}


bool DuplicateEntity()
{
	bool passed = true;

	const TransformComponent *TransformA = myAdmin.GetComponent<TransformComponent>(entityIds[3]);
	const TransformComponent *TransformB = myAdmin.GetComponent<TransformComponent>(entityIds[3]);

	const CameraComponent *CameraA = myAdmin.GetComponent<CameraComponent>(entityIds[4]);
	const CameraComponent *CameraB = myAdmin.GetComponent<CameraComponent>(entityIds[4]);

	if(TransformA == nullptr || TransformB == nullptr || CameraA == nullptr || CameraB == nullptr)	{ passed = false; }

	if(TransformA->mPosRotScale != TransformB->mPosRotScale)					{ passed = false; }
	if(CameraA->mLookAtPlusPosRotScale != CameraB -> mLookAtPlusPosRotScale)	{ passed = false; }

	if(myAdmin.GetEntity(entityIds[3]).GetComponentSet() != myAdmin.GetEntity(entityIds[4]).GetComponentSet())		{ passed = false; }
	
	LogPassed(passed, "Duplicate Entity");
	return passed;
}

bool DeleteEntity()
{
	bool passed = myAdmin.GetEntity(2).GetGlobalId() == -1;
	LogPassed(passed, "Delete Entity");
	return passed;
}

bool RemoveComponent1()
{
	bool passed = true;
	if(myAdmin.GetComponent<TransformComponent>(entityIds[0]) != nullptr)	{ passed = false; }

	if(myAdmin.GetEntity(1).mComponentSet != ComponentSet::None)	{ passed = false; }

	LogPassed(passed, "Remove Component(1)");
	return passed;
}

bool RemoveComponent2()
{
	bool passed = true;
	if(myAdmin.GetComponent<CameraComponent>(entityIds[3]) != nullptr)	{ passed = false; }

	LogPassed(passed, "Remove Component(2)");
	return passed;
}

void LogPassed(const bool pPassed, const std::string &pTestDesc)
{
	std::string logLine = "PASSED: " + pTestDesc + ".\n";
	if(!pPassed)
	{
		logLine = "FAILED: " + pTestDesc + ".\n";
	}
	OutputDebugStringA(logLine.c_str());
}


}