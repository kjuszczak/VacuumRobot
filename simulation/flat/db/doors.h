#include "stdint.h"

typedef struct door
{
    uint16_t startXY[2];
    uint16_t endXY[2];
} doorStruct;

typedef struct doors
{
    doorStruct* doors;
    uint16_t numOfDoors;
} doorsStruct;