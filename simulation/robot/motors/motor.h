#ifndef MOTOR_H
#define MOTOR_H

#include "stdint.h"
#include <pthread.h>

typedef struct motor
{
    double angularVelocity;  // [rad/s]
    double linearVelocity;   // [m/s]
    double current;          // [A]
    int pwm;
    double angle;
    pthread_mutex_t* motorMutex;
} motorStruct;

void calculateVelocity(motorStruct* motor, double diamater, double maxAngularVelocity);
void calculateMotorAngle(motorStruct* motor, double time);

#endif // MOTOR_H
