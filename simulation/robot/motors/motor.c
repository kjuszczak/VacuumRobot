#include "motor.h"
#include <stdio.h>

void calculateVelocity(motorStruct* motor, float diamater, float maxAngularVelocity)
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