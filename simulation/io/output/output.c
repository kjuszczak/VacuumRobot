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

#include "../../pscommon/logger/log.h"

int createSharedMemoryForSensorsOutput(sensorsOutputThreadStruct* sensorsOutputDataThread)
{
    int fdShm;
    
    //  Create mutex semaphore
    if ((sensorsOutputDataThread->mutexSem = sem_open("/sensors-sem-mutex", O_CREAT, 0660, 0)) == SEM_FAILED) {
        LG_ERR("Cannot create semaphore.");
		return 0;
    }

    // Create spool semaphore
    if ((sensorsOutputDataThread->spoolSem = sem_open("/sensors-sem-spool", O_CREAT, 0660, 0)) == SEM_FAILED) {
        LG_ERR("Cannot create semaphore.");
		return 0;
    }
    
    // Get shared memory 
    if ((fdShm = shm_open("/sensors-shared-mem", O_RDWR | O_CREAT, 0660)) == -1) {
        LG_ERR("Cannot create shared memory.");
		return 0;
    }

    // Map shared memory
    if ((sensorsOutputDataThread->sensorsOutputData = mmap(NULL, sizeof (sensorsOutputStruct), PROT_READ | PROT_WRITE, MAP_SHARED, fdShm, 0)) == MAP_FAILED) {
        LG_ERR("Cannot map shared memory.");
		return 0;
    }
}

int createSharedMemoryForEncodersOutput(encodersOutputThreadStruct* encodersOutputDataThread)
{
    int fdShm;
    
    //  Create mutex semaphore
    if ((encodersOutputDataThread->mutexSem = sem_open("/encoders-sem-mutex", O_CREAT, 0660, 0)) == SEM_FAILED) {
        LG_ERR("Cannot create semaphore.");
		return 0;
    }

    // Create spool semaphore
    if ((encodersOutputDataThread->spoolSem = sem_open("/encoders-sem-spool", O_CREAT, 0660, 0)) == SEM_FAILED) {
        LG_ERR("Cannot create semaphore.");
		return 0;
    }
    
    // Get shared memory 
    if ((fdShm = shm_open("/encoders-shared-mem", O_RDWR | O_CREAT, 0660)) == -1) {
        LG_ERR("Cannot create shared memory.");
		return 0;
    }

    // Map shared memory
    if ((encodersOutputDataThread->encodersOutputData = mmap(NULL, sizeof (encodersOutputStruct), PROT_WRITE, MAP_SHARED, fdShm, 0)) == MAP_FAILED) {
        LG_ERR("Cannot map shared memory.");
		return 0;
    }

    return 1;
}

int unmapShmForEncoders(encodersOutputThreadStruct* encodersOutputDataThread)
{
    // Unmap shared memory
    if (munmap(encodersOutputDataThread->encodersOutputData, sizeof (encodersOutputThreadStruct)) == -1) {
        LG_ERR("Cannot unmap shared memory.");
		return 0;
    }
    
    return 0;
}

int closeSharedMemoryForEncodersOutput(encodersOutputThreadStruct* encodersOutputDataThread)
{
    sem_destroy(encodersOutputDataThread->mutexSem);
    sem_destroy(encodersOutputDataThread->spoolSem);
    unmapShmForEncoders(encodersOutputDataThread);
}

int unmapShmForSensors(sensorsOutputThreadStruct* sensorsOutputDataThread)
{
    // Unmap shared memory
    if (munmap(sensorsOutputDataThread->sensorsOutputData, sizeof (sensorsOutputThreadStruct)) == -1) {
        LG_ERR("Cannot unmap shared memmory.");
		return 0;
    }
    
    return 0;
}

int closeSharedMemoryForSensorsOutput(sensorsOutputThreadStruct* sensorsOutputDataThread)
{
    sem_destroy(sensorsOutputDataThread->mutexSem);
    sem_destroy(sensorsOutputDataThread->spoolSem);
    unmapShmForSensors(sensorsOutputDataThread);
}

void fillBufferWithSensors(sensorsOutputStruct* buffer, robotStruct* robot)
{
    pthread_mutex_lock(robot->sensors[0]->sensorMutex);
    buffer->sensors[0] = robot->sensors[0]->distanceFromObject;
    pthread_mutex_unlock(robot->sensors[0]->sensorMutex);

    pthread_mutex_lock(robot->sensors[1]->sensorMutex);
    buffer->sensors[1] = robot->sensors[1]->distanceFromObject;
    pthread_mutex_unlock(robot->sensors[1]->sensorMutex);

    pthread_mutex_lock(robot->sensors[2]->sensorMutex);
    buffer->sensors[2] = robot->sensors[2]->distanceFromObject;
    pthread_mutex_unlock(robot->sensors[2]->sensorMutex);

    pthread_mutex_lock(robot->sensors[3]->sensorMutex);
    buffer->sensors[3] = robot->sensors[3]->distanceFromObject;
    pthread_mutex_unlock(robot->sensors[3]->sensorMutex);
}

void fillBufferWithEncoders(encodersOutputStruct* buffer, robotStruct* robot)
{
    pthread_mutex_lock(robot->encoders[0]->encoderMutex);
    buffer->leftEncoderSigA = robot->encoders[0]->sigA;
    buffer->leftEncoderSigB = robot->encoders[0]->sigB;
    int leftWheelAngle = robot->encoders[0]->lastAngle;
    pthread_mutex_unlock(robot->encoders[0]->encoderMutex);

    pthread_mutex_lock(robot->encoders[1]->encoderMutex);
    buffer->rightEncoderSigA = robot->encoders[1]->sigA;
    buffer->rightEncoderSigB = robot->encoders[1]->sigB;
    int rightWheelAngle = robot->encoders[1]->lastAngle;
    pthread_mutex_unlock(robot->encoders[1]->encoderMutex);

    // LG_INF("write: \tleftWheelSigA:%u, leftWheelSigB:%u, rightWheelSigA:%u, rightWheelSigB:%u, leftWheelAngle:%d, rightWheelAngle:%d",
    //         buffer->leftEncoderSigA,
    //         buffer->leftEncoderSigB,
    //         buffer->rightEncoderSigA,
    //         buffer->rightEncoderSigB,
    //         leftWheelAngle,
    //         rightWheelAngle);
}

void* tWriteSensorsOutputThreadFunc(void *cookie)
{
    sensorsOutputStruct buffer;
    sensorsOutputSimProcessStruct* sensorsOutputDataThread = (sensorsOutputSimProcessStruct*)cookie;

    if (!createSharedMemoryForSensorsOutput(sensorsOutputDataThread->sensorsOutputThread))
    {
        LG_ERR("Cannot create robot output writer.");
		return NULL;
    }

    for (;;)
    {
        pthread_barrier_wait(sensorsOutputDataThread->robot->sensorsOutputWriterBarrier);

        //Fill buffer
        fillBufferWithSensors(&buffer, sensorsOutputDataThread->robot);

        // Enter critcal section
        while (sem_wait(sensorsOutputDataThread->sensorsOutputThread->mutexSem)) {}

        // Send data
        memcpy(sensorsOutputDataThread->sensorsOutputThread->sensorsOutputData, &buffer, sizeof(sensorsOutputStruct));

	    // Increment the spool
        sem_post(sensorsOutputDataThread->sensorsOutputThread->spoolSem);
    }
}

void* tWriteEncodersOutputThreadFunc(void *cookie)
{
    encodersOutputStruct buffer;
    encodersOutputSimProcessStruct* encodersOutputDataThread = (encodersOutputSimProcessStruct*)cookie;

    if (!createSharedMemoryForEncodersOutput(encodersOutputDataThread->encodersOutputThread))
    {
        LG_ERR("Cannot create robot output writer.");
		return NULL;
    }

    pthread_barrier_wait(encodersOutputDataThread->robot->encodersUpdaterBarrier);

    for (;;)
    {
        pthread_barrier_wait(encodersOutputDataThread->robot->encodersOutputWriterBarrier);

        //Fill buffer
        fillBufferWithEncoders(&buffer, encodersOutputDataThread->robot);

        while (sem_wait(encodersOutputDataThread->encodersOutputThread->mutexSem)) {}

        // Send data
        memcpy(encodersOutputDataThread->encodersOutputThread->encodersOutputData, &buffer, sizeof(encodersOutputStruct));

	    // Increment the spool
        sem_post(encodersOutputDataThread->encodersOutputThread->spoolSem);

        pthread_barrier_wait(encodersOutputDataThread->robot->encodersUpdaterBarrier);
    }
}
