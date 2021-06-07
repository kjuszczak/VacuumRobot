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
#include "../../pscommon/logger/log.h"

wheelsPwmInputStruct wheelsPwm = {0, 0};

int writeWheelsPwmToFifo(int leftWheelPwm, int rightWheelPwm)
{
    int fd;
    wheelsPwm.leftWheelPwm = leftWheelPwm;
    wheelsPwm.rightWheelPwm = rightWheelPwm;

    // Open FIFO file
    if ((fd = open("wheels_pwm_fifo", O_WRONLY)) == -1) {
        LG_WRN("Cannot open FIFO");
        return 0; 
    }

    // Write a message to FIFO
    if (write(fd, &wheelsPwm, sizeof (wheelsPwmInputStruct)) != sizeof (wheelsPwmInputStruct)) {
        LG_WRN("Cannot write to FIFO");
        return 0; 
    }

    // Close FIFO
    if (close(fd) == -1) {
        LG_WRN("Cannot close FIFO.");
        return 0; 
    }   
}
