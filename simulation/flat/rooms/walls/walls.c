#include "walls.h"
#include <stdio.h>
#include <math.h>

#include "../../pscommon/constants.h"

float getDistanceFromWall(const wallsStruct* wallsInRoom, const doorsStruct* doors, uint16_t robotX, uint16_t robotY, float robotAngle)
{
    float d1, d2, dMin;
    double x, y;
    if (robotAngle < 90)
    {
        d1 = (robotY - wallsInRoom->walls[3].startXY[1]) / sin(robotAngle * RADIAN_PROP);
        d2 = (wallsInRoom->walls[2].startXY[0] - robotX) / cos(robotAngle * RADIAN_PROP);
        dMin = d1 <= d2 ? d1 : d2;
        x = cos(robotAngle * RADIAN_PROP) * dMin + robotX;
        y = robotY - sin(robotAngle * RADIAN_PROP) * dMin;
    }
    else if (robotAngle >= 90 && robotAngle < 180)
    {
        d1 = (robotY - wallsInRoom->walls[3].startXY[1]) / cos((robotAngle - 90) * RADIAN_PROP);
        d2 = (robotX - wallsInRoom->walls[0].startXY[0]) / sin((robotAngle - 90) * RADIAN_PROP);
        dMin = d1 <= d2 ? d1 : d2;
        x = robotX - sin((robotAngle - 90) * RADIAN_PROP) * dMin;
        y = robotY - cos((robotAngle - 90) * RADIAN_PROP) * dMin;
    }
    else if (robotAngle >= 180 && robotAngle < 270)
    {
        d1 = (robotX - wallsInRoom->walls[0].startXY[0]) / cos((robotAngle - 180) * RADIAN_PROP);
        d2 = (wallsInRoom->walls[1].startXY[1] - robotY) / sin((robotAngle - 180) * RADIAN_PROP);
        dMin = d1 <= d2 ? d1 : d2;
        x = robotX - cos((robotAngle - 180) * RADIAN_PROP) * dMin;
        y = robotY + sin((robotAngle - 180) * RADIAN_PROP) * dMin;
    }
    else if (robotAngle >= 270 && robotAngle < 360)
    {
        d1 = (wallsInRoom->walls[1].startXY[1] - robotY) / cos((robotAngle - 270) * RADIAN_PROP);
        d2 = (wallsInRoom->walls[2].startXY[0] - robotX) / sin((robotAngle - 270) * RADIAN_PROP);
        dMin = d1 <= d2 ? d1 : d2;
        x = robotX + sin((robotAngle - 270) * RADIAN_PROP) * dMin;
        y = robotY + cos((robotAngle - 270) * RADIAN_PROP) * dMin;
    }
    return isDoorOnWay(doors, x, y) ? -1 : dMin - ROBOT_RADIUS;
}