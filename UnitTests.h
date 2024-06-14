#ifndef UNIT_TESTS_H
#define UNIT_TESTS_H

#include "ComponentSet.h"
#include "Main.h"

#include <string>

namespace UnitTests
{

void LogPassed(const bool pPassed, const std::string &pTestDesc);

bool ValidateCreateComponents();
bool ValidateSystemsUpdate();
bool ValidateDuplicateEntity(const unsigned int pSourceEntityId, const unsigned int pTargetEntityId);

template<typename T>
bool ValidateRemoveComponent(T pComponentRemoved, const ComponentSet pComponentExisting, const unsigned int pEntityId)
{
	bool passed = true;
	ComponentSet currSet = myAdmin.GetEntity(pEntityId)->GetComponentSet();

	// no error catching here
	ComponentSet componentRemovedType = T::sType;
	if(componentRemovedType == ComponentSet::None)
	{
		passed = false;
	}
	else
	{
		if(!Contains(currSet, pComponentExisting))
		{
			passed = false; 
		}
		if(Contains(currSet, componentRemovedType))
		{
			passed = false;
		}
	}
	if(myAdmin.GetComponent<T>(pEntityId) != nullptr)	{ passed = false; }

	LogPassed(passed, "Remove Component");
	return passed;
}

bool ValidateDeleteEntity(const unsigned int pEntityId);

};

#endif