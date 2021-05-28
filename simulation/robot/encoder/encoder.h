#ifndef ENCODER_H
#define ENCODER_H

#include <stdint.h>
#include <pthread.h>
#include <mqueue.h>


typedef struct encoder
{
    uint8_t sigA;
    uint8_t sigB;
    uint8_t counter;
    int lastAngle;
    uint8_t mode;
    uint8_t updateSignals;
    pthread_mutex_t* encoderMutex;
    pthread_barrier_t* encoderBarrier;
} encoderStruct;

void updateEncoder(encoderStruct* enc, int angle, pthread_barrier_t* outputBarrier);
void updateSigB(encoderStruct* enc, uint8_t* sigB);
void incrementEncoder(encoderStruct* enc);
void decrementEncoder(encoderStruct* enc);

#endif // ENCODER_H
