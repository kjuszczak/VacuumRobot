#ifndef SOCKET_H
#define SOCKET_H

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

#define MAXLEN 4056

typedef struct socket
{
    /* data */
    uint16_t port;
    json_object* jFlatObj;
    json_object* jRobotObj;
} tSocketData;

/* UDP communication with Visualization process */
int sendFlatJson(tSocketData *socketData);
int sendRobotJson(tSocketData *socketData);
/******************************************************/

#endif // SOCKET_H
