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

#define MAX_NUMBER_OF_SENSORS 4

/* Mutex variables */
pthread_mutex_t robotUdpMutex = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t sensorMutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t sensorMutex2 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t sensorMutex3 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t sensorMutex4 = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t leftWheelMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t rightWheelMutex = PTHREAD_MUTEX_INITIALIZER;
/******************************************************/

/* Global variables */

/* SENSORS */
sensorStruct sensor1 = {1, 0, -1, &sensorMutex1};
sensorStruct sensor2 = {-1, 0, -1, &sensorMutex2};
sensorStruct sensor3 = {0, 1, -1, &sensorMutex3};
sensorStruct sensor4 = {0, -1, -1, &sensorMutex4};
sensorStruct* sensors[4] = {&sensor1, &sensor2, &sensor3, &sensor4};

/* WHEELS - max speed 20 [cm/s] */
motorStruct leftWheel = {0, 0, 0, -255, &leftWheelMutex};
motorStruct rightWheel = {0, 0, 0, 255, &rightWheelMutex};
motorStruct* wheels[2] = {&leftWheel, &rightWheel};

/* ROBOT */
robotStruct robot = {0, 120, 130, 0, sensors, wheels, &robotUdpMutex};

/* THREADS VARIABLES */
tSocketData* socketVisData = NULL;
robotThreadStruct robotDataForThreads = {&robot, NULL};
robotThreadStruct sensorsDataForThreads[MAX_NUMBER_OF_SENSORS] = {
	{&robotDataForThreads, 0},
	{&robotDataForThreads, 1},
	{&robotDataForThreads, 2},
	{&robotDataForThreads, 3}
};
/******************************************************/

void init(tSocketData *socketData, roomsStruct* rooms)
{
	socketVisData = socketData;
	robotDataForThreads.rooms = rooms;
}

void createThreadsForRobotSimulation(tSocketData *socketData, roomsStruct* rooms)
{
	init(socketData, rooms);
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
	timerSpecStruct.it_interval.tv_nsec = SIMULATION_SAMPLE_TIME * 1000000000;

	/* Change timer parameters and run */
  	timer_settime( timerVar, 0, &timerSpecStruct, NULL);
}

void *tTimerThreadFunc(void *cookie)
{
	static int counter = 0;
	
	createRobotThreads();
	createJsonSenderThread();
}

int createRobotThreads()
{
	createSensorsThreads();
	createWheelsThread();
	createRoomIdUpdaterThread();
}

int createSensorsThreads()
{
	pthread_t sensorsThreads[MAX_NUMBER_OF_SENSORS], sensorsMeasurementSenderThreads[MAX_NUMBER_OF_SENSORS];
	/* Scheduling policy: FIFO or RR */
	int policy = SCHED_FIFO;
	/* Structure of other thread parameters */
	struct sched_param param;

	/* Set new thread priority */
	param.sched_priority = sched_get_priority_max(policy);
	pthread_setschedparam( pthread_self(), policy, &param);

	for (size_t i = 0; i < MAX_NUMBER_OF_SENSORS; i++)
	{
		pthread_create(&sensorsThreads[i], NULL, tUpdateSensorsThreadFunc, (void *) &sensorsDataForThreads[i]);
		pthread_detach(sensorsThreads[i]);

		pthread_create(&sensorsMeasurementSenderThreads[i], NULL, tSendSensorMeasurementThreadFunc, (void *) &sensorsDataForThreads[i]);
		pthread_detach(sensorsMeasurementSenderThreads[i]);
	}
}

int createWheelsThread()
{
	pthread_t wheelsThread;
	/* Scheduling policy: FIFO or RR */
	int policy = SCHED_FIFO;
	/* Structure of other thread parameters */
	struct sched_param param;

	/* Set new thread priority */
	param.sched_priority = sched_get_priority_max(policy);
	pthread_setschedparam( pthread_self(), policy, &param);

	pthread_create(&wheelsThread, NULL, tSimulateWheelsThreadFunc, (void *) &robotDataForThreads);
	pthread_detach(wheelsThread);
}

int createRoomIdUpdaterThread()
{
	pthread_t roomIdUpdaterThread;
	/* Scheduling policy: FIFO or RR */
	int policy = SCHED_FIFO;
	/* Structure of other thread parameters */
	struct sched_param param;

	/* Set new thread priority */
	param.sched_priority = sched_get_priority_max(policy);
	pthread_setschedparam( pthread_self(), policy, &param);

	pthread_create(&roomIdUpdaterThread, NULL, tUpdateRoomIdThreadFunc, (void *) &robotDataForThreads);
	pthread_detach(roomIdUpdaterThread);
}

int createJsonSenderThread()
{
    pthread_t updateRobotJson;
	/* Scheduling policy: FIFO or RR */
	int policy = SCHED_FIFO;
	/* Structure of other thread parameters */
	struct sched_param param;

	/* Set new thread priority */
	param.sched_priority = sched_get_priority_max(policy);
	pthread_setschedparam( pthread_self(), policy, &param);

    pthread_create(&updateRobotJson, NULL, tRobotJsonUpdateThreadFunc, NULL);
    pthread_detach(updateRobotJson);
}

/* THREADS FUNCTION */
void* tRobotJsonUpdateThreadFunc(void *cookie)
{
    createRobotJson(&robot, socketVisData->jRobotObj);
    createGarbageUpdateJson(socketVisData->jRobotObj);
    sendRobotJson(socketVisData);
}
