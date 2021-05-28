#include "controller.h"

#include <stdio.h>

#include "../io/output/output.h"
#include "encoder/encoder.h"

void *tMainControllerPeriodicThreadFunc(void *cookie)
{
    controllerStruct* controller = (controllerStruct*)cookie;

    for (;;)
    {
        pthread_barrier_wait(controller->mainPeriodicFuncBarrier);

        pthread_barrier_wait(controller->encodersOutputReaderBarrier);
        pthread_barrier_wait(controller->sensorsOutputReaderBarrier);
        // printf("tMainControllerPeriodicThreadFunc: \tleftWheelangle:%d, rightWheelangle:%d\n", controller->leftWheel->angle, controller->rightWheel->angle);
        // printf("tMainControllerPeriodicThreadFunc: sigA:%u, sigB:%u, angle:%d\n",
        //         controller->leftWheel->sigA,
        //         controller->leftWheel->sigB,
        //         controller->leftWheel->angle);
        // wheelsPwm.leftWheelPwm = 255;
        // wheelsPwm.rightWheelPwm = 255;

        // writeWheelsPwmToFifo(&wheelsPwm);
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