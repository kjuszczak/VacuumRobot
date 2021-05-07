#include "stdint.h"

typedef struct door
{
    uint16_t startXY[2];
    uint16_t endXY[2];
} doorStruct;

typedef struct doors
{
    doorStruct** doors;
    uint16_t numOfDoors;
} doorsStruct;

uint8_t isDoorOnWay(const doorsStruct* doors, int xDirectory, int yDirectory, uint16_t robotX, uint16_t robotY);