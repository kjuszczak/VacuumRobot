#ifndef ROBOT_H
#define ROBOT_H

#include "sensors/sensor.h"
#include "motors/motor.h"
#include "encoder/encoder.h"
#include "../flat/rooms/rooms.h"
#include "../../pscommon/constants.h"

#include "stdint.h"
#include <pthread.h>
#include <mqueue.h>

/* Robot parameters */

typedef struct robot
{
    uint8_t roomId;
    double x;
    double y;
    double angle;
    sensorStruct** sensors;
    motorStruct** wheels;
    encoderStruct** encoders;
    pthread_mutex_t* robotMutex;
    pthread_barrier_t* robotMainPeriodicFuncBarrier;
    pthread_barrier_t* roomIdUpdaterBarrier;
    pthread_barrier_t* sensorsOutputWriterBarrier;
    pthread_barrier_t* encodersOutputWriterBarrier;
    pthread_barrier_t* encodersUpdaterBarrier;
    pthread_barrier_t* wheelsPwmInputReaderBarrier;
    pthread_barrier_t* garbageUpdaterBarrier;
    // mqd_t* encodersOutputMQueue;
} robotStruct;

/* Threads structs */
typedef struct robotThread
{
    robotStruct* robot;
    roomsStruct* rooms;
    mqd_t* outputMQueue;
} robotThreadStruct;

typedef struct sensorThread
{
    robotThreadStruct* robotThread;
    uint8_t sensorId;
} sensorThreadStruct;

typedef struct encoderThread
{
    robotThreadStruct* robotThread;
    uint8_t encoderId;
} encoderThreadStruct;
/******************************************************/

/* Threads functions */
void* tMainRobotPeriodicThreadFunc(void *cookie);

void* tUpdateSensorsThreadFunc(void *cookie);
void* tUpdateEncoderThreadFunc(void *cookie);
void* tUpdateRoomIdThreadFunc(void *cookie);
void* tUpdateGarbagesThreadFunc(void *cookie);
void* tUpdateGarbageContainerLevelThreadFunc(void *cookie);

void* tSendSensorMeasurementThreadFunc(void *cookie);
void* tSendGarbageContainerLeverThreadFunc(void *cookie);
void* tSendEncoderMeasurementThreadFunc(void *cookie);
/******************************************************/

/* leftWheelVelocity, rightWheelVelocity - [cm/s], time - [s] */
void updateRobotParameters(robotStruct* robot, double leftWheelVelocity, double rightWheelVelocity, double time);
void updateSensorsAnglesParameters(robotStruct* robot);

#endif //ROBOT_H
