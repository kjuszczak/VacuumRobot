#include "sensor.h"
#include <stdio.h>

void changeDirection(sensorStruct** sensors, int angle)
{
    if (sensors == NULL)
    {
        fprintf(stderr, "Sensor is NULL\n");
        return;
    }
    int scaledAngle = angle % 360;
    switch (scaledAngle)
    {
        case 0:
            changeDirectionFor0(sensors);
            break;
        case 90:
        case -270:
            changeDirectionFor90(sensors);
            break;
        case 180:
        case -180:
            changeDirectionFor180(sensors);
            break;
        case 270:
        case -90:
            changeDirectionFor270(sensors);
            break;
        default:
            break;
    }
}

void changeDirectionFor0(sensorStruct** sensors)
{
    pthread_mutex_lock(sensors[0]->sensorMutex);
    pthread_mutex_lock(sensors[1]->sensorMutex);
    pthread_mutex_lock(sensors[2]->sensorMutex);
    pthread_mutex_lock(sensors[3]->sensorMutex);
    sensors[0]->xDirectory = 1;
    sensors[0]->yDirectory = 0;
    sensors[1]->xDirectory = -1;
    sensors[1]->yDirectory = 0;
    sensors[2]->xDirectory = 0;
    sensors[2]->yDirectory = 1;
    sensors[3]->xDirectory = 0;
    sensors[3]->yDirectory = -1;
    pthread_mutex_unlock(sensors[0]->sensorMutex);
    pthread_mutex_unlock(sensors[1]->sensorMutex);
    pthread_mutex_unlock(sensors[2]->sensorMutex);
    pthread_mutex_unlock(sensors[3]->sensorMutex);
}

void changeDirectionFor90(sensorStruct** sensors)
{
    pthread_mutex_lock(sensors[0]->sensorMutex);
    pthread_mutex_lock(sensors[1]->sensorMutex);
    pthread_mutex_lock(sensors[2]->sensorMutex);
    pthread_mutex_lock(sensors[3]->sensorMutex);
    sensors[0]->xDirectory = 0;
    sensors[0]->yDirectory = -1;
    sensors[1]->xDirectory = 0;
    sensors[1]->yDirectory = -1;
    sensors[2]->xDirectory = 1;
    sensors[2]->yDirectory = 0;
    sensors[3]->xDirectory = -1;
    sensors[3]->yDirectory = 0;
    pthread_mutex_unlock(sensors[0]->sensorMutex);
    pthread_mutex_unlock(sensors[1]->sensorMutex);
    pthread_mutex_unlock(sensors[2]->sensorMutex);
    pthread_mutex_unlock(sensors[3]->sensorMutex);
}

void changeDirectionFor180(sensorStruct** sensors)
{
    pthread_mutex_lock(sensors[0]->sensorMutex);
    pthread_mutex_lock(sensors[1]->sensorMutex);
    pthread_mutex_lock(sensors[2]->sensorMutex);
    pthread_mutex_lock(sensors[3]->sensorMutex);
    sensors[0]->xDirectory = -1;
    sensors[0]->yDirectory = 0;
    sensors[1]->xDirectory = 1;
    sensors[1]->yDirectory = 0;
    sensors[2]->xDirectory = 0;
    sensors[2]->yDirectory = -1;
    sensors[3]->xDirectory = 0;
    sensors[3]->yDirectory = 1;
    pthread_mutex_unlock(sensors[0]->sensorMutex);
    pthread_mutex_unlock(sensors[1]->sensorMutex);
    pthread_mutex_unlock(sensors[2]->sensorMutex);
    pthread_mutex_unlock(sensors[3]->sensorMutex);
}

void changeDirectionFor270(sensorStruct** sensors)
{
    pthread_mutex_lock(sensors[0]->sensorMutex);
    pthread_mutex_lock(sensors[1]->sensorMutex);
    pthread_mutex_lock(sensors[2]->sensorMutex);
    pthread_mutex_lock(sensors[3]->sensorMutex);
    sensors[0]->xDirectory = 0;
    sensors[0]->yDirectory = 1;
    sensors[1]->xDirectory = 0;
    sensors[1]->yDirectory = -1;
    sensors[2]->xDirectory = -1;
    sensors[2]->yDirectory = 0;
    sensors[3]->xDirectory = 1;
    sensors[3]->yDirectory = 0;
    pthread_mutex_unlock(sensors[0]->sensorMutex);
    pthread_mutex_unlock(sensors[1]->sensorMutex);
    pthread_mutex_unlock(sensors[2]->sensorMutex);
    pthread_mutex_unlock(sensors[3]->sensorMutex);
}
