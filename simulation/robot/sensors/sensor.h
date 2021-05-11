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

void changeDirection(sensorStruct* sensor, int angle);

#endif
