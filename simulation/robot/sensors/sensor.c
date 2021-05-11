#include "sensor.h"
#include <stdio.h>

void changeDirection(sensorStruct* sensor, int angle)
{
    if (sensor == NULL)
    {
        fprintf(stderr, "Sensor is NULL\n");
        return;
    }
    int scaledAngle = angle % 360;
    switch (scaledAngle)
    {
        case 0:
            sensor->xDirectory = 1;
            sensor->xDirectory = 0;
            break;
        case 90:
        case -270:
            sensor->xDirectory = 0;
            sensor->xDirectory = -1;
            break;
        case 180:
        case -180:
            sensor->xDirectory = -1;
            sensor->xDirectory = 0;
            break;
        case 270:
        case -90:
            sensor->xDirectory = 0;
            sensor->xDirectory = 1;
            break;
        default:
            break;
    }
}
