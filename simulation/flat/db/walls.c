#include "walls.h"
#include <stdio.h>
#include <math.h>

uint16_t getDistanceFromWall(const wallStruct* walls, int xDirectory, int yDirectory, uint16_t robotX, uint16_t robotY)
{
    if ((xDirectory == 1) && (yDirectory == 0))
    {
        return sqrt((walls[3].startXY[0] - robotX) * (walls[3].startXY[0] - robotX));
    }
    else if ((xDirectory == -1) && (yDirectory == 0))
    {
        return sqrt((walls[1].startXY[0] - robotX) * (walls[1].startXY[0] - robotX));
    }
    else if ((xDirectory == 0) && (yDirectory == 1))
    {
        return sqrt((walls[2].startXY[1] - robotY) * (walls[2].startXY[1] - robotY));
    }
    else if ((xDirectory == 0) && (yDirectory == -1))
    {
        return sqrt((walls[4].startXY[1] - robotY) * (walls[4].startXY[1] - robotY));
    }
    fprintf(stderr, "Wrong directory.\n");
    return 0;
}