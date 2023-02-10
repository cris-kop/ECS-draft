#include <Windows.h>
#include <string>
#include <vector>
#include <algorithm>

#include "Main.h"
#include "EntityAdmin.h"
#include "TestData.h"
#include "UnitTests.h"

EntityAdmin myAdmin;
std::vector<int> entityIds;


int WINAPI WinMain(HINSTANCE hInstance,
				   HINSTANCE hPrevInstance,
				   LPSTR lpCmdLine,
				   int nCmdShow)
{
	myAdmin.Init();

	// param is to define if tests should stop when 1 fails
	if(!RunTests(false))
	{
		return -1;
	}
	return 0;
}

bool RunTests(const bool pStopAtFailed)
{
	for(size_t index=0;index<worldPropPos.size();++index)
	{
		entityIds.emplace_back(myAdmin.AddWorldProp(worldPropPos[index], worldPropRot[index], worldPropScale[index]));
	}
	entityIds.emplace_back(myAdmin.AddCamera(cameraPos, cameraRot, cameraScale, cameraLookAt, cameraYawPitchRoll));

	if(!UnitTests::AttachComponents())
	{
		if(pStopAtFailed)	{ return false; }
	}

	myAdmin.UpdateSystems();
	if(!UnitTests::SystemUpdate())
	{
		if(pStopAtFailed)	{ return false; }
	}	

	myAdmin.RemoveComponents(ComponentSet::Transform, entityIds[0]);
	if(!UnitTests::RemoveComponent1())
	{
		if(pStopAtFailed)	{ return false; }
	}	

	entityIds.emplace_back(myAdmin.DuplicateEntity(entityIds[3]));
	if(!UnitTests::DuplicateEntity())
	{
		if(pStopAtFailed)	{ return false; }
	}	

	myAdmin.RemoveComponents(ComponentSet::Camera, entityIds[3]);
	if(!UnitTests::RemoveComponent2())
	{
		if(pStopAtFailed)	{ return false; }
	}	

	myAdmin.DeleteEntity(2);
	if(!UnitTests::DeleteEntity())
	{
		if(pStopAtFailed)	{ return false; }
	}	
	return true;
}
