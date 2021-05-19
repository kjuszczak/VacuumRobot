#include "creator.h"

#include <pthread.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>

#include "../io/output/output.h"
#include "../../pscommon/robotOutput.h"

sensorsOutputThreadStruct sensorsOutputDataThread;
encodersOutputThreadStruct encodersOutputDataThread;

void createThreadsForController()
{
    createSharedMemoryForSensorsOutput(&sensorsOutputDataThread);
	createSharedMemoryForEncodersOutput(&encodersOutputDataThread);
    createTimer();
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
    timerEvent.sigev_notify_function = tMainControllerPeriodicThreadFunc;
	timerEvent.sigev_notify_attributes = &aWorkerThreadAttr;

	/* Create timer */
  	if ((status = timer_create(CLOCK_REALTIME, &timerEvent, &timerVar))) {
  		fprintf(stderr, "Error creating timer : %d\n", status);
  		return 0;
  	}

  	/* Set up timer structure with time parameters */
	timerSpecStruct.it_value.tv_sec = 1;
	timerSpecStruct.it_value.tv_nsec = 0;
	timerSpecStruct.it_interval.tv_sec = 0;
	timerSpecStruct.it_interval.tv_nsec = CONTROLLER_SAMPLE_TIME * 1000000000;

	/* Change timer parameters and run */
  	timer_settime( timerVar, 0, &timerSpecStruct, NULL);
}

void *tMainControllerPeriodicThreadFunc(void *cookie)
{
    readSensors(&sensorsOutputDataThread);
	readEncoders(&encodersOutputDataThread);
}