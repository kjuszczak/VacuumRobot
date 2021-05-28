#ifndef CONTROLLER_ROBOT_OUTPUT_H
#define CONTROLLER_ROBOT_OUTPUT_H

#include "../../../pscommon/robotOutput.h"
#include "../creator/controller.h"

typedef struct sensorsOutputControllerProcess
{
    sensorsOutputThreadStruct* sensorsOutputThread;
    controllerStruct* controller;
} sensorsOutputControllerProcessStruct;

typedef struct encodersOutputControllerProcess
{
    encodersOutputThreadStruct* encodersOutputThread;
    controllerStruct* controller;
} encodersOutputControllerProcessStruct;

int createSharedMemoryForSensorsOutput(sensorsOutputThreadStruct* sensorsOutputDataThread);
int createSharedMemoryForEncodersOutput(encodersOutputThreadStruct* encodersOutputDataThread);

int unmapShmForSensors(sensorsOutputThreadStruct* sensorsOutputDataThread);
int closeSharedMemoryForSensorsOutput(sensorsOutputThreadStruct* sensorsOutputDataThread);

int unmapShmForEncoders(encodersOutputThreadStruct* encodersOutputDataThread);
int closeSharedMemoryForEncodersOutput(encodersOutputThreadStruct* encodersOutputDataThread);

void* tReadSensorsOutputThreadFunc(void *cookie);
void* tReadEncodersOutputThreadFunc(void *cookie);

#endif // CONTROLLER_ROBOT_OUTPUT_H
