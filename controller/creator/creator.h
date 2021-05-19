#ifndef CONTROLLER_CREATOR_H
#define CONTROLLER_CREATOR_H

#define CONTROLLER_SAMPLE_TIME 0.001

void createThreadsForController();

int createTimer();

void *tMainControllerPeriodicThreadFunc(void *cookie);

#endif // CONTROLLER_CREATOR_H
