#ifndef ENCODER_CONTROLLER_H
#define ENCODER_CONTROLLER_H

#include "stdint.h"
#include <pthread.h>
#include <signal.h>

typedef struct encoder
{
    uint8_t sigA;
    uint8_t sigB;
    uint8_t lastSigA;
    int counter;
    int angle;
    pthread_mutex_t* encoderMutex;
    pthread_barrier_t* encoderBarrier;
    pthread_barrier_t* encodersCalculationBarrier;
} encoderStruct;

void calculateAngle(encoderStruct* encoder);

#endif // CONTROLLEENCODER_CONTROLLER_HR_H
