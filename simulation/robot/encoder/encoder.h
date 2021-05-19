#ifndef ENCODER_H
#define ENCODER_H

#include <stdint.h>
#include <pthread.h>
#include <mqueue.h>


typedef struct encoder
{
    uint8_t sigA;
    uint8_t sigB;
    uint8_t lastSigA;
    uint8_t counter;
    double lastAngle;
    pthread_mutex_t* encoderMutex;
    pthread_barrier_t* encoderBarrier;
} encoderStruct;

void updateEncoder(encoderStruct* enc, double angle);
void updateSigB(encoderStruct* enc, uint8_t* sigB);
void incrementEncoder(encoderStruct* enc);
void decrementEncoder(encoderStruct* enc);

#endif // ENCODER_H
