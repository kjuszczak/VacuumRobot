#ifndef ROBOT_CREATOR_H
#define ROBOT_CREATOR_H

#include "robot.h"

#include "../flat/rooms/rooms.h"
#include "../sockets/socket.h"

void init(tSocketData *socketData, roomsStruct* rooms);
void clean();

void createThreadsForRobotSimulation(tSocketData *socketData, roomsStruct* rooms);

int createClockHandler();

int createRobotThreads();
int createJsonSenderThread();

int createMainPeriodicRobotThread();
int createSensorsThreads();
int createEncodersThreads();
int createRoomIdUpdaterThread();
int createGarbagesUpdaterThread();

int createRobotInputReaderThread();
int createSensorsOutputWriterThread();
int createEncodersOutputWriterThread();

/*
 * Threads functions
 */
void *tClockHandlerThreadFunc(void *cookie);
void *tRobotJsonUpdateThreadFunc(void *cookie);

#endif // ROBOT_CREATOR_H
