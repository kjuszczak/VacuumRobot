#include "encoder.h"

#include <stdio.h>

#include "../../pscommon/constants.h"

void calculateAngle(encoderStruct* encoder)
{
    uint8_t sigA, sigB, lastSigA;
    int counter, angle;

    pthread_mutex_lock(encoder->encoderMutex);
    sigA = encoder->sigA;
    sigB = encoder->sigB;
    lastSigA = encoder->lastSigA;
    counter = encoder->counter;
    pthread_mutex_unlock(encoder->encoderMutex);

    // printf("calculateAngle \t\t\t\tstart: sigA:%u, sigB:%u\n", sigA, sigB);

    if (sigA == lastSigA)
    {
        return;
    }
    if (sigA != sigB)
    {
        if (counter < (ENCODER_RESOLUTION - 1))
        {
            counter++;
        }
        else
        {
            counter = 0;
        }
    } 
    else
    {
        if (counter > -(ENCODER_RESOLUTION - 1))
        {
            counter--;
        }
        else
        {
            counter = 0;
        }
    }

    // counter = counter % ENCODER_RESOLUTION;
    angle = ((360 * counter) / ENCODER_RESOLUTION);
    // printf("calculateAngle: \tsigA:%u, sigB:%u, angle:%d\n", sigA, sigB, angle);

    pthread_mutex_lock(encoder->encoderMutex);
    encoder->lastSigA = sigA;
    encoder->counter = counter;
    encoder->angle = angle;
    pthread_mutex_unlock(encoder->encoderMutex);
}