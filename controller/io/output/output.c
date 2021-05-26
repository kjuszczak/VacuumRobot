#include "output.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <pthread.h>

#include "../../pscommon/constants.h"

uint8_t lastLeftSigA = 0;
uint8_t lastRightSigA = 0;
int leftCounter = 0;
int rightCounter = 0;
int leftAngle = 0;
int rightAngle = 0;


int createSharedMemoryForSensorsOutput(sensorsOutputThreadStruct* sensorsOutputDataThread)
{
    int fdShm;
    
    //  Create mutex semaphore
    if ((sensorsOutputDataThread->mutexSem = sem_open("/sensors-sem-mutex", O_CREAT, 0660, 0)) == SEM_FAILED) {
        fprintf(stderr, "Cannot create semaphore.\n");
		return 0;
    }

    // Create spool semaphore
    if ((sensorsOutputDataThread->spoolSem = sem_open("/sensors-sem-spool", O_CREAT, 0660, 0)) == SEM_FAILED) {
        fprintf(stderr, "Cannot create semaphore.\n");
		return 0;
    }
    
    // Get shared memory 
    if ((fdShm = shm_open("/sensors-shared-mem", O_RDWR | O_CREAT | O_TRUNC, 0660)) == -1) {
        fprintf(stderr, "Cannot create shared memory.\n");
		return 0;
    }

    //  Truncate share memory
    if (ftruncate(fdShm, sizeof (sensorsOutputStruct)) == -1) {
        fprintf(stderr, "Cannot truncate shared memory.\n");
		return 0;
    }

    // Map shared memory
    if ((sensorsOutputDataThread->sensorsOutputData = mmap(NULL, sizeof (sensorsOutputStruct), PROT_READ | PROT_WRITE, MAP_SHARED, fdShm, 0)) == MAP_FAILED) {
        fprintf(stderr, "Cannot map shared memory.\n");
		return 0;
    }

    sem_post(sensorsOutputDataThread->mutexSem);
}

int createSharedMemoryForEncodersOutput(encodersOutputThreadStruct* encodersOutputDataThread)
{
    int fdShm;
    
    //  Create mutex semaphore
    if ((encodersOutputDataThread->mutexSem = sem_open("/encoders-sem-mutex", O_CREAT, 0660, 0)) == SEM_FAILED) {
        fprintf(stderr, "Cannot create semaphore.\n");
		return 0;
    }

    // Create spool semaphore
    if ((encodersOutputDataThread->spoolSem = sem_open("/encoders-sem-spool", O_CREAT, 0660, 0)) == SEM_FAILED) {
        fprintf(stderr, "Cannot create semaphore.\n");
		return 0;
    }
    
    // Get shared memory 
    if ((fdShm = shm_open("/encoders-shared-mem", O_RDWR | O_CREAT | O_TRUNC, 0660)) == -1) {
        fprintf(stderr, "Cannot create shared memory.\n");
		return 0;
    }

    //  Truncate share memory /*
    if (ftruncate(fdShm, sizeof (encodersOutputStruct)) == -1) {
        fprintf(stderr, "Cannot truncate shared memory.\n");
		return 0;
    }

    // Map shared memory
    if ((encodersOutputDataThread->encodersOutputData = mmap(NULL, sizeof (encodersOutputStruct), PROT_READ, MAP_SHARED, fdShm, 0)) == MAP_FAILED) {
        fprintf(stderr, "Cannot map shared memory.\n");
		return 0;
    }

    sem_post(encodersOutputDataThread->mutexSem);
}

void readSensors(sensorsOutputThreadStruct* sensorsOutput)
{
    sensorsOutputStruct buffer;

    sem_wait(sensorsOutput->spoolSem);

    // Enter critcal section
    sem_wait(sensorsOutput->mutexSem);

    // Receive data
    memcpy(&buffer, sensorsOutput->sensorsOutputData, sizeof(sensorsOutputStruct));

    // Leave critical section
    sem_post(sensorsOutput->mutexSem);
}

void readEncoders(encodersOutputThreadStruct* encodersOutputDataThread)
{
    encodersOutputStruct buffer;

    // Enter critcal section
    sem_wait(encodersOutputDataThread->mutexSem);

    // Send data
    memcpy(&buffer,encodersOutputDataThread->encodersOutputData, sizeof(encodersOutputStruct));

    // Leave critical section
    sem_post(encodersOutputDataThread->mutexSem);

    // Increment the spool
    sem_post(encodersOutputDataThread->spoolSem); 


    decodeEncoders(encodersOutputDataThread);
}

void decodeEncoders(encodersOutputThreadStruct* encodersOutputDataThread)
{
    if (encodersOutputDataThread->encodersOutputData->leftEncoderSigA != lastLeftSigA)
    {
        if (encodersOutputDataThread->encodersOutputData->leftEncoderSigA != encodersOutputDataThread->encodersOutputData->leftEncoderSigB)
        {
            if (leftCounter < ENCODER_RESOLUTION)
            {
                leftCounter ++;
            }
            else
            {
                leftCounter = 0;
            }
        } 
        else
        {
            if (leftCounter > -ENCODER_RESOLUTION)
            {
                leftCounter --;
            }
            else
            {
                leftCounter = 0;
            }
        }
        leftAngle = (360 * leftCounter) / ENCODER_RESOLUTION;
    }
    lastLeftSigA = encodersOutputDataThread->encodersOutputData->leftEncoderSigA;

    if (encodersOutputDataThread->encodersOutputData->rightEncoderSigA != lastRightSigA)
    {
        if (encodersOutputDataThread->encodersOutputData->rightEncoderSigA != encodersOutputDataThread->encodersOutputData->rightEncoderSigB)
        {
            rightCounter ++;
        } 
        else
        {
            rightCounter --;
        }
        rightAngle = (360 * rightCounter) / ENCODER_RESOLUTION;
        rightAngle = rightAngle < 360 ? rightAngle : 0;
    }
    lastRightSigA = encodersOutputDataThread->encodersOutputData->rightEncoderSigA;
}

void* tReadSensorsOutputThreadFunc(void *cookie)
{
    sensorsOutputStruct buffer;
    sensorsOutputControllerProcessStruct* sensorsOutputDataThread = (sensorsOutputControllerProcessStruct*)cookie;

    if (!createSharedMemoryForSensorsOutput(sensorsOutputDataThread->sensorsOutputThread))
    {
        fprintf(stderr, "Cannot create robot output writer.\n");
		return NULL;
    }

    sem_post(sensorsOutputDataThread->sensorsOutputThread->mutexSem);

    for (;;)
    {
        // pthread_barrier_wait(sensorsOutputDataThread->robot->sensorsOutputWriterBarrier);

        // Enter critcal section
        sem_wait(sensorsOutputDataThread->sensorsOutputThread->mutexSem);

        // Send data
        memcpy(sensorsOutputDataThread->sensorsOutputThread->sensorsOutputData, &buffer, sizeof(sensorsOutputStruct));

        // Leave critical section
        sem_post(sensorsOutputDataThread->sensorsOutputThread->mutexSem);

	    // Increment the spool
        sem_post(sensorsOutputDataThread->sensorsOutputThread->spoolSem);
    }
}
