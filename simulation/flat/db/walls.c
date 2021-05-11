#include "walls.h"
#include <stdio.h>
#include <math.h>

float getDistanceFromWall(const wallsStruct* wallsInRoom, int xDirectory, int yDirectory, uint16_t robotX, uint16_t robotY)
{
    if ((xDirectory == 1) && (yDirectory == 0))
    {
        return sqrt((wallsInRoom->walls[3].startXY[0] - robotX) * (wallsInRoom->walls[3].startXY[0] - robotX));
    }
    else if ((xDirectory == -1) && (yDirectory == 0))
    {
        return sqrt((wallsInRoom->walls[1].startXY[0] - robotX) * (wallsInRoom->walls[1].startXY[0] - robotX));
    }
    else if ((xDirectory == 0) && (yDirectory == 1))
    {
        return sqrt((wallsInRoom->walls[2].startXY[1] - robotY) * (wallsInRoom->walls[2].startXY[1] - robotY));
    }
    else if ((xDirectory == 0) && (yDirectory == -1))
    {
        return sqrt((wallsInRoom->walls[4].startXY[1] - robotY) * (wallsInRoom->walls[4].startXY[1] - robotY));
    }
    fprintf(stderr, "Wrong directory.\n");
    return 0;
}