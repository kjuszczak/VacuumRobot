#include "motor.h"
#include <stdio.h>

#include "../../pscommon/constants.h"

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
    else
    {
        motor->angularVelocity = 0;
        motor->linearVelocity = 0;
    }
}

void calculateMotorAngle(motorStruct* motor, double time)
{
    motor->angle += (int)(time * (motor->angularVelocity * DEGREE_PROP));
    motor->angle = motor->angle < 360 &&  motor->angle > -360? motor->angle : 0;
}
