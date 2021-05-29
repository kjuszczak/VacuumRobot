#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "stdint.h"
#include <pthread.h>

#include "../../pscommon/constants.h"
#include "encoder/encoder.h"

typedef struct controller
{
    encoderStruct* leftWheel;
    encoderStruct* rightWheel;
    double* sensors;
    pthread_barrier_t* mainPeriodicFuncBarrier;
    pthread_barrier_t* encodersOutputReaderBarrier;
    pthread_barrier_t* encodersCalculationBarrier;
    pthread_barrier_t* sensorsOutputReaderBarrier;
} controllerStruct;

void *tMainControllerPeriodicThreadFunc(void *cookie);
void *tEncoderAngleUpdaterThreadFunc(void *cookie);

#endif // CONTROLLER_H
