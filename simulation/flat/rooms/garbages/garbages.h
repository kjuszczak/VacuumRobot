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
    garbageStruct* garbages;
    uint16_t numOfGarbages;
    pthread_mutex_t* garbagesMutex;
} garbagesStruct;

void createGarbagesInRoom(garbagesStruct* garbages, uint16_t leftTopX, uint16_t leftTopY, uint16_t rightDownX, uint16_t rightDownY);

void allocateGarbage(garbagesStruct* garbages, uint16_t x, uint16_t y, uint16_t weight);
void deleteGarbage(garbagesStruct* garbages, size_t indexToDelete);
void deleteGarbageIfVacuumed(garbagesStruct* garbages, uint16_t x, uint16_t y);
int findGarbage(garbagesStruct* garbages, uint16_t x, uint16_t y, uint16_t radius);
#endif // GARBAGES_H
