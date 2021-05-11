#include "robot.h"
#include <unistd.h>
#include <stdio.h>
#include <math.h>

#define PI 3.14159265
#define RADIAN_PROP PI / 180
#define MAX_SENSOR_NUMBER 4

#define ROBOT_DIAMATER 38 // [cm]

void* tUpdateSensorsThreadFunc(void *cookie)
{
    robotThreadStruct* sensorData = (robotThreadStruct*)cookie;

    pthread_mutex_lock(sensorData->robot->robotMutex);
    uint16_t x = sensorData->robot->x;
    uint16_t y = sensorData->robot->y;
    pthread_mutex_unlock(sensorData->robot->robotMutex);

    pthread_mutex_lock(sensorData->robot->sensors[sensorData->sensorId]->sensorMutex);
    int xDirectory = sensorData->robot->sensors[sensorData->sensorId]->xDirectory;
    int yDirectory = sensorData->robot->sensors[sensorData->sensorId]->yDirectory;
    pthread_mutex_unlock(sensorData->robot->sensors[sensorData->sensorId]->sensorMutex);

    float distanceFromObject = getDistanceFromWall(
        sensorData->rooms->rooms[sensorData->robot->roomId]->walls,
        xDirectory,
        yDirectory,
        x,
        y);

    pthread_mutex_lock(sensorData->robot->sensors[sensorData->sensorId]->sensorMutex);
    sensorData->robot->sensors[sensorData->sensorId]->distanceFromObject = distanceFromObject;
    pthread_mutex_unlock(sensorData->robot->sensors[sensorData->sensorId]->sensorMutex);
}

void* tUpdateRoomIdThreadFunc(void *cookie)
{
    robotThreadStruct* sensorData = (robotThreadStruct*)cookie;

    pthread_mutex_lock(sensorData->robot->robotMutex);
    uint16_t x = sensorData->robot->x;
    uint16_t y = sensorData->robot->y;
    pthread_mutex_unlock(sensorData->robot->robotMutex);

    uint8_t roomId = getCurrentRoomWhereRobotIs(sensorData->rooms, x, y);

    pthread_mutex_lock(sensorData->robot->robotMutex);
    sensorData->robot->roomId = roomId;
    pthread_mutex_unlock(sensorData->robot->robotMutex);
}

void* tSendSensorMeasurementThreadFunc(void *cookie)
{
    float highLevelTime;
    robotThreadStruct* sensorData = (robotThreadStruct*)cookie;

    pthread_mutex_lock(sensorData->robot->sensors[sensorData->sensorId]->sensorMutex);
    float distance = sensorData->robot->sensors[sensorData->sensorId]->distanceFromObject;
    pthread_mutex_unlock(sensorData->robot->sensors[sensorData->sensorId]->sensorMutex);

    if (distance <= 200)
    {
        highLevelTime = (distance * 1000 * 2) / 34;
    }
}

void updateRobotParameters(robotStruct* robot, float leftWheelVelocity, float rightWheelVelocity, float time)
{
    if (robot == NULL)
    {
        fprintf(stderr, "Cannot copy json string to buffer\n");
        return;
    }
    if (leftWheelVelocity == rightWheelVelocity)
    {
        pthread_mutex_lock(robot->robotMutex);
        robot->x += cos(robot->angle * RADIAN_PROP) * leftWheelVelocity * time;
        robot->y += -sin(robot->angle * RADIAN_PROP) * leftWheelVelocity * time;
        pthread_mutex_unlock(robot->robotMutex);
    }
    else if (-leftWheelVelocity == rightWheelVelocity)
    {
        pthread_mutex_lock(robot->robotMutex);
        robot->angle += ((rightWheelVelocity - leftWheelVelocity) / ROBOT_DIAMATER) * time;
        pthread_mutex_unlock(robot->robotMutex);
    }
    else if (leftWheelVelocity == -rightWheelVelocity)
    {
        pthread_mutex_lock(robot->robotMutex);
        robot->angle += ((leftWheelVelocity - rightWheelVelocity) / ROBOT_DIAMATER) * time;
        pthread_mutex_unlock(robot->robotMutex);
    }
    printf("Karolki: updateRobotParameters: x:%u, y:%u, angle:%f\n", robot->x, robot->y, robot->angle);
}
