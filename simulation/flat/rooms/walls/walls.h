#ifndef WALLS_H
#define WALLS_H

#include "../doors/doors.h"
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

float getDistanceFromWall(const wallsStruct* wallsInRoom, const doorsStruct* doors, uint16_t robotX, uint16_t robotY, float robotAngle);

#endif // WALLS_H
