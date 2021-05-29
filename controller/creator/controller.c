#include "controller.h"

#include <stdio.h>

#include "../io/input/input.h"
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

        if (controller->sensors[0] > 10)
        {
            writeWheelsPwmToFifo(255, 255);
        }
        else
        {
            writeWheelsPwmToFifo(255, -255);
            // printf("STOP\n");
        }
        // printf("controller: sensors1:%lf, sensors2:%lf, sensors3:%lf, sensors4:%lf\n",
        //         controller->sensors[0],
        //         controller->sensors[1],
        //         controller->sensors[2],
        //         controller->sensors[3]);
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