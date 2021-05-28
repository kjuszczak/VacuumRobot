#ifndef CONTROLLER_CREATOR_H
#define CONTROLLER_CREATOR_H

#include "../../pscommon/constants.h"

void init();
void clean();

int createClockHandler();

void createThreadsForController();

void createMainPeriodicThread();

void createThreadForEncodersAngleUpdater();
void createThreadForEncodersReader();
void createThreadForSensorsReader();

void *tClockHandlerThreadFunc(void *cookie);

#endif // CONTROLLER_CREATOR_H
