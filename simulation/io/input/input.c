#include "input.h"

#include <stdio.h>
#include <stdlib.h>
#include <error.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include "../../pscommon/logger/log.h"

int fdWheelsPwm;

void fillBufferWithWheelsPwm(wheelsPwmInputStruct* buffer, robotStruct* robot)
{
    // Left wheel
    pthread_mutex_lock(robot->wheels[0]->motorMutex);
    robot->wheels[0]->pwm = buffer->leftWheelPwm;
    pthread_mutex_unlock(robot->wheels[0]->motorMutex);

    // Right wheel
    pthread_mutex_lock(robot->wheels[1]->motorMutex);
    robot->wheels[1]->pwm = buffer->rightWheelPwm;
    pthread_mutex_unlock(robot->wheels[1]->motorMutex);
}

int createWheelsPwmInputFifo()
{
    // Create FIFO
    if ((mkfifo ("wheels_pwm_fifo", 0664) == -1) && (errno != EEXIST)) {
        LG_ERR("Cannot create FIFO.");
        return 0; 
    }

    // Open FIFO file
    if ((fdWheelsPwm = open ("wheels_pwm_fifo", O_RDONLY)) == -1) {
        LG_ERR("Cannot open FIFO.");
        return 0; 
    }
}

void* tReadWheelsPwmInputThreadFunc(void *cookie)
{
    int fd, bytesRead;
    wheelsPwmInputStruct buffer;
    robotStruct* robot = (robotStruct*)cookie;

    createWheelsPwmInputFifo();

    for (;;)
    {
        pthread_barrier_wait(robot->wheelsPwmInputReaderBarrier);

        // Clear data buffer
        memset (&buffer, 0, sizeof(wheelsPwmInputStruct));

        // Read data from FIFO
        if ((bytesRead = read (fdWheelsPwm, &buffer, sizeof (wheelsPwmInputStruct))) == -1) {
            LG_WRN("Cannot read from FIFO.");
            continue;
        }

        // If there is message print it
        if (bytesRead > 0) {
            fillBufferWithWheelsPwm(&buffer, robot);
        }
    }
}
