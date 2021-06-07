#include "socket.h"

#include "../../pscommon/logger/log.h"

int sendFlatJson(tSocketData *socketData)
{
    char buff[MAXLEN]; 
	int n, addr_length;
	/* Socket address structure */
	struct sockaddr_in socket_addr;
	/* Create socket */
	int my_socket = socket(PF_INET, SOCK_DGRAM, 0);

	if (my_socket == -1) {
		LG_WRN("Cannot create socket");
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
		LG_WRN("Cannot copy json string to buffer");
		return 0;
	}
	sendto(my_socket, buff, strlen(buff), MSG_CONFIRM, (const struct sockaddr *) &socket_addr, sizeof(socket_addr)); 

	/* Clean up */
    close(my_socket); 

	return 0;
}

int sendRobotJson(tSocketData *socketData)
{
    char buff[MAXLEN]; 
	int n, addr_length;
	/* Socket address structure */
	struct sockaddr_in socket_addr;
	/* Create socket */
	int my_socket = socket(PF_INET, SOCK_DGRAM, 0);

	if (my_socket == -1) {
		LG_WRN("Cannot create socket");
		return 0;
	}

	/* Initialize socket address to 0*/
	memset(&socket_addr, 0, sizeof(socket_addr));
	/* Set socket address parameters */
	socket_addr.sin_family = AF_INET;
	socket_addr.sin_port = htons(socketData->port);
	socket_addr.sin_addr.s_addr = INADDR_ANY;

	if (strcpy(buff, json_object_to_json_string(socketData->jRobotObj)) == NULL)
	{
		LG_WRN("Cannot copy json string to buffer");
		return 0;
	}

	/* Send a message to server */
	sendto(my_socket, buff, strlen(buff), MSG_CONFIRM, (const struct sockaddr *) &socket_addr, sizeof(socket_addr)); 

	/* Clean up */
    close(my_socket); 

	return 0;
}
