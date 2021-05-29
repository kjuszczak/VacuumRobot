#include "robot.h"
#include <unistd.h>
#include <stdio.h>
#include <math.h>
#include <pthread.h>

#include "../flat/rooms/garbages/garbages.h"
#include "../io/input/input.h"
#include "../../pscommon/constants.h"
int lol = 1;
int robotOutput = 1;

uint8_t timerCounter = 0;
uint8_t timerVisCounter = 0;

void* tUpdateSensorsThreadFunc(void *cookie)
{
    sensorThreadStruct* sensorData = (sensorThreadStruct*)cookie;

    for (;;)
    {
        pthread_barrier_wait(sensorData->robotThread->robot->sensors[sensorData->sensorId]->sensorBarrier);

        pthread_mutex_lock(sensorData->robotThread->robot->robotMutex);
        double x = sensorData->robotThread->robot->x;
        double y = sensorData->robotThread->robot->y;
        pthread_mutex_unlock(sensorData->robotThread->robot->robotMutex);

        pthread_mutex_lock(sensorData->robotThread->robot->sensors[sensorData->sensorId]->sensorMutex);
        double angle = sensorData->robotThread->robot->sensors[sensorData->sensorId]->angle;
        pthread_mutex_unlock(sensorData->robotThread->robot->sensors[sensorData->sensorId]->sensorMutex);

        double distanceFromObject = getDistanceFromWall(
            sensorData->robotThread->rooms->rooms[sensorData->robotThread->robot->roomId]->walls,
            sensorData->robotThread->rooms->rooms[sensorData->robotThread->robot->roomId]->doors,
            x,
            y,
            angle);

        pthread_mutex_lock(sensorData->robotThread->robot->sensors[sensorData->sensorId]->sensorMutex);
        sensorData->robotThread->robot->sensors[sensorData->sensorId]->distanceFromObject = distanceFromObject;
        pthread_mutex_unlock(sensorData->robotThread->robot->sensors[sensorData->sensorId]->sensorMutex);

        pthread_barrier_wait(sensorData->robotThread->robot->sensorsOutputWriterBarrier);
    }
}

void* tUpdateEncoderThreadFunc(void *cookie)
{
    encoderThreadStruct* encoderData = (encoderThreadStruct*)cookie;

    for (;;)
    {
        pthread_barrier_wait(encoderData->robotThread->robot->encoders[encoderData->encoderId]->encoderBarrier);
        
        pthread_barrier_wait(encoderData->robotThread->robot->encodersUpdaterBarrier);

        pthread_mutex_lock(encoderData->robotThread->robot->wheels[encoderData->encoderId]->motorMutex);
        int angle = encoderData->robotThread->robot->wheels[encoderData->encoderId]->angle;
        pthread_mutex_unlock(encoderData->robotThread->robot->wheels[encoderData->encoderId]->motorMutex);

        updateEncoder(
            encoderData->robotThread->robot->encoders[encoderData->encoderId],
            angle,
            encoderData->robotThread->robot->encodersOutputWriterBarrier);

        pthread_barrier_wait(encoderData->robotThread->robot->encodersOutputWriterBarrier);
    }
}

void* tUpdateRoomIdThreadFunc(void *cookie)
{
    robotThreadStruct* robotThread = (robotThreadStruct*)cookie;

    for (;;)
    {
        pthread_barrier_wait(robotThread->robot->roomIdUpdaterBarrier);

        pthread_mutex_lock(robotThread->robot->robotMutex);
        double x = robotThread->robot->x;
        double y = robotThread->robot->y;
        pthread_mutex_unlock(robotThread->robot->robotMutex);

        uint8_t roomId = getCurrentRoomWhereRobotIs(robotThread->rooms, x, y);

        pthread_mutex_lock(robotThread->robot->robotMutex);
        robotThread->robot->roomId = roomId;
        pthread_mutex_unlock(robotThread->robot->robotMutex);
    }
}

void* tUpdateGarbagesThreadFunc(void *cookie)
{
    robotThreadStruct* robotThread = (robotThreadStruct*)cookie;
    for (;;)
    {
        pthread_barrier_wait(robotThread->robot->garbageUpdaterBarrier);
        deleteGarbageIfVacuumed(robotThread->rooms->rooms[robotThread->robot->roomId]->garbages, robotThread->robot->x, robotThread->robot->y);
    }
}

/* Input - [2xPWM], Output - [4xSensors, 2xEncoder] */
void* tMainRobotPeriodicThreadFunc(void *cookie)
{
    robotThreadStruct* robotThread = (robotThreadStruct*)cookie;
    double linearVelocity[2];

    for (;;)
    {
        pthread_barrier_wait(robotThread->robot->robotMainPeriodicFuncBarrier);

        pthread_barrier_wait(robotThread->robot->encoders[0]->encoderBarrier);
        pthread_barrier_wait(robotThread->robot->encoders[1]->encoderBarrier);

        pthread_barrier_wait(robotThread->robot->sensors[0]->sensorBarrier);
        pthread_barrier_wait(robotThread->robot->sensors[1]->sensorBarrier);
        pthread_barrier_wait(robotThread->robot->sensors[2]->sensorBarrier);
        pthread_barrier_wait(robotThread->robot->sensors[3]->sensorBarrier);

        if (timerCounter)
        {
            timerCounter = 0;
            continue;
        }
        timerCounter++;

        calculateVelocity(robotThread->robot->wheels[0], WHEEL_DIAMATER_M, MAX_ANGULAR_VELOCITY);
        calculateMotorAngle(robotThread->robot->wheels[0], SIMULATION_SAMPLE_TIME);

        linearVelocity[0] = robotThread->robot->wheels[0]->linearVelocity * 100; // [cm]
        calculateVelocity(robotThread->robot->wheels[1], WHEEL_DIAMATER_M, MAX_ANGULAR_VELOCITY);
        calculateMotorAngle(robotThread->robot->wheels[1], SIMULATION_SAMPLE_TIME);
        linearVelocity[1] = robotThread->robot->wheels[1]->linearVelocity * 100; // [cm]

        updateRobotParameters(robotThread->robot, linearVelocity[0], linearVelocity[1], SIMULATION_SAMPLE_TIME);
        updateSensorsAnglesParameters(robotThread->robot);

        pthread_barrier_wait(robotThread->robot->roomIdUpdaterBarrier);
        pthread_barrier_wait(robotThread->robot->garbageUpdaterBarrier);

        if (timerVisCounter < 10)
        {
            timerVisCounter++;
            continue;
        }
        mq_send(*robotThread->outputMQueue, (char *)&robotOutput, sizeof(int), 0);
        timerVisCounter = 0;
    }
}

void updateRobotParameters(robotStruct* robot, double leftWheelVelocity, double rightWheelVelocity, double time)
{
    if (robot == NULL)
    {
        fprintf(stderr, "Cannot copy json string to buffer\n");
        return;
    }
    if (leftWheelVelocity == rightWheelVelocity)
    {
        pthread_mutex_lock(robot->robotMutex);
        robot->x += time * leftWheelVelocity * cos(robot->angle * RADIAN_PROP);
        robot->y += time * leftWheelVelocity * -sin(robot->angle * RADIAN_PROP);
        pthread_mutex_unlock(robot->robotMutex);
    }
    else if (leftWheelVelocity < 0 && rightWheelVelocity > 0)
    {
        pthread_mutex_lock(robot->robotMutex);
        robot->angle += (time * (250 / (ROBOT_DIAMATER_CM / 2))); // 250 == (rightWheelVelocity - leftWheelVelocity)
        robot->angle = robot->angle < 360 ? robot->angle : 0;
        pthread_mutex_unlock(robot->robotMutex);
    }
    else if (leftWheelVelocity > 0 && rightWheelVelocity < 0)
    {
        pthread_mutex_lock(robot->robotMutex);
        robot->angle -= time * (250 / (ROBOT_DIAMATER_CM / 2)); // 250 == (leftWheelVelocity - rightWheelVelocity)
        robot->angle = robot->angle > -360 ? robot->angle : 0;
        pthread_mutex_unlock(robot->robotMutex);
    }
    // printf("robot->x:%lf, robot->y:%lf, robot->angle:%lf\n", robot->x, robot->y, robot->angle);
}

void updateSensorsAnglesParameters(robotStruct* robot)
{
    double angle;
    pthread_mutex_lock(robot->robotMutex);
    angle = robot->angle;
    pthread_mutex_unlock(robot->robotMutex);
    updateAngles(robot->sensors, angle);
}
