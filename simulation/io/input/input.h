#ifndef SIMULATION_ROBOT_INPUT_H
#define SIMULATION_ROBOT_INPUT_H

#include "../../pscommon/robotInput.h"
#include "../robot/robot.h"

void fillBufferWithWheelsPwm(wheelsPwmInputStruct* buffer, robotStruct* robot);
int createWheelsPwmInputFifo();

void* tReadWheelsPwmInputThreadFunc(void *cookie);

#endif // SIMULATION_ROBOT_INPUT_H
