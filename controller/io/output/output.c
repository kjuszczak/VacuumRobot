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
    if ((fdShm = shm_open("/sensors-shared-mem", O_RDWR | O_CREAT, 0660)) == -1) {
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
    if ((fdShm = shm_open("/encoders-shared-mem", O_RDWR | O_CREAT, 0660)) == -1) {
        fprintf(stderr, "Cannot create shared memory.\n");
		return 0;
    }

    //  Truncate share memory /*
    if (ftruncate(fdShm, sizeof (encodersOutputStruct)) == -1) {
        fprintf(stderr, "Cannot truncate shared memory.\n");
		return 0;
    }

    // Map shared memory
    if ((encodersOutputDataThread->encodersOutputData = mmap(NULL, sizeof (encodersOutputStruct), PROT_READ | PROT_WRITE, MAP_SHARED, fdShm, 0)) == MAP_FAILED) {
        fprintf(stderr, "Cannot map shared memory.\n");
		return 0;
    }

    sem_post(encodersOutputDataThread->mutexSem);
}

void readSensors(sensorsOutputThreadStruct* sensorsOutput)
{
    sensorsOutputStruct buffer;

    // Enter critcal section
    sem_wait(sensorsOutput->mutexSem);

    // Receive data
    memcpy(&buffer, sensorsOutput->sensorsOutputData, sizeof(sensorsOutputStruct));

    // Leave critical section
    sem_post(sensorsOutput->mutexSem);

    // Increment the spool
    sem_post(sensorsOutput->spoolSem);

    // printf("Controller: Sensor1:%.5lf, Sensor2:%.5lf, Sensor3:%.5lf, Sensor4:%.5lf\n",
    //         buffer.sensors[0],
    //         buffer.sensors[1],
    //         buffer.sensors[2],
    //         buffer.sensors[3]);
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

    // printf("Controller: leftEncoderSigA:%d, leftEncoderSigB:%d, rightEncoderSigA:%d, rightEncoderSigB:%d\n",
    //     buffer.leftEncoderSigA,
    //     buffer.leftEncoderSigB,
    //     buffer.rightEncoderSigA,
    //     buffer.rightEncoderSigB);
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
