#include "input.h"

#include <stdio.h>
#include <stdlib.h>
#include <error.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int writeWheelsPwmToFifo(wheelsPwmInputStruct* wheelsPwm)
{
    int fd;

    // Open FIFO file
    if ((fd = open("wheels_pwm_fifo", O_WRONLY)) == -1) {
        fprintf(stderr, "Cannot open FIFO.\n" ); 
        return 0; 
    }

    // Write a message to FIFO
    if (write(fd, wheelsPwm, sizeof (wheelsPwmInputStruct)) != sizeof (wheelsPwmInputStruct)) {
        fprintf(stderr, "Cannot write to FIFO.\n" ); 
        return 0; 
    }

    // Close FIFO
    if (close(fd) == -1) {
        fprintf(stderr, "Cannot close FIFO.\n" ); 
        return 0; 
    }   
}

void* tWriteWheelsPwmInputThreadFunc(void *cookie)
{
    int fd, bytesRead;
    wheelsPwmInputStruct* pwmsToWrite = (wheelsPwmInputStruct*)cookie;

    for (;;)
    {
        // Open FIFO file
        if ((fd = open("wheels_pwm_fifo", O_WRONLY)) == -1) {
            fprintf(stderr, "Cannot open FIFO.\n" ); 
            return 0; 
        }

        // Write a message to FIFO
        if (write(fd, pwmsToWrite, sizeof (wheelsPwmInputStruct)) != sizeof (wheelsPwmInputStruct)) {
            fprintf(stderr, "Cannot write to FIFO.\n" ); 
            return 0; 
        }

        // Close FIFO
        if (close(fd) == -1) {
            fprintf(stderr, "Cannot close FIFO.\n" ); 
            return 0; 
        }
    }
}