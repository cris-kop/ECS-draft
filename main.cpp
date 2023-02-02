#include <Windows.h>
#include <string>
#include <vector>
#include <algorithm>

#include "EntityAdmin.h"


int WINAPI WinMain(HINSTANCE hInstance,
				   HINSTANCE hPrevInstance,
				   LPSTR lpCmdLine,
				   int nCmdShow)
{
	EntityAdmin myAdmin;
	myAdmin.Init();

	myAdmin.AddWorldProp(Vector3f(0.0f, 5.0f, 1.0f), Vector3f(90.0f, 0.0f, 0.0f), Vector3f(1.0f, 1.5f, 1.0f));
	myAdmin.AddWorldProp(Vector3f(1.0f, 5.0f, 3.0f), Vector3f(0.0f, 90.0f, 0.0f), Vector3f(1.0f, 1.0f, 0.5f));
	int entityId = myAdmin.AddWorldProp(Vector3f(2.0f, 5.0f, 5.0f), Vector3f(0.0f, 0.0f, 90.0f), Vector3f(0.75f, 1.0f, 1.5f));

	int otherEntityId = myAdmin.AddCamera(Vector3f(1.0f, 5.0f, 3.0f), Vector3f(0.0f, 90.0f, 0.0f), Vector3f(1.0f, 1.0f, 0.5f), Vector3f(0.0f, 0.0f, 1.0f), Vector3f(0.0f, 45.0f, 0.0f));

	myAdmin.UpdateSystems();

	//TransformComponent *testGetter = myAdmin.GetComponent(entityId, ComponentSet::Transform);
	TransformComponent *testGetter = myAdmin.GetComponent<TransformComponent>(entityId);
	CameraComponent *camGetter = myAdmin.GetComponent<CameraComponent>(otherEntityId);

//	myAdmin.DeleteEntity(entityId);

	myAdmin.RemoveComponents(ComponentSet::Transform | ComponentSet::Camera, otherEntityId);
//	myAdmin.RemoveComponents(ComponentSet::Camera, otherEntityId);

	// ADD UNIT TESTS!!!!!!
	// VALIDATE GET COMPONENT WITH THEIR VALUES, ALSO ENTITY IDs


	return 0;
}
