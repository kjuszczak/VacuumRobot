#include "sensor.h"
#include <stdio.h>

void updateAngles(sensorStruct** sensors, int angle)
{
    if (sensors == NULL)
    {
        fprintf(stderr, "Sensor is NULL\n");
        return;
    }
    int scaledAngle = angle < 0 ? angle + 360 : angle;
    pthread_mutex_lock(sensors[0]->sensorMutex);
    pthread_mutex_lock(sensors[1]->sensorMutex);
    pthread_mutex_lock(sensors[2]->sensorMutex);
    pthread_mutex_lock(sensors[3]->sensorMutex);
    sensors[0]->angle = scaledAngle;
    sensors[1]->angle = (scaledAngle + 180) % 360;
    sensors[2]->angle = (scaledAngle + 270) % 360;
    sensors[3]->angle = (scaledAngle + 90) % 360;
    pthread_mutex_unlock(sensors[0]->sensorMutex);
    pthread_mutex_unlock(sensors[1]->sensorMutex);
    pthread_mutex_unlock(sensors[2]->sensorMutex);
    pthread_mutex_unlock(sensors[3]->sensorMutex);
}
