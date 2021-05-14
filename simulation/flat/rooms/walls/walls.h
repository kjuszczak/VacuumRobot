#ifndef WALLS_H
#define WALLS_H

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

float getDistanceFromWall(const wallsStruct* walls, int xDirectory, int yDirectory, uint16_t robotX, uint16_t robotY);

#endif // WALLS_H
