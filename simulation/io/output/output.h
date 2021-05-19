#ifndef SIMULATION_ROBOT_OUTPUT_H
#define SIMULATION_ROBOT_OUTPUT_H

#include "../../../pscommon/robotOutput.h"
#include "../robot/robot.h"

typedef struct sensorsOutputSimProcess
{
    sensorsOutputThreadStruct* sensorsOutputThread;
    robotStruct* robot;
} sensorsOutputSimProcessStruct;

typedef struct encodersOutputSimProcess
{
    encodersOutputThreadStruct* encodersOutputThread;
    robotStruct* robot;
} encodersOutputSimProcessStruct;

int createSharedMemoryForSensorsOutput(sensorsOutputThreadStruct* sensorsOutputDataThread);
int createSharedMemoryForEncodersOutput(encodersOutputThreadStruct* encodersOutputDataThread);

void fillBufferWithSensors(sensorsOutputStruct* buffer, robotStruct* robot);
void fillBufferWithEncoders(encodersOutputStruct* buffer, robotStruct* robot);

void* tWriteSensorsOutputThreadFunc(void *cookie);
void* tWriteEncodersOutputThreadFunc(void *cookie);

#endif // SIMULATION_ROBOT_OUTPUT_H
