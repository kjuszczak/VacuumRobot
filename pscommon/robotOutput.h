#ifndef ROBOT_OUTPUT_H
#define ROBOT_OUTPUT_H

#include <semaphore.h>
#include <stdint.h>

typedef struct sensorsOutput
{
    double sensors[4];
} sensorsOutputStruct;

typedef struct encodersOutput
{
    uint8_t leftEncoderSigA;
    uint8_t leftEncoderSigB;
    uint8_t rightEncoderSigA;
    uint8_t rightEncoderSigB;
} encodersOutputStruct;

typedef struct sensorsOutputThread
{
    sensorsOutputStruct* sensorsOutputData;
    sem_t *mutexSem;
    sem_t *spoolSem;
} sensorsOutputThreadStruct;

typedef struct encodersOutputThread
{
    encodersOutputStruct* encodersOutputData;
    sem_t *mutexSem;
    sem_t *spoolSem;
} encodersOutputThreadStruct;


#endif // ROBOT_OUTPUT_H
