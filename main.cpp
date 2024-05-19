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

// typeid OR ComponentSet enum?

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
	// Create 4 entities, 3 worldprops and 1 camera
	for(size_t index=0;index<worldPropPos.size();++index)
	{
		entityIds.emplace_back(myAdmin.AddWorldProp(worldPropPos[index], worldPropRot[index], worldPropScale[index]));
	}
	entityIds.emplace_back(myAdmin.AddCamera(cameraPos, cameraRot, cameraScale, cameraLookAt, cameraYawPitchRoll));

	// Are all entities with components created, correct values?
	if(!UnitTests::ValidateCreateComponents())
	{
		if(pStopAtFailed)	{ return false; }
	}

	// Did the systems operate on the components?
	myAdmin.UpdateSystems();
	if(!UnitTests::ValidateSystemsUpdate())
	{
		if(pStopAtFailed)	{ return false; }
	}	

	// Duplicate the Camera entity, with Transform + Camera
	unsigned entityToDuplicate = entityIds[static_cast<unsigned int>(worldPropPos.size())];
	entityIds.emplace_back(myAdmin.DuplicateEntity(entityToDuplicate));
	if(!UnitTests::ValidateDuplicateEntity(entityToDuplicate, entityIds.back()))
	{
		if(pStopAtFailed)	{ return false; }
	}	

	// Remove Transform component from duplicated Camera entity
	myAdmin.RemoveComponent(ComponentSet::Transform, entityToDuplicate);
	if(!UnitTests::ValidateRemoveComponent(TransformComponent(), ComponentSet::Camera, entityToDuplicate))
	{
		if(pStopAtFailed)	{ return false; }
	}

	// Remove the entity 
	myAdmin.DeleteEntity(entityToDuplicate+1);
	if(!UnitTests::ValidateDeleteEntity(entityToDuplicate+1))
	{
		if(pStopAtFailed)	{ return false; }
	}	
	return true;
}
