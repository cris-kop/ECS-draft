#ifndef UNIT_TESTS_H
#define UNIT_TESTS_H

#include <string>

namespace UnitTests
{

bool ValidateCreateComponents();
bool ValidateSystemsUpdate();
bool ValidateDuplicateEntity(const unsigned int pSourceEntityId, const unsigned int pTargetEntityId);
bool ValidateDeleteEntity(const unsigned int pEntityId);
//bool RemoveComponent1();
//bool RemoveComponent2();*/

void LogPassed(const bool pPassed, const std::string &pTestDesc);

};

#endif