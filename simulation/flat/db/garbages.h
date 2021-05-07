#include "stdint.h"

typedef struct garbage
{
    uint16_t x;
    uint16_t y;
} garbageStruct;

typedef struct garbages
{
    garbageStruct** garbages;
    uint16_t numOfGarbages;
} garbagesStruct;

void allocateGarbage(garbagesStruct* garbages, uint16_t x, uint16_t y);
void deleteGarbage(garbagesStruct* garbages, uint16_t x, uint16_t y);
void deleteGarbageIfVacuumed(garbagesStruct* garbages, uint16_t x, uint16_t y);