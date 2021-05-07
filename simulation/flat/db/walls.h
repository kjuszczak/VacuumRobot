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

uint16_t getDistanceFromWall(const wallStruct* walls, int xDirectory, int yDirectory, uint16_t robotX, uint16_t robotY);