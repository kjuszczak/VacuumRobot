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

#define MAXLEN 1000

/*
 * Thread functions
 */
/* UDP communication with Visualization process */
void *tUdpVisualizationThreadFunc(void *);
/******************************************************/

/*
 * Mutex variables
 */
pthread_mutex_t flatUdpMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t robotUdpMutex = PTHREAD_MUTEX_INITIALIZER;
/******************************************************/

/*
 * Global variables
 */
json_object *jobjFlat;
json_object *jobjRobot;
/******************************************************/

int main(int argc, char *argv[]) {

int	status, i;

    /* Thread variable */
    pthread_t	tSimpleThread;
    /* Thread attributes variable */
    pthread_attr_t	aSimpleThreadAttr;

    /* Initialize thread attributes structure for FIFO scheduling */
	pthread_attr_init(&aSimpleThreadAttr);
	pthread_attr_setschedpolicy(&aSimpleThreadAttr, SCHED_FIFO);

	/* Create thread */
	if ((status = pthread_create( &tSimpleThread, &aSimpleThreadAttr, tUdpVisualizationThreadFunc, NULL))) {
		fprintf(stderr, "Cannot create thread.\n");
		return 0;
	}

	/* Join (wait for) thread */
	pthread_join(tSimpleThread, NULL);

	return EXIT_SUCCESS;
}

/*
 *  Thread functions
 */
void *tUdpVisualizationThreadFunc(void *cookie) {

    char buff[MAXLEN]; 
	int n, addr_length;
	/* Socket address structure */
	struct sockaddr_in socket_addr;
	/* Create socket */
	int my_socket = socket(PF_INET, SOCK_DGRAM, 0);

	if (my_socket == -1) {
		fprintf(stderr, "Cannot create socket\n");
		return 0;
	}

	/* Initialize socket address to 0*/
	memset(&socket_addr, 0, sizeof(socket_addr));
	/* Set socket address parameters */
	socket_addr.sin_family = AF_INET;
	socket_addr.sin_port = htons(1100);
	socket_addr.sin_addr.s_addr = INADDR_ANY;

	char *str = "{ \
        \"id\": \"flat\", \
		\"length\": 880, \
		\"width\": 880, \
		\"walls\": [ \
			{ \"start_x\": 100, \"start_y\": 120, \"end_x\": 100, \"end_y\": 180 }, \
			{ \"start_x\": 200, \"start_y\": 180, \"end_x\": 360, \"end_y\": 180 }, \
			{ \"start_x\": 600, \"start_y\": 600, \"end_x\": 800, \"end_y\": 600 } \
		], \
        \"doors\": [ \
			{ \"start_x\": 680, \"start_y\": 600, \"end_x\": 760, \"end_y\": 600 } \
		], \
        \"furnitures\": [ \
			{ \"furniture\": [ \
                { \"x\": 150, \"y\": 150, \"length\": 200, \"width\": 50 }, \
                { \"x\": 300, \"y\": 150, \"length\": 50, \"width\": 80 } \
                ] } \
		], \
        \"garbages\": [ \
			{ \"x\": 50, \"y\": 100 }, \
            { \"x\": 70, \"y\": 100 }, \
		] \
	}";

    jobjFlat = json_tokener_parse(str);

	pthread_mutex_lock(&flatUdpMutex);
    if (strcpy(buff, json_object_to_json_string(jobjFlat)) == NULL)
    {
        fprintf(stderr, "Cannot copy json string to buffer\n");
        return 0;
    }
	pthread_mutex_unlock(&flatUdpMutex);

    /* Send a message to server */
    sendto(my_socket, buff, strlen(buff), MSG_CONFIRM, (const struct sockaddr *) &socket_addr, sizeof(socket_addr)); 

	/* Clean up */
    close(my_socket); 

	return 0;
}
/******************************************************/