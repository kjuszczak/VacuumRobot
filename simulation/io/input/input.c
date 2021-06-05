#include "input.h"

#include <stdio.h>
#include <stdlib.h>
#include <error.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

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
        fprintf(stderr, "[SIMULINK] Cannot create FIFO.\n" ); 
        return 0; 
    }

    // Open FIFO file
    if ((fdWheelsPwm = open ("wheels_pwm_fifo", O_RDONLY)) == -1) {
        fprintf(stderr, "[SIMULINK] Cannot open FIFO.\n" ); 
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
            fprintf(stderr, "[SIMULINK] Cannot read from FIFO.\n" ); 
            continue;
        }

        // If there is message print it
        if (bytesRead > 0) {
            fillBufferWithWheelsPwm(&buffer, robot);
        }
    }
}
