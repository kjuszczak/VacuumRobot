#include "doors.h"
#include <unistd.h>

uint8_t isDoorOnWay(const doorsStruct* doors, int xDirectory, int yDirectory, uint16_t robotX, uint16_t robotY)
{
    for (size_t i = 0; i < doors->numOfDoors; i++)
    {
        if ((xDirectory == 1) && (yDirectory == 0))
        {
            if (doors->doors[i]->startXY[0] >= robotX)
            {
                if ((doors->doors[i]->startXY[1] < robotY) && (doors->doors[i]->endXY[1] > robotY))
                {
                    return 1;
                }
            }
        }
        else if ((xDirectory == -1) && (yDirectory == 0))
        {
            if (doors->doors[i]->startXY[0] <= robotX)
            {
                if ((doors->doors[i]->startXY[1] < robotY) && (doors->doors[i]->endXY[1] > robotY))
                {
                    return 1;
                }
            }
        }
        else if ((xDirectory == 0) && (yDirectory == 1))
        {
            if (doors->doors[i]->startXY[1] >= robotY)
            {
                if ((doors->doors[i]->startXY[0] < robotX) && (doors->doors[i]->endXY[0] > robotX))
                {
                    return 1;
                }
            }
        }
        else if ((xDirectory == 0) && (yDirectory == -1))
        {
            if (doors->doors[i]->startXY[1] <= robotY)
            {
                if ((doors->doors[i]->startXY[0] < robotX) && (doors->doors[i]->endXY[0] > robotX))
                {
                    return 1;
                }
            }
        }
    }
    return 0;
}

