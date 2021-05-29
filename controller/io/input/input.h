#ifndef CONTROLLER_ROBOT_INPUT_H
#define CONTROLLER_ROBOT_INPUT_H

#include "../../../pscommon/robotInput.h"

int writeWheelsPwmToFifo(int leftWheelPwm, int rightWheelPwm);
void* tWriteWheelsPwmInputThreadFunc(void *cookie);

#endif // CONTROLLER_ROBOT_INPUT_H