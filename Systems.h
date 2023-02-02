#ifndef SYSTEMS_H
#define SYSTEMS_H

#include "Archetypedata.h"


struct ISystem
{
	ISystem(const ComponentSet pComponentSet) : mComponentSet(pComponentSet) { }

	virtual void Update(ArchetypeData &pComponentsData) = 0;

	ComponentSet mComponentSet;
};

struct WorldPropsSystem : public ISystem
{
	WorldPropsSystem(const ComponentSet pComponentSet) : ISystem(pComponentSet) { }

	void Update(ArchetypeData &pComponentsData);
};

struct CameraSystem : public ISystem
{
	CameraSystem(const ComponentSet pComponentSet) : ISystem(pComponentSet) { }

	void Update(ArchetypeData &pComponentsData);
};

#endif
