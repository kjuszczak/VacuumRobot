#ifndef MOTOR_H
#define MOTOR_H

#include "stdint.h"
#include <pthread.h>

typedef struct motor
{
    float angularVelocity;  // [rad/s]
    float linearVelocity;   // [m/s]
    float current;          // [A]
    int pwm;
    pthread_mutex_t* motorMutex;
} motorStruct;

void calculateVelocity(motorStruct* motor, float diamater, float maxAngularVelocity);

#endif // MOTOR_H
