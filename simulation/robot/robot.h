#ifndef ROBOT_H
#define ROBOT_H

#include "sensors/sensor.h"
#include "motors/motor.h"
#include "encoder/encoder.h"
#include "../flat/rooms/rooms.h"

#include "stdint.h"
#include <pthread.h>
#include <mqueue.h>

/* Robot parameters */

#define SIMULATION_SAMPLE_TIME 0.005  // [s]

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
    pthread_barrier_t* roomIdUpdaterBarrier;
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

/* Threads structs */
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
void updateSensorsDirectionParameters(robotStruct* robot);

#endif //ROBOT_H
