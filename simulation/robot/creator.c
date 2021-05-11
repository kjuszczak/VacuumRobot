#include "creator.h"
#include "visualization/jsonCreator.h"
#include "../flat/db/creator.h"
#include "../sockets/common/socket.h"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>
#include <errno.h>
#include <mqueue.h>

tSocketData* socketVisData;

/* Mutex variables */
pthread_mutex_t robotUdpMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t sensorMutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t sensorMutex2 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t sensorMutex3 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t sensorMutex4 = PTHREAD_MUTEX_INITIALIZER;
/******************************************************/

/* SENSORS */
sensorStruct sensor1 = {1, 0, -1, &sensorMutex1};
sensorStruct sensor2 = {-1, 0, -1, &sensorMutex2};
sensorStruct sensor3 = {0, 1, -1, &sensorMutex3};
sensorStruct sensor4 = {0, -1, -1, &sensorMutex4};
sensorStruct* sensors[4] = {&sensor1, &sensor2, &sensor3, &sensor4};

/* ROBOT */
robotStruct robot = {0, 500, 150, -90, sensors, &robotUdpMutex};

void createThreadsForRobotSimulation(tSocketData *socketData, roomsStruct* rooms)
{
    socketVisData = socketData;
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

    /* Initialize threads attributes structures for FIFO scheduling */
	pthread_attr_init(&aWorkerThreadAttr);
	pthread_attr_setschedpolicy(&aWorkerThreadAttr, SCHED_FIFO);
	
	/* Initialize event to send signal SIGRTMAX */
	timerEvent.sigev_notify = SIGEV_THREAD;
    timerEvent.sigev_notify_function = tTimerThreadFunc;
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
	timerSpecStruct.it_interval.tv_nsec = 50000000;

	/* Change timer parameters and run */
  	timer_settime( timerVar, 0, &timerSpecStruct, NULL);
}

void *tTimerThreadFunc(void *cookie)
{
    pthread_t first, second;
	/* Scheduling policy: FIFO or RR */
	int policy = SCHED_FIFO;
	/* Structure of other thread parameters */
	struct sched_param param;

	/* Set new thread priority */
	param.sched_priority = sched_get_priority_max(policy);
	pthread_setschedparam( pthread_self(), policy, &param);

	static int counter = 0;
	
	pthread_create(&first, NULL, tUpdateRobotParametersThreadFunc, NULL);
	pthread_detach(first);

    pthread_create(&second, NULL, tRobotJsonUpdateThreadFunc, (void*) socketVisData);
    pthread_detach(second);
}

void* tRobotJsonUpdateThreadFunc(void *cookie)
{
    tSocketData* threadData = (tSocketData*)cookie;
    createRobotJson(&robot, threadData->jRobotObj);
    createGarbageUpdateJson(threadData->jRobotObj);
    sendRobotJson(threadData);
}

void *tUpdateRobotParametersThreadFunc(void *cookie)
{
    updateRobotParameters(&robot, 2, 2, 1);
}