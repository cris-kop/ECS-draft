#ifndef MAIN_H
#define MAIN_H

#include "vector3.h"
#include "EntityAdmin.h"

extern EntityAdmin myAdmin;
extern std::vector<int> entityIds;

bool RunTests(const bool pStopAtFailed);

#endif
