#include "encoder.h"

#include <math.h>
#include <stdio.h>

#define INCREMENT_MODE      1
#define DECREMENT_MODE      2

#define TRUE                0x01
#define FALSE               0x00

void updateEncoder(encoderStruct* enc, int angle, pthread_barrier_t* outputBarrier)
{
    // printf("updateEncoder: start\n");
    // printf("updateEncoder: start: sigA:%u, sigB:%u, angle:%d, lastAngle:%d\n", enc->sigA, enc->sigB, angle, enc->lastAngle);

    uint8_t isIncrementing = ((angle == 0) && (enc->lastAngle == 359)) ? 1 : ((angle - enc->lastAngle) == 1);
    uint8_t isDecrementing = ((angle == 0) && (enc->lastAngle == -359)) ? -1 : ((enc->lastAngle - angle) == 1);

    if (isIncrementing)
    // if (angle == 1)
    {
        // printf("updateEncoder: angle:%d, lastAngle:%d, (angle - enc->lastAngle):%d\n", 
        //         angle,
        //         enc->lastAngle,
        //         angle - enc->lastAngle);
        incrementEncoder(enc);
        enc->lastAngle = angle;
        enc->updateSignals = TRUE;
    }
    else if (isDecrementing)
    // else if (angle == -1)
    {
        // printf("updateEncoder: angle:%d, lastAngle:%d, (enc->lastAngle - angle):%d\n",
        //         angle,
        //         enc->lastAngle,
        //         enc->lastAngle - angle);
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
    // printf("updateEncoder: end\n");
    // pthread_barrier_wait(outputBarrier);
    // printf("updateEncoder: end: \tsigA:%u, sigB:%u, angle:%d\n", enc->sigA, enc->sigB, angle);
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
        // if (!tempSigB && !tempSigA)
        // {
        //     tempSigA = 1;  
        // }
        // else if (!tempSigB && tempSigA)
        // {
        //     tempSigA = 1;
        // }
        // else if (tempSigB && tempSigA)
        // {
        //     tempSigA = 0;
        // }
        // else if (tempSigB && !tempSigA)
        // {
        //     tempSigA = 0;
        // }
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
