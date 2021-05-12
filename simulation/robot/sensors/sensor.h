#ifndef SENSOR_H
#define SENSOR_H

#include <pthread.h>

typedef struct sensor
{
    int xDirectory;
    int yDirectory;
    float distanceFromObject;
    pthread_mutex_t* sensorMutex;
} sensorStruct;

void changeDirection(sensorStruct** sensors, int angle);

void changeDirectionFor0(sensorStruct** sensors);
void changeDirectionFor90(sensorStruct** sensors);
void changeDirectionFor180(sensorStruct** sensors);
void changeDirectionFor270(sensorStruct** sensors);

#endif
