#include "logThread.h"

#include "log.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

const char* fileName = "VACUUM_ROBOT.log";
const char* componentStr = NULL;

/* Mqueue variable */
mqd_t loggerMQueue;
/* Mqueue attributes structure */
struct	mq_attr outputMQueueAttr;
/* Thread variable */
pthread_t tLoggerThread;
/* Thread attributes structure */
pthread_attr_t aLoggerThreadAttr;

int initLogger(const char* cmpStr) 
{
	int status;

    componentStr = cmpStr;

	/* Initialize attributes structure */
	pthread_attr_init(&aLoggerThreadAttr);

	/* Set Message Queue size */
	outputMQueueAttr.mq_maxmsg = 100;
	outputMQueueAttr.mq_msgsize = sizeof(loggerStruct);

	/* Create Message Queue */
	if ((loggerMQueue = mq_open("/outputMQ", O_CREAT | O_RDWR, 777, &outputMQueueAttr)) == -1) {
		fprintf(stderr, "Creation of the mqueue failed.\n");
		return 0;
	}

	/* Create logger thread */
	if ((status = pthread_create( &tLoggerThread, NULL, tLoggerThreadFunc, &aLoggerThreadAttr))) {
		fprintf(stderr, "Cannot create thread.\n");
		return 0;
	}

	return 0;
}

void createFile()
{
	FILE * output_file;
    /* Create file */
	output_file = fopen(fileName, "w");
	fclose(output_file);
}

void *tLoggerThreadFunc(void *cookie)
{
	/* Output file descriptor */
	FILE * output_file;
    loggerStruct logger;
	// logBuff buffer;

	/* Scheduling policy: FIFO or RR */
	int policy = SCHED_FIFO;
	/* Structure of other thread parameters */
	struct sched_param param;

	/* Read modify and set new thread priority */
	param.sched_priority = sched_get_priority_min(policy);
	pthread_setschedparam( pthread_self(), policy, &param);

	for (;;) {
		/* Wait until something will appears in queue */
		while (mq_receive(loggerMQueue, (char *)&logger, sizeof(loggerStruct), NULL) == -1) {}

		/* Append file with new data */
		output_file = fopen(fileName, "a");
        fwrite(logger.buffer, 1, logger.num, output_file);
		fclose(output_file);
	}

	return 0;
}
