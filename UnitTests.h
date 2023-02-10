#ifndef UNIT_TESTS_H
#define UNIT_TESTS_H

#include <string>

namespace UnitTests
{

bool AttachComponents();
bool SystemUpdate();
bool DuplicateEntity();
bool DeleteEntity();
bool RemoveComponent1();
bool RemoveComponent2();

void LogPassed(const bool pPassed, const std::string &pTestDesc);

};

#endif