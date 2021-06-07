#include "controller.h"

#include <stdio.h>
#include <stdint.h>

#include "../io/input/input.h"
#include "../io/output/output.h"
#include "encoder/encoder.h"
#include "robot/robot.h"

#include "../../pscommon/logger/log.h"

#define MIN_DISTANCE        10 // [cm]
#define FORWARD_DISTANCE    25 // [cm]

#define NO_ROTATION         0
#define TURNING_BACK        1
#define SIMPLE_ROTATION     2

#define NO_MOVEMENT         0
#define FORWARD_MOVEMENT    1

#define RIGHT               1
#define LEFT                2

uint8_t isRightRotating = 0;
uint8_t isLeftRotating = 0;
uint8_t isBackwardDriving = 0;

/*
    @rotatingMode - mode of rotation:
        - 0 - no rotation
        - 1 - turning back
        - 2 - simple rotation
*/
uint8_t rotatingMode = NO_ROTATION;

/*
    @forwardMode - mode of forward movement if rotatingMode == 1
        0 - no movement
        1 - forward movement
*/
uint8_t forwardMode = NO_MOVEMENT;

/*
    @lastRotation - direction of last rotation:
        - 0 - no rotation
        - 1 - right
        - 2 - left
*/
uint8_t lastRotation = NO_ROTATION;

void *tMainControllerPeriodicThreadFunc(void *cookie)
{
    controllerStruct* controller = (controllerStruct*)cookie;
    startRotating(180);
    for (;;)
    {
        pthread_barrier_wait(controller->mainPeriodicFuncBarrier);

        pthread_barrier_wait(controller->encodersOutputReaderBarrier);
        pthread_barrier_wait(controller->sensorsOutputReaderBarrier);

        driveRobot(controller);
        writeWheelsPwmToFifo(controller->leftWheelPwm, controller->rightWheelPwm);
    }
}

void *tEncoderAngleUpdaterThreadFunc(void *cookie)
{
    encoderStruct* encoder = (encoderStruct*)cookie;
    for (;;)
    {
        pthread_barrier_wait(encoder->encoderBarrier);
        calculateAngle(encoder);
        pthread_barrier_wait(encoder->encodersCalculationBarrier);
    }
}

void driveRobot(controllerStruct* controller)
{
    pthread_mutex_lock(controller->leftWheel->encoderMutex);
    int leftWheelAngle = controller->leftWheel->angle;
    pthread_mutex_unlock(controller->leftWheel->encoderMutex);

    pthread_mutex_lock(controller->rightWheel->encoderMutex);
    int rightWheelAngle = controller->rightWheel->angle;
    pthread_mutex_unlock(controller->rightWheel->encoderMutex);

    if (checkRotating(leftWheelAngle, rightWheelAngle))
    {
        return;
    }
    
    if (isDistanceAllowed(controller->sensors[0]) && !isBackwardDriving)
    {
        if (rotatingMode)
        {
            if (forwardMode && !isGoalReached(leftWheelAngle))
            {
                controller->leftWheelPwm = PWM_VALUE;
                controller->rightWheelPwm = PWM_VALUE;
                return;
            }
        }
        else
        {
            controller->leftWheelPwm = PWM_VALUE;
            controller->rightWheelPwm = PWM_VALUE;
            return;
        }
    }

    if (isDistanceAllowed(controller->sensors[2]) && (lastRotation != RIGHT))
    {
        controller->leftWheelPwm = PWM_VALUE;
        controller->rightWheelPwm = -PWM_VALUE;
        isRightRotating = 1;
        startRotating(90);
        pthread_mutex_lock(controller->leftWheel->encoderMutex);
        controller->leftWheel->counter = 0;
        pthread_mutex_unlock(controller->leftWheel->encoderMutex);
        return;
    }

    if (isDistanceAllowed(controller->sensors[3]) && (lastRotation != LEFT))
    {
        controller->leftWheelPwm = -PWM_VALUE;
        controller->rightWheelPwm = PWM_VALUE;
        isLeftRotating = 1;
        startRotating(90);
        pthread_mutex_lock(controller->rightWheel->encoderMutex);
        controller->rightWheel->counter = 0;
        pthread_mutex_unlock(controller->rightWheel->encoderMutex);
        return;
    }

    if (lastRotation)
    {
        rotatingMode = SIMPLE_ROTATION;
        lastRotation = lastRotation == RIGHT ? LEFT : RIGHT;
        forwardMode = NO_MOVEMENT;
        return;
    }

    if (isDistanceAllowed(controller->sensors[1]))
    {
        controller->leftWheelPwm = -PWM_VALUE;
        controller->rightWheelPwm = -PWM_VALUE;
        isBackwardDriving = 1;
        return;
    }

    controller->leftWheelPwm = 0;
    controller->rightWheelPwm = 0;
}

uint8_t isDistanceAllowed(double distance)
{
    return (distance > MIN_DISTANCE) || (distance == -1);
}

uint8_t checkRotating(int leftWheelAngle, int rightWheelAngle)
{
    if (isRightRotating)
    {
        if (!isGoalReached(leftWheelAngle))
        {
            return 1;
        }
        isRightRotating = 0;
        if (!rotatingMode)
        {
            rotatingMode = TURNING_BACK;
            startStartMoving(FORWARD_DISTANCE);
            forwardMode = FORWARD_MOVEMENT;
        }
        else
        {
            lastRotation = (rotatingMode == TURNING_BACK) ? RIGHT : lastRotation;
            rotatingMode = NO_ROTATION;
            forwardMode = NO_MOVEMENT;
        }
    }
    else if (isLeftRotating)
    {
        if (!isGoalReached(rightWheelAngle))
        {
            return 1;
        }
        isLeftRotating = 0;
        if (!rotatingMode)
        {
            rotatingMode = TURNING_BACK;
            startStartMoving(FORWARD_DISTANCE);
            forwardMode = FORWARD_MOVEMENT;
        }
        else
        {
            lastRotation = (rotatingMode == TURNING_BACK) ? LEFT : lastRotation;
            rotatingMode = NO_ROTATION;
            forwardMode = NO_MOVEMENT;
        }
    }
    return 0;
}
