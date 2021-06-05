#include "doors.h"

#include <math.h>
#include <unistd.h>

#include "../../pscommon/constants.h"

uint8_t isDoorOnWay(const doorsStruct* doors, float wallX, float wallY)
{
    wallX = round(wallX);
    wallY = round(wallY);

    for (size_t i = 0; i < doors->numOfDoors; i++)
    {
        if (wallX == doors->doors[i]->startXY[0] && wallX == doors->doors[i]->endXY[0])
        {
            if ((wallY - ROBOT_RADIUS_CM) > doors->doors[i]->startXY[1] && (wallY + ROBOT_RADIUS_CM) < doors->doors[i]->endXY[1])
            {
                return 1;
            }
        }
        if (wallY == doors->doors[i]->startXY[1] && wallY == doors->doors[i]->endXY[1])
        {
            if ((wallX - ROBOT_RADIUS_CM) > doors->doors[i]->startXY[0] && (wallX + ROBOT_RADIUS_CM) < doors->doors[i]->endXY[0])
            {
                return 1;
            }
        }
    }
    return 0;
}
