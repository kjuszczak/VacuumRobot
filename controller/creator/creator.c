#include "creator.h"

#include <pthread.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>

#include "controller.h"
#include "../io/input/input.h"
#include "../io/output/output.h"

uint8_t clockCounter = 0;

/* Mutex variables */
pthread_mutex_t leftWheelEncoderMutex;
pthread_mutex_t rightWheelEncoderMutex;
/******************************************************/

/* Threads barriers */
pthread_barrier_t mainPeriodicFuncBarrier;

pthread_barrier_t leftWheelEncoderBarrier;
pthread_barrier_t rightWheelEncoderBarrier;
pthread_barrier_t encodersOutputReaderBarrier;
pthread_barrier_t encodersCalculationBarrier;
pthread_barrier_t sensorsOutputReaderBarrier;
/******************************************************/

/* Global variables */
encoderStruct leftWheel = {0, 0, 0, 0, 0, &leftWheelEncoderMutex, &leftWheelEncoderBarrier, &encodersCalculationBarrier};
encoderStruct rightWheel = {0, 0, 0, 0, 0, &rightWheelEncoderMutex, &rightWheelEncoderBarrier, &encodersCalculationBarrier};

controllerStruct controller = {&leftWheel,
                               &rightWheel,
                               &mainPeriodicFuncBarrier,
                               &encodersOutputReaderBarrier,
							   &encodersCalculationBarrier,
							   &sensorsOutputReaderBarrier};
/* THREADS VARIABLES */
encodersOutputThreadStruct encoderOutputThreadData;
encodersOutputControllerProcessStruct encoderOutputThread = {&encoderOutputThreadData, &controller};

sensorsOutputThreadStruct sensorsOutputThreadData;
sensorsOutputControllerProcessStruct sensorsOutputThread = {&sensorsOutputThreadData, &controller};
/******************************************************/

void init()
{
    /* Initialize barrier */
	pthread_barrier_init(&mainPeriodicFuncBarrier, NULL, 2);

    pthread_barrier_init(&leftWheelEncoderBarrier, NULL, 2);
    pthread_barrier_init(&rightWheelEncoderBarrier, NULL, 2);
    pthread_barrier_init(&encodersOutputReaderBarrier, NULL, 2);
	pthread_barrier_init(&encodersCalculationBarrier, NULL, 3);
	pthread_barrier_init(&sensorsOutputReaderBarrier, NULL, 2);
}

void clean()
{
	closeSharedMemoryForEncodersOutput(&encoderOutputThreadData);
	closeSharedMemoryForSensorsOutput(&sensorsOutputThreadData);
}

void createThreadsForController()
{
    init();

    createMainPeriodicThread();
    createThreadForEncodersAngleUpdater();
    createThreadForEncodersReader();
	createThreadForSensorsReader();
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
    if (sigaction(SIGRTMIN + 2, &action, NULL) < 0) 
	{
        fprintf(stderr, "Cannot register SIGRTMIN handler.\n");
        return -1;
    }
	return 0;
}

void createMainPeriodicThread()
{
	pthread_t mainFuncThread;
	/* Scheduling policy: FIFO or RR */
	int policy = SCHED_FIFO;
	/* Structure of other thread parameters */
	struct sched_param param;

	/* Set new thread priority */
	param.sched_priority = sched_get_priority_max(policy);
	pthread_setschedparam( pthread_self(), policy, &param);

	pthread_create(&mainFuncThread, NULL, tMainControllerPeriodicThreadFunc, (void *) &controller);
	pthread_detach(mainFuncThread);	
}

void createThreadForEncodersAngleUpdater()
{
	pthread_t encodersAngleThread;
	/* Scheduling policy: FIFO or RR */
	int policy = SCHED_FIFO;
	/* Structure of other thread parameters */
	struct sched_param param;

	/* Set new thread priority */
	param.sched_priority = sched_get_priority_max(policy);
	pthread_setschedparam( pthread_self(), policy, &param);

	pthread_create(&encodersAngleThread, NULL, tEncoderAngleUpdaterThreadFunc, (void *) &leftWheel);
	pthread_detach(encodersAngleThread);
    pthread_create(&encodersAngleThread, NULL, tEncoderAngleUpdaterThreadFunc, (void *) &rightWheel);
	pthread_detach(encodersAngleThread);
}

void createThreadForEncodersReader()
{
	pthread_t encodersOutputReader;
	/* Scheduling policy: FIFO or RR */
	int policy = SCHED_FIFO;
	/* Structure of other thread parameters */
	struct sched_param param;

	/* Set new thread priority */
	param.sched_priority = sched_get_priority_max(policy);
	pthread_setschedparam( pthread_self(), policy, &param);

	pthread_create(&encodersOutputReader, NULL, tReadEncodersOutputThreadFunc, (void *) &encoderOutputThread);
	pthread_detach(encodersOutputReader);	
}

void createThreadForSensorsReader()
{
	pthread_t sensorsOutputReader;
	/* Scheduling policy: FIFO or RR */
	int policy = SCHED_FIFO;
	/* Structure of other thread parameters */
	struct sched_param param;

	/* Set new thread priority */
	param.sched_priority = sched_get_priority_max(policy);
	pthread_setschedparam( pthread_self(), policy, &param);

	pthread_create(&sensorsOutputReader, NULL, tReadSensorsOutputThreadFunc, (void *) &sensorsOutputThread);
	pthread_detach(sensorsOutputReader);		
}

void *tClockHandlerThreadFunc(void *cookie)
{
    /* Scheduling policy: FIFO or RR */
	int policy = SCHED_FIFO;
	/* Structure of other thread parameters */
	struct sched_param param;

	/* Read modify and set new thread priority */
	param.sched_priority = sched_get_priority_max(policy);
	pthread_setschedparam(pthread_self(), policy, &param);

	// if (clockCounter < SIMULATION_MAX_COUNTER)
	// {
	// 	clockCounter++;
	// 	return;
	// }

	pthread_barrier_wait(&mainPeriodicFuncBarrier);
	clockCounter = 0;
}
