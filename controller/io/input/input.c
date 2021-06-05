#include "input.h"

#include <stdio.h>
#include <stdlib.h>
#include <error.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include "../creator/controller.h"

wheelsPwmInputStruct wheelsPwm = {0, 0};

int writeWheelsPwmToFifo(int leftWheelPwm, int rightWheelPwm)
{
    int fd;
    wheelsPwm.leftWheelPwm = leftWheelPwm;
    wheelsPwm.rightWheelPwm = rightWheelPwm;

    // Open FIFO file
    if ((fd = open("wheels_pwm_fifo", O_WRONLY)) == -1) {
        fprintf(stderr, "[CONTROLLER] Cannot open FIFO.\n" ); 
        return 0; 
    }

    // Write a message to FIFO
    if (write(fd, &wheelsPwm, sizeof (wheelsPwmInputStruct)) != sizeof (wheelsPwmInputStruct)) {
        fprintf(stderr, "[CONTROLLER] Cannot write to FIFO.\n" ); 
        return 0; 
    }

    // Close FIFO
    if (close(fd) == -1) {
        fprintf(stderr, "[CONTROLLER] Cannot close FIFO.\n" ); 
        return 0; 
    }   
}
