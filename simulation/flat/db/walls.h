#include <stdint.h>

typedef struct wall
{
    uint16_t startXY[2];
    uint16_t endXY[2];
} wallStruct;

typedef struct walls
{
    wallStruct* walls;
    uint16_t numOfWalls;
} wallsStruct;