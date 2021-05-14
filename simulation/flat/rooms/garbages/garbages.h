#ifndef GARBAGES_H
#define GARBAGES_H

#include "stdint.h"
#include <pthread.h>

typedef struct garbage
{
    uint16_t x;
    uint16_t y;
    uint16_t weight;
} garbageStruct;

typedef struct garbages
{
    garbageStruct** garbages;
    uint16_t numOfGarbages;
    pthread_mutex_t* garbagesMutex;
} garbagesStruct;

void allocateGarbage(garbagesStruct* garbages, uint16_t x, uint16_t y, uint16_t weight);
void deleteGarbage(garbagesStruct* garbages, uint16_t x, uint16_t y);
void deleteGarbageIfVacuumed(garbagesStruct* garbages, uint16_t x, uint16_t y);

#endif // GARBAGES_H
