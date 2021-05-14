#ifndef SENSOR_H
#define SENSOR_H

#include <pthread.h>

#define MAX_NUMBER_OF_SENSORS 4

typedef struct sensor
{
    int xDirectory;
    int yDirectory;
    double distanceFromObject;
    pthread_mutex_t* sensorMutex;
    pthread_barrier_t* sensorBarrier;
} sensorStruct;

void changeDirection(sensorStruct** sensors, int angle);

void changeDirectionFor0(sensorStruct** sensors);
void changeDirectionFor90(sensorStruct** sensors);
void changeDirectionFor180(sensorStruct** sensors);
void changeDirectionFor270(sensorStruct** sensors);

#endif
