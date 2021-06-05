#include "robot.h"

#include <stdio.h>

#include "../../../pscommon/constants.h"

#define EPS 0.001

double goalDistance = 0;
double counter = 0;
int lastWheelAngle = 0;

void startRotating(double goal)
{
    goalDistance = goal * RADIAN_PROP * ROBOT_RADIUS_CM; // [cm]
    counter = 0;
}

void startStartMoving(double goal)
{
    goalDistance = goal;
    counter = 0;
}

uint8_t isGoalReached(double wheelAngle)
{
    if ((wheelAngle == 0) && (lastWheelAngle == 359))
    {
        counter++;
    }
    lastWheelAngle = wheelAngle;

    double distance = (counter * 360 + wheelAngle) * RADIAN_PROP * ((WHEEL_DIAMATER_M * 100) / 2); // [cm]

    return (goalDistance - distance) <= EPS;
}
