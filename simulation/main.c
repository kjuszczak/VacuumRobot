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

#include "sockets/common/socket.h"
#include "flat/db/creator.h"
#include "robot/creator.h"

/*
 * Mutex variables
 */
pthread_mutex_t visUdpMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t robotUdpMutex = PTHREAD_MUTEX_INITIALIZER;
/******************************************************/

/*
 * Global variables
 */
json_object *jobjFlat;
json_object *jobjRobot;
/******************************************************/

int main(int argc, char *argv[])
{

	int	status, i;

    /* Thread variable */
    pthread_t	tSimpleThread;
    /* Thread attributes variable */
    pthread_attr_t	aSimpleThreadAttr;

    /* Initialize thread attributes structure for FIFO scheduling */
	pthread_attr_init(&aSimpleThreadAttr);
	pthread_attr_setschedpolicy(&aSimpleThreadAttr, SCHED_FIFO);

	jobjFlat = json_object_new_object();
	createFlatJsonObj(jobjFlat);
	// printf("jobj from str:\n---\n%s\n---\n", json_object_to_json_string_ext(jobjFlat, JSON_C_TO_STRING_SPACED | JSON_C_TO_STRING_PRETTY));
	jobjRobot = json_object_new_object();

	tSocketData visSocketData = {1100, &visUdpMutex, jobjFlat, jobjRobot};
	pthread_mutex_lock(&visUdpMutex);

	/* Create thread */
	if ((status = pthread_create( &tSimpleThread, &aSimpleThreadAttr, tUdpThreadFunc, (void*) &visSocketData))) {
		fprintf(stderr, "Cannot create thread.\n");
		return 0;
	}

	usleep(2000000);
	createRobotUpdateJson(jobjRobot);
	createGarbageUpdateJson(jobjRobot);
	pthread_mutex_unlock(&visUdpMutex);

	/* Join (wait for) thread */
	pthread_join(tSimpleThread, NULL);

	return EXIT_SUCCESS;
}
