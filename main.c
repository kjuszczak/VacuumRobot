#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>

#include "pscommon/constants.h"
#include "pscommon/logger/logThread.h"
#include "pscommon/logger/log.h"

const char* component = "MAIN";

pid_t pid_simulation, pid_controller;

static void exit_handler(int);
void clean();

int createTimer();
void* tClockManagerThreadFunc(void *cookie);

int main(int argc, char *argv[])
{
	char * robot_simulation[3] = {"./robot_simulation", NULL};
    char * robot_controller[3] = {"./robot_controller", NULL};

    /* Prepare sigaction struct for handler */
    struct sigaction action;
    action.sa_handler = exit_handler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;

    /* Register signal handler for SIGINT */
    if (sigaction(SIGINT, &action, NULL) < 0)
    {
        LG_ERR("Cannot register SIGINT handler.");
        return -1;
    }

    createFile();
    initLogger(component);
    LG_INF("Main starts");

	pid_simulation = fork();

	if (pid_simulation == 0)
    {
		execvp(robot_simulation[0], robot_simulation);
	} 
    else
    {
        pid_controller = fork();
        if (pid_controller == 0)
        {
            execvp(robot_controller[0], robot_controller);
        }
	}

    if (createTimer())
    {
        return EXIT_FAILURE;
    }

	// /* Wait for processes*/
	while(getc(stdin)=='q') {}

    clean();
    return EXIT_SUCCESS;
}

int createTimer()
{
    int	status;

    /* Threads attributes variables */
    pthread_attr_t aWorkerThreadAttr;
    /* Structure with time values */
    struct itimerspec timerSpecStruct;
    /* Timer variable */
    timer_t	timerVar;
    /* Signal variable */
    struct sigevent timerEvent;
	/* Thread function data */
	union sigval threadData;

    /* Initialize threads attributes structures for FIFO scheduling */
	pthread_attr_init(&aWorkerThreadAttr);
	pthread_attr_setschedpolicy(&aWorkerThreadAttr, SCHED_FIFO);
	
	/* Initialize event to send signal SIGRTMAX */
	timerEvent.sigev_notify = SIGEV_THREAD;
    timerEvent.sigev_notify_function = tClockManagerThreadFunc;
	timerEvent.sigev_notify_attributes = &aWorkerThreadAttr;

	/* Create timer */
  	if ((status = timer_create(CLOCK_REALTIME, &timerEvent, &timerVar))) {
        LG_ERR("Error creating timer : %d", status);
  		return EXIT_FAILURE;
  	}

  	/* Set up timer structure with time parameters */
	timerSpecStruct.it_value.tv_sec = 1;
	timerSpecStruct.it_value.tv_nsec = 0;
	timerSpecStruct.it_interval.tv_sec = 0;
	timerSpecStruct.it_interval.tv_nsec = CLOCK_SAMPLE_TIME * 1000000000;

	/* Change timer parameters and run */
  	timer_settime( timerVar, 0, &timerSpecStruct, NULL);

    return EXIT_SUCCESS;
}

void* tClockManagerThreadFunc(void *cookie)
{
    kill(pid_controller, SIGRTMIN + 2);
    kill(pid_simulation, SIGRTMIN + 1);
}

/* Exit signal handler function */
void exit_handler(int sig)
{
    clean();
    exit(EXIT_SUCCESS);
}

void clean()
{
    LG_INF("Main exits");

    kill(pid_simulation, SIGRTMIN);
    waitpid(pid_simulation, NULL, 0);
    kill(pid_controller, SIGRTMIN);
    waitpid(pid_controller, NULL, 0);

    system("sudo rm -rf /dev/shm/*");
}