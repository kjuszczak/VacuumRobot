#ifndef ROBOT_H
#define ROBOT_H

#include "stdint.h"
#include <pthread.h>
#include "sensors/sensor.h"
#include "../flat/db/rooms.h"

typedef struct robot
{
    uint8_t roomId;
    uint16_t x;
    uint16_t y;
    float angle;
    sensorStruct** sensors;
    pthread_mutex_t* robotMutex;
} robotStruct;

typedef struct robotThread
{
    robotStruct* robot;
    roomsStruct* rooms;
    uint8_t sensorId;
} robotThreadStruct;

void* tUpdateSensorsThreadFunc(void *cookie);
void* tUpdateRoomIdThreadFunc(void *cookie);
void* tUpdateGarbagesThreadFunc(void *cookie);
void* tUpdateGarbageContainerLevelThreadFunc(void *cookie);
void* tUpdateEncoderThreadFunc(void *cookie);

void* tSendSensorMeasurementThreadFunc(void *cookie);
void* tSendGarbageContainerLeverThreadFunc(void *cookie);
void* tSendEncoderMeasurementThreadFunc(void *cookie);

/* leftWheelVelocity, rightWheelVelocity - [cm/s], time - [s] */
void updateRobotParameters(robotStruct* robot, float leftWheelVelocity, float rightWheelVelocity, float time);

#endif //ROBOT_H
