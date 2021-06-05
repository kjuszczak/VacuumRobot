#ifndef ROBOT_H
#define ROBOT_H

#include <stdint.h>

void startRotating(double goal);
void startStartMoving(double goal);

uint8_t isGoalReached(double wheelAngle);

#endif // ROBOT_H