#include "input.h"

#include <stdio.h>
#include <stdlib.h>
#include <error.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

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

void* tReadWheelsPwmInputThreadFunc(void *cookie)
{
    int fd, bytesRead;
    wheelsPwmInputStruct buffer;
    robotStruct* robot = (robotStruct*)cookie;

    // Create FIFO
    if ((mkfifo ("wheels_pwm_fifo", 0664) == -1) && (errno != EEXIST)) {
        fprintf(stderr, "Cannot create FIFO.\n" ); 
        return 0; 
    }

    // Open FIFO file
    if ((fd = open ("wheels_pwm_fifo", O_RDONLY)) == -1) {
        fprintf(stderr, "Cannot open FIFO.\n" ); 
        return 0; 
    }

    for (;;)
    {
        // Clear data buffer
        memset (&buffer, 0, sizeof(wheelsPwmInputStruct));
        
        // Read data from FIFO
        if ((bytesRead = read (fd, &buffer, sizeof (wheelsPwmInputStruct))) == -1) {
            fprintf(stderr, "Something is wrong with FIFO.\n" ); 
            return 0; 
        }

        // If there is no data just continue
        if (bytesRead == 0)
            continue;

        // If there is message print it
        if (bytesRead > 0) {
            fillBufferWithWheelsPwm(&buffer, robot);
        }
    }
}
