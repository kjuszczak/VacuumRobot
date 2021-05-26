#ifndef CONTROLLER_ROBOT_OUTPUT_H
#define CONTROLLER_ROBOT_OUTPUT_H

#include "../../../pscommon/robotOutput.h"

typedef struct sensorsOutputControllerProcess
{
    sensorsOutputThreadStruct* sensorsOutputThread;
} sensorsOutputControllerProcessStruct;

int createSharedMemoryForSensorsOutput(sensorsOutputThreadStruct* sensorsOutputDataThread);
int createSharedMemoryForEncodersOutput(encodersOutputThreadStruct* encodersOutputDataThread);

void readSensors(sensorsOutputThreadStruct* sensorsOutput);
void readEncoders(encodersOutputThreadStruct* encodersOutputDataThread);

void decodeEncoders(encodersOutputThreadStruct* encodersOutputDataThread);

void* tReadSensorsOutputThreadFunc(void *cookie);
void* tReadEncodersOutputThreadFunc(void *cookie);

#endif // CONTROLLER_ROBOT_OUTPUT_H
