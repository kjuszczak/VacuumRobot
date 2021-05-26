#include "encoder.h"

#include <math.h>
#include <stdio.h>

#define INCREMENT_MODE      1
#define DECREMENT_MODE      2

#define TRUE                0x01
#define FALSE               0x00

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
        enc->updateSignals = TRUE;
    }
    else if (enc->lastAngle - angle >= 1)
    {
        decrementEncoder(enc);
        enc->lastAngle = angle;
        enc->updateSignals = TRUE;
    }
    else if (enc->updateSignals && enc->mode == INCREMENT_MODE)
    {
        incrementEncoder(enc);
        enc->updateSignals = FALSE;
    }
    else if (enc->updateSignals && enc->mode == DECREMENT_MODE)
    {
        decrementEncoder(enc);
        enc->updateSignals = FALSE;
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
    *sigB = !(*sigB);
}

void incrementEncoder(encoderStruct* enc)
{
    pthread_mutex_lock(enc->encoderMutex);
    uint8_t tempSigA = enc->sigA;
    uint8_t tempSigB = enc->sigB;
    pthread_mutex_unlock(enc->encoderMutex);

    if (!enc->mode)
    {
        tempSigA = TRUE;
        tempSigB = FALSE;
        enc->counter = 1;
        enc->mode = INCREMENT_MODE;
    }
    else if (enc->mode == DECREMENT_MODE)
    {
        if(!enc->counter)
        {
            tempSigA = !tempSigA;
            tempSigB = !tempSigB;
        }
        enc->counter = enc->counter ? 0 : 1;
        enc->mode = INCREMENT_MODE;
    }
    else
    {
        if (!tempSigB)
        {
            tempSigA = TRUE;  
        }
        else
        {
            tempSigA = FALSE;
        }
        updateSigB(enc, &tempSigB);
    }
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

    if (!enc->mode)
    {
        tempSigA = TRUE;
        tempSigB = TRUE;
        enc->counter = 1;
        enc->mode = DECREMENT_MODE;
    }
    else if (enc->mode == INCREMENT_MODE)
    {
        if(!enc->counter)
        {
            tempSigA = !tempSigA;
            tempSigB = !tempSigB;
        }
        enc->counter = enc->counter ? 0 : 1;
        enc->mode = DECREMENT_MODE;    
    }
    else
    {
        if (!tempSigB)
        {
            tempSigA = FALSE;  
        }
        else
        {
            tempSigA = TRUE;
        }
        updateSigB(enc, &tempSigB);
    }
    pthread_mutex_lock(enc->encoderMutex);
    enc->sigA = tempSigA;
    enc->sigB = tempSigB;
    pthread_mutex_unlock(enc->encoderMutex);
}
