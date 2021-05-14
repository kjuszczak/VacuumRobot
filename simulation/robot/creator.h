#ifndef ROBOT_CREATOR_H
#define ROBOT_CREATOR_H

#include "robot.h"
#include "../flat/db/rooms.h"
#include "../sockets/common/socket.h"

#include <json-c/json.h>
#include <pthread.h>

void init(tSocketData *socketData, roomsStruct* rooms);

void createThreadsForRobotSimulation(tSocketData *socketData, roomsStruct* rooms);

int createTimer();

int createRobotThreads();
int createJsonSenderThread();

int createSensorsThreads();
int createEncodersThreads();
int createRoomIdUpdaterThread();

/*
 * Threads functions
 */
void *tRobotJsonUpdateThreadFunc(void *cookie);

#endif // ROBOT_CREATOR_H
