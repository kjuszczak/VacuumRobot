#ifndef CONTROLLER_CREATOR_H
#define CONTROLLER_CREATOR_H

#include "../../pscommon/constants.h"

void createThreadsForController();

int createTimer();

void *tMainControllerPeriodicThreadFunc(void *cookie);

#endif // CONTROLLER_CREATOR_H
