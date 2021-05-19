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
#include <signal.h>

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

static void exit_handler(int);

int main(int argc, char *argv[])
{
	printf("Simulation main\n");

    /* Create empty signal set to run sigsuspend */
    sigset_t mask;
    sigemptyset(&mask);
    sigprocmask(0, NULL, &mask);
	sigaddset(&mask, SIGRTMIN + 1);
    sigprocmask(SIG_SETMASK, &mask, NULL);

    /* Prepare sigaction struct for handler */
    struct sigaction action;
    action.sa_handler = exit_handler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;

    /* Register signal handler for SIGRTMIN */
    if (sigaction(SIGRTMIN, &action, NULL) < 0) 
	{
        fprintf(stderr, "Cannot register SIGRTMIN handler.\n");
        return -1;
    }

	jobjFlat = json_object_new_object();
	jobjRobot = json_object_new_object();

	visSocketData.port = 1100;
	visSocketData.jFlatObj = jobjFlat;
	visSocketData.jRobotObj = jobjRobot;

	/* Create flat */
	createFlatJsonObj(jobjFlat);
	sendFlatJson(&visSocketData);

	createThreadsForRobotSimulation(&visSocketData, getFlatPtr());

	while(1)
	{
		sigsuspend(&mask);
	}

	return EXIT_SUCCESS;
}

/* Exit signal handler function */
void exit_handler(int sig)
{
	printf("Simulation main exits\n");
    exit(EXIT_SUCCESS);
}
