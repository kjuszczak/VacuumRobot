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
    pthread_mutex_t* jsonMutex;
    json_object* jObj;
} tSocketData;

/*
 * Thread functions
 */
/* UDP communication with Visualization process */
void* tUdpThreadFunc(void *);
/******************************************************/
