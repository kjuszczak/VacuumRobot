#include "encoder.h"

#include <stdio.h>

void updateEncoder(encoderStruct* enc, int angle)
{
    if (enc == NULL)
    {
        fprintf(stderr, "Encoder is null\n");
        return;
    }

    if (angle - enc->lastAngle >= 1)
    {
        incrementEncoder(enc);
        enc->lastAngle = angle;
    }
    else if (enc->lastAngle - angle >= 1)
    {
        decrementEncoder(enc);
        enc->lastAngle = angle;
    }
}

void updateSigB(encoderStruct* enc, uint8_t* sigB)
{
    if (enc->counter < 1)
    {
        enc->counter++;
        return;
    }
    else
    {
        enc->counter = 0;
    }
    *sigB = *sigB ? 0 : 1;
}

void incrementEncoder(encoderStruct* enc)
{
    pthread_mutex_lock(enc->encoderMutex);
    uint8_t tempSigA = enc->sigA;
    uint8_t tempSigB = enc->sigB;
    pthread_mutex_unlock(enc->encoderMutex);

    if (!tempSigB && !tempSigA)
    {
        tempSigA = 1;  
    }
    else if (!tempSigB && tempSigA)
    {
        tempSigA = 1;
    }
    else if (tempSigB && tempSigA)
    {
        tempSigA = 0;
    }
    else if (tempSigB && !tempSigA)
    {
        tempSigA = 0;
    }
    updateSigB(enc, &tempSigB);

    pthread_mutex_lock(enc->encoderMutex);
    enc->sigA = tempSigA;
    enc->sigB = tempSigB;
    pthread_mutex_unlock(enc->encoderMutex);
}

void decrementEncoder(encoderStruct* enc)
{
    pthread_mutex_lock(enc->encoderMutex);
    uint8_t tempSigA = enc->sigA;
    uint8_t tempSigB = enc->sigB;
    pthread_mutex_unlock(enc->encoderMutex);

    if (!tempSigB && tempSigA)
    {
        tempSigA = 0;  
    }
    else if (!tempSigB && !tempSigA)
    {
        tempSigA = 0;
    }
    else if (tempSigB && !tempSigA)
    {
        tempSigA = 1;
    }
    else if (tempSigB && tempSigA)
    {
        tempSigA = 1;
    }
    updateSigB(enc, &tempSigB);

    pthread_mutex_lock(enc->encoderMutex);
    enc->sigA = tempSigA;
    enc->sigB = tempSigB;
    pthread_mutex_unlock(enc->encoderMutex);
}