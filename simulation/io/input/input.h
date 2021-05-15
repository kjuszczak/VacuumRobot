#ifndef SIMULATION_ROBOT_INPUT_H
#define SIMULATION_ROBOT_INPUT_H

#include "../../pscommon/robotInput.h"
#include "../robot/robot.h"

typedef struct wheelsPwmInputSimProcess
{
    wheelsPwmInputStruct* wheelsPwmInputData;
    robotStruct* robot;
} wheelsPwmInputSimProcessStruct;

void fillBufferWithWheelsPwm(wheelsPwmInputStruct* buffer, robotStruct* robot);

void* tReadWheelsPwmInputThreadFunc(void *cookie);

#endif // SIMULATION_ROBOT_INPUT_H
