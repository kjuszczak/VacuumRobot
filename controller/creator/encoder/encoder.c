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

    angle = ((360 * counter) / ENCODER_RESOLUTION);

    pthread_mutex_lock(encoder->encoderMutex);
    encoder->lastSigA = sigA;
    encoder->counter = counter;
    encoder->angle = angle;
    pthread_mutex_unlock(encoder->encoderMutex);
}