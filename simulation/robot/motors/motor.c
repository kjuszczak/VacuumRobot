#include "motor.h"
#include <stdio.h>

#define PI 3.14159265
#define DEGREE_PROP 180 / PI

void calculateVelocity(motorStruct* motor, double diamater, double maxAngularVelocity)
{
    if (motor == NULL)
    {
        fprintf(stderr, "Motor is NULL\n");
        return;
    }

    pthread_mutex_lock(motor->motorMutex);
    int pwm = motor->pwm;
    pthread_mutex_unlock(motor->motorMutex);

    if (pwm == 255)
    {
        motor->angularVelocity = maxAngularVelocity;
        motor->linearVelocity = maxAngularVelocity * (diamater / 2);
    }
    else if (pwm == -255)
    {
        motor->angularVelocity = -maxAngularVelocity;
        motor->linearVelocity = -maxAngularVelocity * (diamater / 2);
    }
}

void calculateMotorAngle(motorStruct* motor, double time)
{
    motor->angle += (motor->angularVelocity * DEGREE_PROP) * time;
    motor->angle = motor->angle % 360;
}
