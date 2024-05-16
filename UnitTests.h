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
	ComponentSet currSet = myAdmin.GetEntity(pEntityId).mComponentSet;

	// no error catching here
	ComponentSet componentRemovedType = ComponentSet::None;

	std::unordered_map<std::type_index, ComponentSet>::const_iterator setIt = gCOMPONENT_MAP.find(typeid(T));
	if(setIt != gCOMPONENT_MAP.end())
	{
		componentRemovedType = setIt->second;
	}

	if(!Contains(currSet, pComponentExisting))
	{
		passed = false; 
	}
	if(Contains(currSet, componentRemovedType))
	{
		passed = false;
	}

	if(myAdmin.GetComponent<T>(pEntityId) != nullptr)	{ passed = false; }

	LogPassed(passed, "Remove Component");
	return passed;
}

bool ValidateDeleteEntity(const unsigned int pEntityId);

};

#endif