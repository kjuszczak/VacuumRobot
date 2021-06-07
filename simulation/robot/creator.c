#include "creator.h"

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <errno.h>
#include <mqueue.h>

#include "visualization/jsonCreator.h"
#include "../flat/creator.h"
#include "../io/input/input.h"
#include "../io/output/output.h"

#include "../../pscommon/logger/log.h"

uint8_t clockCounter = 0;

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
pthread_barrier_t robotMainPeriodicFuncBarrier;

pthread_barrier_t sensorBarrier1;
pthread_barrier_t sensorBarrier2;
pthread_barrier_t sensorBarrier3;
pthread_barrier_t sensorBarrier4;

pthread_barrier_t roomIdUpdaterBarrier;

pthread_barrier_t leftEncoderBarrier;
pthread_barrier_t rightEncoderBarrier;

pthread_barrier_t sensorsOutputWriterBarrier;

pthread_barrier_t encodersOutputWriterBarrier;
pthread_barrier_t encodersUpdaterBarrier;

pthread_barrier_t wheelsPwmInputReaderBarrier;

pthread_barrier_t garbageUpdaterBarrier;
/******************************************************/

/* Mqueue variable */
mqd_t outputMQueue;
mqd_t encodersOutputMQueue;
/* Mqueue attributes structure */
struct mq_attr outputMQueueAttr;
struct mq_attr encodersOutputMQueueAttr;
/******************************************************/

/* Global variables */

/* SENSORS */
sensorStruct sensor1 = {-1, 0, &sensorMutex1, &sensorBarrier1};
sensorStruct sensor2 = {-1, 0, &sensorMutex2, &sensorBarrier2};
sensorStruct sensor3 = {-1, 0, &sensorMutex3, &sensorBarrier3};
sensorStruct sensor4 = {-1, 0, &sensorMutex4, &sensorBarrier4};
sensorStruct* sensors[MAX_NUMBER_OF_SENSORS] = {&sensor1, &sensor2, &sensor3, &sensor4};

/* WHEELS - max speed 20 [cm/s] */
motorStruct leftWheel = {0, 0, 0, 0, 0, &leftWheelMutex};
motorStruct rightWheel = {0, 0, 0, 0, 0, &rightWheelMutex};
motorStruct* wheels[2] = {&leftWheel, &rightWheel};

/* ENCODERS */
encoderStruct leftEncoder = {0, 0, 0, 0, 0, 0, &leftEncoderMutex, &leftEncoderBarrier};
encoderStruct rightEncoder = {0, 0, 0, 0, 0, 0, &rightEncoderMutex, &rightEncoderBarrier};
encoderStruct* encoders[2] = {&leftEncoder, &rightEncoder};

/* ROBOT */
robotStruct robot = {0,
					 850, 420, 90, 
					 sensors, 
					 wheels, 
					 encoders, 
					 &robotUdpMutex,
					 &robotMainPeriodicFuncBarrier,
					 &roomIdUpdaterBarrier,
					 &sensorsOutputWriterBarrier,
					 &encodersOutputWriterBarrier,
					 &encodersUpdaterBarrier,
					 &wheelsPwmInputReaderBarrier,
					 &garbageUpdaterBarrier};

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

sensorsOutputThreadStruct sensorsOutputThread;
sensorsOutputSimProcessStruct sensorsOutputSimProcessForThreads = {&sensorsOutputThread, &robot};

encodersOutputThreadStruct encodersOutputThread;
encodersOutputSimProcessStruct encodersOutputSimProcessForThreads = {&encodersOutputThread, &robot};
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
		LG_ERR("Creation of the mqueue failed.");
		return;
	}

	encodersOutputMQueueAttr.mq_maxmsg = 128;
	encodersOutputMQueueAttr.mq_msgsize = sizeof(int);

	/* Create Message Queue */
	if ((encodersOutputMQueue = mq_open("/encodersOutputQueue", O_CREAT | O_RDWR, 777, &encodersOutputMQueueAttr)) == -1) {
		LG_ERR("Creation of the mqueue failed.");
		return;
	}

	/* Initialize barrier */
	pthread_barrier_init(&robotMainPeriodicFuncBarrier, NULL, 2);
	for (size_t i = 0; i < MAX_NUMBER_OF_SENSORS; i++)
	{
		pthread_barrier_init(sensors[i]->sensorBarrier, NULL, 2);
	}
	pthread_barrier_init(&roomIdUpdaterBarrier, NULL, 2);
	for (size_t i = 0; i < 2; i++)
	{
		pthread_barrier_init(encoders[i]->encoderBarrier, NULL, 2);
	}
	pthread_barrier_init(&sensorsOutputWriterBarrier, NULL, 5); // 4x sensors

	pthread_barrier_init(&encodersOutputWriterBarrier, NULL, 3); // 2x sensors
	pthread_barrier_init(&encodersUpdaterBarrier, NULL, 3); // 2x sensors

	pthread_barrier_init(&wheelsPwmInputReaderBarrier, NULL, 2);
	pthread_barrier_init(&garbageUpdaterBarrier, NULL, 2);
}

void clean()
{
	closeSharedMemoryForSensorsOutput(&sensorsOutputThread);
	closeSharedMemoryForEncodersOutput(&encodersOutputThread);
}

void createThreadsForRobotSimulation(tSocketData *socketData, roomsStruct* rooms)
{
	init(socketData, rooms);

	createRobotThreads();
	createJsonSenderThread();
	createClockHandler();
}

int createClockHandler()
{
    /* Prepare sigaction struct for handler */
    struct sigaction action;
    action.sa_handler = tClockHandlerThreadFunc;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;

    /* Register signal handler for clock manager */
    if (sigaction(SIGRTMIN + 1, &action, NULL) < 0) 
	{
		LG_ERR("Cannot register SIGRTMIN handler.");
        return -1;
    }

	return 0;
}

int createRobotThreads()
{
	createMainPeriodicRobotThread();
	createSensorsThreads();
	createEncodersThreads();
	createRoomIdUpdaterThread();
	createGarbagesUpdaterThread();
	createRobotInputReaderThread();
	createSensorsOutputWriterThread();
	createEncodersOutputWriterThread();
}

int createMainPeriodicRobotThread()
{
	pthread_t mainPeriodicRobotThread;
	/* Scheduling policy: FIFO or RR */
	int policy = SCHED_FIFO;
	/* Structure of other thread parameters */
	struct sched_param param;

	/* Set new thread priority */
	param.sched_priority = sched_get_priority_max(policy);
	pthread_setschedparam( pthread_self(), policy, &param);

	pthread_create(&mainPeriodicRobotThread, NULL, tMainRobotPeriodicThreadFunc, (void *) &robotDataForThreads);
	pthread_detach(mainPeriodicRobotThread);	
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
	param.sched_priority = sched_get_priority_min(policy);
	pthread_setschedparam( pthread_self(), policy, &param);

	pthread_create(&roomIdUpdaterThread, NULL, tUpdateRoomIdThreadFunc, (void *) &robotDataForThreads);
	pthread_detach(roomIdUpdaterThread);
}

int createGarbagesUpdaterThread()
{
	pthread_t garbagesUpdaterThread;
	/* Scheduling policy: FIFO or RR */
	int policy = SCHED_FIFO;
	/* Structure of other thread parameters */
	struct sched_param param;

	/* Set new thread priority */
	param.sched_priority = sched_get_priority_min(policy);
	pthread_setschedparam( pthread_self(), policy, &param);

	pthread_create(&garbagesUpdaterThread, NULL, tUpdateGarbagesThreadFunc, (void *) &robotDataForThreads);
	pthread_detach(garbagesUpdaterThread);
}

int createRobotInputReaderThread()
{
	pthread_t inputWheelsPwmReader;
	/* Scheduling policy: FIFO or RR */
	int policy = SCHED_FIFO;
	/* Structure of other thread parameters */
	struct sched_param param;

	/* Set new thread priority */
	param.sched_priority = sched_get_priority_max(policy);
	pthread_setschedparam( pthread_self(), policy, &param);

	pthread_create(&inputWheelsPwmReader, NULL, tReadWheelsPwmInputThreadFunc, (void *) &robot);
	pthread_detach(inputWheelsPwmReader);
}

int createSensorsOutputWriterThread()
{
	pthread_t sensorsOutputWriter;
	/* Scheduling policy: FIFO or RR */
	int policy = SCHED_FIFO;
	/* Structure of other thread parameters */
	struct sched_param param;

	/* Set new thread priority */
	param.sched_priority = sched_get_priority_max(policy);
	pthread_setschedparam( pthread_self(), policy, &param);

	pthread_create(&sensorsOutputWriter, NULL, tWriteSensorsOutputThreadFunc, (void *) &sensorsOutputSimProcessForThreads);
	pthread_detach(sensorsOutputWriter);
}

int createEncodersOutputWriterThread()
{
	pthread_t encodersOutputWriter;
	/* Scheduling policy: FIFO or RR */
	int policy = SCHED_FIFO;
	/* Structure of other thread parameters */
	struct sched_param param;

	/* Set new thread priority */
	param.sched_priority = sched_get_priority_max(policy);
	pthread_setschedparam( pthread_self(), policy, &param);

	pthread_create(&encodersOutputWriter, NULL, tWriteEncodersOutputThreadFunc, (void *) &encodersOutputSimProcessForThreads);
	pthread_detach(encodersOutputWriter);
}

int createJsonSenderThread()
{
    pthread_t updateRobotJson;
	/* Scheduling policy: FIFO or RR */
	int policy = SCHED_FIFO;
	/* Structure of other thread parameters */
	struct sched_param param;

	/* Set new thread priority */
	param.sched_priority = sched_get_priority_min(policy);
	pthread_setschedparam( pthread_self(), policy, &param);

    pthread_create(&updateRobotJson, NULL, tRobotJsonUpdateThreadFunc, NULL);
    pthread_detach(updateRobotJson);
}

/* THREADS FUNCTION */
void *tClockHandlerThreadFunc(void *cookie)
{
    /* Scheduling policy: FIFO or RR */
	int policy = SCHED_FIFO;
	/* Structure of other thread parameters */
	struct sched_param param;

	/* Read modify and set new thread priority */
	param.sched_priority = sched_get_priority_max(policy);
	pthread_setschedparam(pthread_self(), policy, &param);

	if (clockCounter < SIMULATION_MAX_COUNTER)
	{
		clockCounter++;
		return;
	}
	pthread_barrier_wait(&wheelsPwmInputReaderBarrier);
	pthread_barrier_wait(&robotMainPeriodicFuncBarrier);
	clockCounter = 0;
}

void* tRobotJsonUpdateThreadFunc(void *cookie)
{
	int outputResult = 1;

	for (;;)
	{
		/* Wait until something will appears in queue */
		while (mq_receive(outputMQueue, (char *)&outputResult, sizeof(int), NULL) == -1) {} // protect for signal interruption

		createRobotJson(&robot, socketVisData->jRobotObj);
		createGarbageUpdateJson(socketVisData->jRobotObj);
		sendRobotJson(socketVisData);
	}
}
