#ifndef ROBOT_H
#define ROBOT_H

#include "stdint.h"
#include <pthread.h>
#include "sensors/sensor.h"
#include "motors/motor.h"
#include "../flat/db/rooms.h"

/* Robot parameters */

#define SIMULATION_SAMPLE_TIME 0.01  // [s]

typedef struct robot
{
    uint8_t roomId;
    float x;
    float y;
    float angle;
    sensorStruct** sensors;
    motorStruct** wheels;
    pthread_mutex_t* robotMutex;
} robotStruct;

/* Threads structs */
typedef struct robotThread
{
    robotStruct* robot;
    roomsStruct* rooms;
} robotThreadStruct;


typedef struct sensorThread
{
    robotThreadStruct* robotThread;
    uint8_t sensorId;
} sensorThreadStruct;
/******************************************************/

/* Threads structs */
void* tUpdateSensorsThreadFunc(void *cookie);
void* tUpdateRoomIdThreadFunc(void *cookie);
void* tUpdateGarbagesThreadFunc(void *cookie);
void* tUpdateGarbageContainerLevelThreadFunc(void *cookie);
void* tSimulateWheelsThreadFunc(void *cookie);
void* tUpdateEncoderThreadFunc(void *cookie);

void* tSendSensorMeasurementThreadFunc(void *cookie);
void* tSendGarbageContainerLeverThreadFunc(void *cookie);
void* tSendEncoderMeasurementThreadFunc(void *cookie);
/******************************************************/

/* leftWheelVelocity, rightWheelVelocity - [cm/s], time - [s] */
void updateRobotParameters(robotStruct* robot, float leftWheelVelocity, float rightWheelVelocity, float time);
void updateSensorsDirectionParameters(robotStruct* robot);

#endif //ROBOT_H
