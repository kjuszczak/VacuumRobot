#include "creator.h"
#include "jsonCreator.h"

#include <stdio.h>
#include <pthread.h>

/* I ROOM */
wallStruct walls11[4] = {
    {{0, 0}, {0, 300}},
    {{0, 300}, {300, 300}},
    {{300, 300}, {300, 0}},
    {{0, 0}, {300, 0}}};
wallsStruct walls1 = {walls11, 4};

doorStruct door1 = {{110, 300}, {190,300}};
doorStruct* doors1Array[1] = {&door1};
doorsStruct doors1 = {doors1Array, 1};

pthread_mutex_t garbagesMutex1 = PTHREAD_MUTEX_INITIALIZER;
garbagesStruct garbages1 = {NULL, 0, &garbagesMutex1};

elementStruct element1[2] = {{200, 0, 50, 50}, {250, 0, 50, 70}};
furnituresStruct furnitures1 = {NULL, 0};

roomStruct room1 = {&walls1, &doors1, &garbages1, &furnitures1, 0};

/* II ROOM */
wallStruct walls21[4] = {
    {{300, 0}, {300, 300}},
    {{300, 300}, {880, 300}},
    {{880, 300}, {880, 0}},
    {{0, 0}, {880, 0}}};
wallsStruct walls2 = {walls21, 4};
doorStruct door2 = {{435, 300}, {515,300}};
doorStruct* doors2Array[1] = {&door2};
doorsStruct doors2 = {doors2Array, 1};
pthread_mutex_t garbagesMutex2 = PTHREAD_MUTEX_INITIALIZER;
garbagesStruct garbages2 = {NULL, 0, &garbagesMutex2};
furnituresStruct furnitures2 = {NULL, 0};
roomStruct room2 = {&walls2, &doors2, &garbages2, &furnitures2, 1};

/* III ROOM */
wallStruct walls31[4] = {
    {{0, 530}, {0, 880}},
    {{0, 880}, {200, 880}},
    {{200, 880}, {200, 530}},
    {{200, 530}, {0, 530}}};
wallsStruct walls3 = {walls31, 4};
doorStruct door3 = {{60, 530}, {140, 530}};
doorStruct* doors3Array[1] = {&door3};
doorsStruct doors3 = {doors3Array, 1};
pthread_mutex_t garbagesMutex3 = PTHREAD_MUTEX_INITIALIZER;
garbagesStruct garbages3 = {NULL, 0, &garbagesMutex3};
furnituresStruct furnitures3 = {NULL, 0};
roomStruct room3 = {&walls3, &doors3, &garbages3, &furnitures3, 2};

/* IV ROOM */
wallStruct walls41[4] = {
    {{200, 530}, {200, 880}},
    {{200, 880}, {400, 880}},
    {{400, 880}, {400, 530}},
    {{400, 530}, {200, 530}}};
wallsStruct walls4 = {walls41, 4};
doorStruct door4 = {{260, 530}, {340, 530}};
doorStruct* doors4Array[1] = {&door4};
doorsStruct doors4 = {doors4Array, 1};
pthread_mutex_t garbagesMutex4 = PTHREAD_MUTEX_INITIALIZER;
garbagesStruct garbages4 = {NULL, 0, &garbagesMutex4};
furnituresStruct furnitures4 = {NULL, 0};
roomStruct room4 = {&walls4, &doors4, &garbages4, &furnitures4, 3};

/* V ROOM */
wallStruct walls51[4] = {
    {{400, 530}, {400, 880}},
    {{400, 880}, {880, 880}},
    {{880, 880}, {880, 530}},
    {{880, 530}, {400, 530}}};
wallsStruct walls5 = {walls51, 4};
doorStruct door5 = {{485, 530}, {565, 530}};
doorStruct* doors5Array[1] = {&door5};
doorsStruct doors5 = {doors5Array, 1};
pthread_mutex_t garbagesMutex5 = PTHREAD_MUTEX_INITIALIZER;
garbagesStruct garbages5 = {NULL, 0, &garbagesMutex5};
furnituresStruct furnitures5 = {NULL, 0};
roomStruct room5 = {&walls5, &doors5, &garbages5, &furnitures5, 4};

/* VI ROOM */
wallStruct walls61[4] = {
    {{650, 300}, {650, 530}},
    {{650, 530}, {880, 530}},
    {{880, 530}, {880, 300}},
    {{880, 300}, {650, 300}}};
wallsStruct walls6 = {walls61, 4};
doorStruct door6 = {{650, 375}, {650, 455}};
doorStruct* doors6Array[1] = {&door6};
doorsStruct doors6 = {doors6Array, 1};
pthread_mutex_t garbagesMutex6 = PTHREAD_MUTEX_INITIALIZER;
garbagesStruct garbages6 = {NULL, 0, &garbagesMutex6};
furnituresStruct furnitures6 = {NULL, 0};
roomStruct room6 = {&walls6, &doors6, &garbages6, &furnitures6, 5};

/* VII ROOM */
wallStruct walls71[4] = {
    {{0, 300}, {0, 530}},
    {{0, 530}, {650, 530}},
    {{650, 530}, {650, 300}},
    {{650, 300}, {0, 300}}};
wallsStruct walls7 = {walls71, 4};
doorStruct* doors7Array[6] = {&door1, &door2, &door3, &door4, &door5, &door6};
doorsStruct doors7 = {doors7Array, 6};
pthread_mutex_t garbagesMutex7 = PTHREAD_MUTEX_INITIALIZER;
garbagesStruct garbages7 = {NULL, 0, &garbagesMutex7};
furnituresStruct furnitures7 = {NULL, 0};
roomStruct room7 = {&walls7, &doors7, &garbages7, &furnitures7, 6};

/************************************/
roomStruct* roomsArray[7] = {&room1, &room2, &room3, &room4, &room5, &room6, &room7};
roomsStruct roomsInFlat = {roomsArray, 7};

void createFlatJsonObj(json_object* jObj)
{
    createFlatJson(&roomsInFlat, jObj);
}

void createGarbageUpdateJson(json_object* jObj)
{
    createGarbageJson(&roomsInFlat, jObj);
}

roomsStruct* getFlatPtr()
{
    return &roomsInFlat;
}
