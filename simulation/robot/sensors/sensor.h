#ifndef SENSOR_H
#define SENSOR_H

#include <pthread.h>

#define MAX_NUMBER_OF_SENSORS 4

typedef struct sensor
{
    double distanceFromObject;
    int angle;
    pthread_mutex_t* sensorMutex;
    pthread_barrier_t* sensorBarrier;
} sensorStruct;

void updateAngles(sensorStruct** sensors, int angle);

#endif
