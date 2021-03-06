#include "robot.h"
#include <unistd.h>
#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include <time.h>

#include "../flat/rooms/garbages/garbages.h"
#include "../io/input/input.h"
#include "../../pscommon/constants.h"
#include "../../pscommon/logger/log.h"

double roundedRobotAngle = 0;
int robotOutput = 1;
uint8_t timerCounter = 0;
uint8_t timerVisCounter = 0;

void* tUpdateSensorsThreadFunc(void *cookie)
{
    sensorThreadStruct* sensorData = (sensorThreadStruct*)cookie;

    for (;;)
    {
        /* Wait for periodic call */
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

        /* Activate output writer */
        pthread_barrier_wait(sensorData->robotThread->robot->sensorsOutputWriterBarrier);
    }
}

void* tUpdateEncoderThreadFunc(void *cookie)
{
    encoderThreadStruct* encoderData = (encoderThreadStruct*)cookie;

    for (;;)
    {
        /* Wait for periodic call */
        pthread_barrier_wait(encoderData->robotThread->robot->encoders[encoderData->encoderId]->encoderBarrier);
        
        /* Make sure that encoder output is set*/
        pthread_barrier_wait(encoderData->robotThread->robot->encodersUpdaterBarrier);

        pthread_mutex_lock(encoderData->robotThread->robot->wheels[encoderData->encoderId]->motorMutex);
        int angle = encoderData->robotThread->robot->wheels[encoderData->encoderId]->angle;
        pthread_mutex_unlock(encoderData->robotThread->robot->wheels[encoderData->encoderId]->motorMutex);

        updateEncoder(
            encoderData->robotThread->robot->encoders[encoderData->encoderId],
            angle,
            encoderData->robotThread->robot->encodersOutputWriterBarrier);

        /* Activate output writer */
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
    roundedRobotAngle = robotThread->robot->angle;

    for (;;)
    {
        pthread_barrier_wait(robotThread->robot->robotMainPeriodicFuncBarrier);

        /* OUTPUT */
        pthread_barrier_wait(robotThread->robot->encodersPeriodicBarrier);
        pthread_barrier_wait(robotThread->robot->sensorsPeriodicBarrier);

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

        if (timerVisCounter < VISUALIZATION_MAX_COUNTER)
        {
            timerVisCounter++;
            continue;
        }
        mq_send(*robotThread->outputMQueue, (char *)&robotOutput, sizeof(int), 0);
        LG_INF("robot->x:%lf, robot->y:%lf, robot->angle:%lf", robotThread->robot->x, robotThread->robot->y, robotThread->robot->angle);
        timerVisCounter = 0;
    }
}

void updateRobotParameters(robotStruct* robot, double leftWheelVelocity, double rightWheelVelocity, double step)
{
    if (robot == NULL)
    {
        LG_WRN("Cannot copy json string to buffer");
        return;
    }

    if (leftWheelVelocity == rightWheelVelocity)
    {
        pthread_mutex_lock(robot->robotMutex);
        robot->angle = roundedRobotAngle == 360 ? 0 : roundedRobotAngle;
        robot->x += step * leftWheelVelocity * cos(robot->angle * RADIAN_PROP);
        robot->y += step * leftWheelVelocity * -sin(robot->angle * RADIAN_PROP);
        pthread_mutex_unlock(robot->robotMutex);
    }
    else if (leftWheelVelocity < 0 && rightWheelVelocity > 0)
    {
        pthread_mutex_lock(robot->robotMutex);
        robot->angle += (step * rightWheelVelocity / (ROBOT_DIAMATER_CM / 2)) * DEGREE_PROP;
        robot->angle = robot->angle < 360 ? robot->angle : robot->angle - 360;
        roundedRobotAngle = round(robot->angle);
        pthread_mutex_unlock(robot->robotMutex);
    }
    else if (leftWheelVelocity > 0 && rightWheelVelocity < 0)
    {
        pthread_mutex_lock(robot->robotMutex);
        robot->angle -= (step * leftWheelVelocity / (ROBOT_DIAMATER_CM / 2)) * DEGREE_PROP;
        robot->angle = robot->angle >= 0 ? robot->angle : 360 + robot->angle;
        roundedRobotAngle = round(robot->angle);
        pthread_mutex_unlock(robot->robotMutex);
    }
}

void updateSensorsAnglesParameters(robotStruct* robot)
{
    double angle;
    pthread_mutex_lock(robot->robotMutex);
    angle = robot->angle;
    pthread_mutex_unlock(robot->robotMutex);
    updateAngles(robot->sensors, angle);
}
