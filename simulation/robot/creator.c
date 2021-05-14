#include "creator.h"

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <errno.h>
#include <mqueue.h>

#include "visualization/jsonCreator.h"
#include "../flat/creator.h"

/* Mutex variables */
pthread_mutex_t robotUdpMutex = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t sensorMutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t sensorMutex2 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t sensorMutex3 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t sensorMutex4 = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t leftWheelMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t rightWheelMutex = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t leftEncoderMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t rightEncoderMutex = PTHREAD_MUTEX_INITIALIZER;
/******************************************************/

/* Threads barriers */
pthread_barrier_t sensorBarrier1;
pthread_barrier_t sensorBarrier2;
pthread_barrier_t sensorBarrier3;
pthread_barrier_t sensorBarrier4;

pthread_barrier_t roomIdUpdaterBarrier;

pthread_barrier_t leftEncoderBarrier;
pthread_barrier_t rightEncoderBarrier;
/******************************************************/

/* Mqueue variable */
mqd_t outputMQueue;
/* Mqueue attributes structure */
struct	mq_attr outputMQueueAttr;
/******************************************************/

/* Global variables */

/* SENSORS */
sensorStruct sensor1 = {1, 0, -1, &sensorMutex1, &sensorBarrier1};
sensorStruct sensor2 = {-1, 0, -1, &sensorMutex2, &sensorBarrier2};
sensorStruct sensor3 = {0, 1, -1, &sensorMutex3, &sensorBarrier3};
sensorStruct sensor4 = {0, -1, -1, &sensorMutex4, &sensorBarrier4};
sensorStruct* sensors[MAX_NUMBER_OF_SENSORS] = {&sensor1, &sensor2, &sensor3, &sensor4};

/* WHEELS - max speed 20 [cm/s] */
motorStruct leftWheel = {0, 0, 0, 255, 0, &leftWheelMutex};
motorStruct rightWheel = {0, 0, 0, -255, 0, &rightWheelMutex};
motorStruct* wheels[2] = {&leftWheel, &rightWheel};

/* ENCODERS */
encoderStruct leftEncoder = {0, 0, 0, 0, 0, &leftEncoderMutex, &leftEncoderBarrier};
encoderStruct rightEncoder = {0, 0, 0, 0, 0, &rightEncoderMutex, &rightEncoderBarrier};
encoderStruct* encoders[2] = {&leftEncoder, &rightEncoder};

/* ROBOT */
robotStruct robot = {0, 120, 130, 0, sensors, wheels, encoders, &robotUdpMutex, &roomIdUpdaterBarrier};

/* THREADS VARIABLES */
tSocketData* socketVisData = NULL;
robotThreadStruct robotDataForThreads = {&robot, NULL, &outputMQueue};
sensorThreadStruct sensorsDataForThreads[MAX_NUMBER_OF_SENSORS] = {
	{&robotDataForThreads, 0},
	{&robotDataForThreads, 1},
	{&robotDataForThreads, 2},
	{&robotDataForThreads, 3}
};
encoderThreadStruct encodersDataForThreads[2] = {
	{&robotDataForThreads, 0},
	{&robotDataForThreads, 1}
};
/******************************************************/

void init(tSocketData *socketData, roomsStruct* rooms)
{
	socketVisData = socketData;
	robotDataForThreads.rooms = rooms;

	/* Set Message Queue size */
	outputMQueueAttr.mq_maxmsg = 128;
	outputMQueueAttr.mq_msgsize = sizeof(int);

	/* Create Message Queue */
	if ((outputMQueue = mq_open("/robotOutput", O_CREAT | O_RDWR, 777, &outputMQueueAttr)) == -1) {
		fprintf(stderr, "Creation of the mqueue failed.\n");
		return;
	}

	/* Initialize barrier */
	for (size_t i = 0; i < MAX_NUMBER_OF_SENSORS; i++)
	{
		pthread_barrier_init(sensors[i]->sensorBarrier, NULL, 2);
	}
	pthread_barrier_init(&roomIdUpdaterBarrier, NULL, 2);
	for (size_t i = 0; i < 2; i++)
	{
		pthread_barrier_init(encoders[i]->encoderBarrier, NULL, 2);
	}
}

void createThreadsForRobotSimulation(tSocketData *socketData, roomsStruct* rooms)
{
	init(socketData, rooms);

    createTimer();
	createRobotThreads();
	createJsonSenderThread();
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
    timerEvent.sigev_notify_function = tMainRobotPeriodicThreadFunc;
	timerEvent.sigev_value.sival_ptr = (void*) &robotDataForThreads;
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

int createRobotThreads()
{
	createSensorsThreads();
	createEncodersThreads();
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
	}
}

int createEncodersThreads()
{
	pthread_t encodersThreads[2];
	/* Scheduling policy: FIFO or RR */
	int policy = SCHED_FIFO;
	/* Structure of other thread parameters */
	struct sched_param param;

	/* Set new thread priority */
	param.sched_priority = sched_get_priority_max(policy);
	pthread_setschedparam( pthread_self(), policy, &param);

	for (size_t i = 0; i < 2; i++)
	{
		pthread_create(&encodersThreads[i], NULL, tUpdateEncoderThreadFunc, (void *) &encodersDataForThreads[i]);
		pthread_detach(encodersThreads[i]);
	}
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
	int outputResult = 1;

	for (;;)
	{
		/* Wait until something will appears in queue */
		mq_receive(outputMQueue, (char *)&outputResult, sizeof(int), NULL);

		createRobotJson(&robot, socketVisData->jRobotObj);
		createGarbageUpdateJson(socketVisData->jRobotObj);
		sendRobotJson(socketVisData);
	}
}
