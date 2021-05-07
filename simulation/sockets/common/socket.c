#include "socket.h"

/*
 *  Thread functions
 */
void* tUdpThreadFunc(void *cookie)
{
    tSocketData* socketData = (tSocketData*)cookie;
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
	socket_addr.sin_port = htons(socketData->port);
	socket_addr.sin_addr.s_addr = INADDR_ANY;

	if (strcpy(buff, json_object_to_json_string(socketData->jFlatObj)) == NULL)
	{
		fprintf(stderr, "Cannot copy json string to buffer\n");
		return 0;
	}
	sendto(my_socket, buff, strlen(buff), MSG_CONFIRM, (const struct sockaddr *) &socket_addr, sizeof(socket_addr)); 

	while(1)
	{
		pthread_mutex_lock(socketData->jsonMutex);
		if (strcpy(buff, json_object_to_json_string(socketData->jRobotObj)) == NULL)
		{
			fprintf(stderr, "Cannot copy json string to buffer\n");
			return 0;
		}
		pthread_mutex_unlock(socketData->jsonMutex);

		/* Send a message to server */
		sendto(my_socket, buff, strlen(buff), MSG_CONFIRM, (const struct sockaddr *) &socket_addr, sizeof(socket_addr)); 
	}
	/* Clean up */
    close(my_socket); 

	return 0;
}
/******************************************************/