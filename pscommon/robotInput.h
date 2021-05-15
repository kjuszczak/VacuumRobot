#ifndef ROBOT_INPUT_H
#define ROBOT_INPUT_H

#include <semaphore.h>

typedef struct wheelsPwmInput
{
    int leftWheelPwm;
    int rightWheelPwm;
} wheelsPwmInputStruct;

#endif // ROBOT_INPUT_H
