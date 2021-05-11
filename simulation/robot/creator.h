#ifndef ROBOT_CREATOR_H
#define ROBOT_CREATOR_H

#include "robot.h"
#include "../flat/db/rooms.h"
#include "../sockets/common/socket.h"

#include <json-c/json.h>
#include <pthread.h>

void createThreadsForRobotSimulation(tSocketData *socketData, roomsStruct* rooms);
int createThreadForUpdatingJson(tSocketData *socketData);
int createTimer();

/*
 * Threads functions
 */
void *tTimerThreadFunc(void *cookie);

void *tRobotJsonUpdateThreadFunc(void *cookie);
void *tUpdateRobotParametersThreadFunc(void *cookie);

#endif // ROBOT_CREATOR_H
