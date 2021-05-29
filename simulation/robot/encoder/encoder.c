#include "encoder.h"

#include <math.h>
#include <stdio.h>

#define INCREMENT_MODE      1
#define DECREMENT_MODE      2

#define TRUE                0x01
#define FALSE               0x00

void updateEncoder(encoderStruct* enc, int angle, pthread_barrier_t* outputBarrier)
{
    uint8_t isIncrementing = ((angle == 0) && (enc->lastAngle == 359)) ? 1 : ((angle - enc->lastAngle) == 1);
    uint8_t isDecrementing = ((angle == 0) && (enc->lastAngle == -359)) ? -1 : ((enc->lastAngle - angle) == 1);

    if (enc->updateSignals && enc->mode == INCREMENT_MODE)
    {
        updateSigB(enc, &enc->sigB);
        enc->updateSignals = FALSE;
    }
    else if (enc->updateSignals && enc->mode == DECREMENT_MODE)
    {
        updateSigB(enc, &enc->sigB);
        enc->updateSignals = FALSE;
    }
    else if (isIncrementing)
    {
        incrementEncoder(enc);
        enc->lastAngle = angle;
        enc->updateSignals = TRUE;
        // printf("updateEncoder: increment: \tsigA:%u, sigB:%u, angle:%d\n", enc->sigA, enc->sigB, angle);
    }
    else if (isDecrementing)
    {
        decrementEncoder(enc);
        enc->lastAngle = angle;
        enc->updateSignals = TRUE;
        // printf("updateEncoder: decrement: \tsigA:%u, sigB:%u, angle:%d\n", enc->sigA, enc->sigB, angle);
    }
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
    if (!enc->mode)
    {
        enc->sigA = TRUE;
        enc->sigB = FALSE;
        enc->counter = 1;
        enc->mode = INCREMENT_MODE;
    }
    else if (enc->mode == DECREMENT_MODE)
    {
        if(!enc->counter)
        {
            enc->sigA = !enc->sigA;
            enc->sigB = !enc->sigB;
        }
        enc->counter = enc->counter ? 0 : 1;
        enc->mode = INCREMENT_MODE;
    }
    else
    {
        if (!enc->sigB)
        {
            enc->sigA = TRUE;  
        }
        else
        {
            enc->sigA = FALSE;
        }
        updateSigB(enc, &enc->sigB);
    }
}

void decrementEncoder(encoderStruct* enc)
{
    if (!enc->mode)
    {
        enc->sigA = TRUE;
        enc->sigB = TRUE;
        enc->counter = 1;
        enc->mode = DECREMENT_MODE;
    }
    else if (enc->mode == INCREMENT_MODE)
    {
        if(!enc->counter)
        {
            enc->sigA = !enc->sigA;
            enc->sigB = !enc->sigB;
        }
        enc->counter = enc->counter ? 0 : 1;
        enc->mode = DECREMENT_MODE;    
    }
    else
    {
        if (!enc->sigB)
        {
            enc->sigA = FALSE;  
        }
        else
        {
            enc->sigA = TRUE;
        }
        updateSigB(enc, &enc->sigB);
    }
}
