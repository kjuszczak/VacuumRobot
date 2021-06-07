#include "output.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/mman.h>
#include <pthread.h>

#include "../../pscommon/constants.h"
#include "../creator/encoder/encoder.h"
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
    if ((fdShm = shm_open("/sensors-shared-mem", O_RDWR | O_CREAT | O_TRUNC, 0660)) == -1) {
        LG_ERR("Cannot create shared memory.");
		return 0;
    }

    //  Truncate share memory
    if (ftruncate(fdShm, sizeof (sensorsOutputStruct)) == -1) {
        LG_ERR("Cannot truncate shared memory.");
		return 0;
    }

    // Map shared memory
    if ((sensorsOutputDataThread->sensorsOutputData = mmap(NULL, sizeof (sensorsOutputStruct), PROT_READ | PROT_WRITE, MAP_SHARED, fdShm, 0)) == MAP_FAILED) {
        LG_ERR("Cannot map shared memory.");
		return 0;
    }

    return 1;
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
        LG_ERR("Cannot create semaphore.");;
		return 0;
    }
    
    // Get shared memory 
    if ((fdShm = shm_open("/encoders-shared-mem", O_RDWR | O_CREAT | O_TRUNC, 0660)) == -1) {
        LG_ERR("Cannot create shared memory.");
		return 0;
    }

    //  Truncate share memory /*
    if (ftruncate(fdShm, sizeof (encodersOutputStruct)) == -1) {
        LG_ERR("Cannot truncate shared memory.");
		return 0;
    }

    // Map shared memory
    if ((encodersOutputDataThread->encodersOutputData = mmap(NULL, sizeof (encodersOutputStruct), PROT_READ, MAP_SHARED, fdShm, 0)) == MAP_FAILED) {
        LG_ERR("Cannot map shared memory.");
		return 0;
    }

    return 1;
}

int unmapShmForEncoders(encodersOutputThreadStruct* encodersOutputDataThread)
{
    // Unmap shared memory
    if (munmap(encodersOutputDataThread->encodersOutputData, sizeof (encodersOutputThreadStruct)) == -1) {
        LG_ERR("Cannot unmap shared memmory.");
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

void updateControllerEncoders(encodersOutputStruct* encodersOutputData, controllerStruct* controller)
{
    pthread_mutex_lock(controller->leftWheel->encoderMutex);
    controller->leftWheel->sigA = encodersOutputData->leftEncoderSigA;
    controller->leftWheel->sigB = encodersOutputData->leftEncoderSigB;
    pthread_mutex_unlock(controller->leftWheel->encoderMutex);

    pthread_mutex_lock(controller->rightWheel->encoderMutex);
    controller->rightWheel->sigA = encodersOutputData->rightEncoderSigA;
    controller->rightWheel->sigB = encodersOutputData->rightEncoderSigB;
    pthread_mutex_unlock(controller->rightWheel->encoderMutex);

    pthread_barrier_wait(controller->leftWheel->encoderBarrier);
    pthread_barrier_wait(controller->rightWheel->encoderBarrier);
}

void* tReadSensorsOutputThreadFunc(void *cookie)
{
    sensorsOutputStruct buffer;
    sensorsOutputControllerProcessStruct* sensorsOutputDataThread = (sensorsOutputControllerProcessStruct*)cookie;

    if (!createSharedMemoryForSensorsOutput(sensorsOutputDataThread->sensorsOutputThread))
    {
        LG_ERR("Cannot create robot output writer.");
		return NULL;
    }

    sem_post(sensorsOutputDataThread->sensorsOutputThread->mutexSem);

    for (;;)
    {
        pthread_barrier_wait(sensorsOutputDataThread->controller->sensorsOutputReaderBarrier);

        // Enter critcal section
        while (sem_wait(sensorsOutputDataThread->sensorsOutputThread->spoolSem)) {}

        // Send data
        memcpy(sensorsOutputDataThread->controller->sensors, sensorsOutputDataThread->sensorsOutputThread->sensorsOutputData->sensors, sizeof(sensorsOutputStruct));

        // Leave critical section
        sem_post(sensorsOutputDataThread->sensorsOutputThread->mutexSem);
    }
}

void* tReadEncodersOutputThreadFunc(void *cookie)
{
    encodersOutputStruct buffer;
    encodersOutputControllerProcessStruct* encodersOutputDataThread = (encodersOutputControllerProcessStruct*)cookie;

    if (!createSharedMemoryForEncodersOutput(encodersOutputDataThread->encodersOutputThread))
    {
        LG_ERR("Cannot create robot output reader.");
		return NULL;
    }

    sem_post(encodersOutputDataThread->encodersOutputThread->mutexSem);

    for (;;)
    {
        pthread_barrier_wait(encodersOutputDataThread->controller->encodersOutputReaderBarrier);

        while (sem_wait(encodersOutputDataThread->encodersOutputThread->spoolSem)) {}

        // Receive data
        memcpy(&buffer, encodersOutputDataThread->encodersOutputThread->encodersOutputData, sizeof(encodersOutputStruct));

        updateControllerEncoders(&buffer, encodersOutputDataThread->controller);
        pthread_barrier_wait(encodersOutputDataThread->controller->encodersCalculationBarrier);

        sem_post(encodersOutputDataThread->encodersOutputThread->mutexSem);

        // LG_INF("read: \tleftWheelSigA:%u, leftWheelSigB:%u, rightWheelSigA:%u, rightWheelSigB:%u, leftWheelAngle:%d, rightWheelAngle:%d",
        //     buffer.leftEncoderSigA,
        //     buffer.leftEncoderSigB,
        //     buffer.rightEncoderSigA,
        //     buffer.rightEncoderSigB,
        //     encodersOutputDataThread->controller->leftWheel->angle,
        //     encodersOutputDataThread->controller->rightWheel->angle);
    }
}
