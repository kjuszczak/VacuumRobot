#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <json-c/json.h>

#include "sockets/socket.h"
#include "flat/creator.h"
#include "robot/creator.h"

/*
 * Mutex variables
 */
pthread_mutex_t visUdpMutex = PTHREAD_MUTEX_INITIALIZER;
/******************************************************/

/*
 * Global variables
 */
json_object* jobjFlat;
json_object* jobjRobot;
tSocketData visSocketData;
/******************************************************/

int main(int argc, char *argv[])
{
	printf("Simulation main\n");
	jobjFlat = json_object_new_object();
	jobjRobot = json_object_new_object();

	visSocketData.port = 1100;
	visSocketData.jFlatObj = jobjFlat;
	visSocketData.jRobotObj = jobjRobot;

	/* Create flat */
	createFlatJsonObj(jobjFlat);
	sendFlatJson(&visSocketData);

	createThreadsForRobotSimulation(&visSocketData, getFlatPtr());

	for (;;) {}

	printf("Simulation main exits\n");
	return EXIT_SUCCESS;
}
