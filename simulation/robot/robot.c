#include "robot.h"
#include <unistd.h>
#include <stdio.h>
#include <math.h>
#include <pthread.h>

#define PI 3.14159265
#define RADIAN_PROP PI / 180
#define MAX_SENSOR_NUMBER 4

#define MAX_ANGULAR_VELOCITY 6.15   // [rad/s]
#define ROBOT_DIAMATER_CM 38        // [cm]
#define WHEEL_DIAMATER_M 0.065      // [m]

void* tUpdateSensorsThreadFunc(void *cookie)
{
    sensorThreadStruct* sensorData = (sensorThreadStruct*)cookie;

    pthread_mutex_lock(sensorData->robotThread->robot->robotMutex);
    float x = sensorData->robotThread->robot->x;
    float y = sensorData->robotThread->robot->y;
    pthread_mutex_unlock(sensorData->robotThread->robot->robotMutex);

    pthread_mutex_lock(sensorData->robotThread->robot->sensors[sensorData->sensorId]->sensorMutex);
    int xDirectory = sensorData->robotThread->robot->sensors[sensorData->sensorId]->xDirectory;
    int yDirectory = sensorData->robotThread->robot->sensors[sensorData->sensorId]->yDirectory;
    pthread_mutex_unlock(sensorData->robotThread->robot->sensors[sensorData->sensorId]->sensorMutex);

    float distanceFromObject = getDistanceFromWall(
        sensorData->robotThread->rooms->rooms[sensorData->robotThread->robot->roomId]->walls,
        xDirectory,
        yDirectory,
        x,
        y);

    pthread_mutex_lock(sensorData->robotThread->robot->sensors[sensorData->sensorId]->sensorMutex);
    sensorData->robotThread->robot->sensors[sensorData->sensorId]->distanceFromObject = distanceFromObject;
    pthread_mutex_unlock(sensorData->robotThread->robot->sensors[sensorData->sensorId]->sensorMutex);
}

void* tUpdateRoomIdThreadFunc(void *cookie)
{
    robotThreadStruct* robotThread = (robotThreadStruct*)cookie;

    pthread_mutex_lock(robotThread->robot->robotMutex);
    float x = robotThread->robot->x;
    float y = robotThread->robot->y;
    pthread_mutex_unlock(robotThread->robot->robotMutex);

    uint8_t roomId = getCurrentRoomWhereRobotIs(robotThread->rooms, x, y);

    pthread_mutex_lock(robotThread->robot->robotMutex);
    robotThread->robot->roomId = roomId;
    pthread_mutex_unlock(robotThread->robot->robotMutex);
}

void* tSendSensorMeasurementThreadFunc(void *cookie)
{
    float highLevelTime;
    sensorThreadStruct* sensorData = (sensorThreadStruct*)cookie;

    pthread_mutex_lock(sensorData->robotThread->robot->sensors[sensorData->sensorId]->sensorMutex);
    float distance = sensorData->robotThread->robot->sensors[sensorData->sensorId]->distanceFromObject;
    pthread_mutex_unlock(sensorData->robotThread->robot->sensors[sensorData->sensorId]->sensorMutex);

    // printf("Karolki: tSendSensorMeasurementThreadFunc: sensorId:%u, distance:%f\n", sensorData->sensorId, distance);

    if (distance <= 200)
    {
        highLevelTime = (distance * 1000 * 2) / 34;
    }
}

void* tSimulateWheelsThreadFunc(void *cookie)
{
    robotThreadStruct* robotThread = (robotThreadStruct*)cookie;
    float linearVelocity[2];
    for (size_t i = 0; i < 2; i++)
    {
        calculateVelocity(robotThread->robot->wheels[i], WHEEL_DIAMATER_M, MAX_ANGULAR_VELOCITY);
        linearVelocity[i] = robotThread->robot->wheels[i]->linearVelocity * 100; // [cm]
    }
    updateRobotParameters(robotThread->robot, linearVelocity[0], linearVelocity[1], SIMULATION_SAMPLE_TIME);
    updateSensorsDirectionParameters(robotThread->robot);
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
        robot->angle += (250 / (ROBOT_DIAMATER_CM / 2)) * time; // 250 == (rightWheelVelocity - leftWheelVelocity)
        pthread_mutex_unlock(robot->robotMutex);
    }
    else if (leftWheelVelocity == -rightWheelVelocity)
    {
        pthread_mutex_lock(robot->robotMutex);
        robot->angle += (250 / (ROBOT_DIAMATER_CM / 2)) * time; // 250 == (leftWheelVelocity - rightWheelVelocity)
        pthread_mutex_unlock(robot->robotMutex);
    }
    // printf("Karolki: updateRobotParameters: leftWheelVelocity:%f, rightWheelVelocity:%f, x:%f, y:%f, angle:%f\n",
    //         leftWheelVelocity, rightWheelVelocity, robot->x, robot->y, robot->angle);
}

void updateSensorsDirectionParameters(robotStruct* robot)
{
    float angle;
    pthread_mutex_lock(robot->robotMutex);
    angle = robot->angle;
    pthread_mutex_unlock(robot->robotMutex);
    changeDirection(robot->sensors, angle);
}
