#include "creator.h"
#include "jsonCreator.h"

/* I ROOM */
wallStruct walls11[4] = {
    {{0, 0}, {0, 300}},
    {{0, 300}, {300, 300}},
    {{300, 300}, {300, 0}},
    {{0, 0}, {300, 0}}};
wallsStruct walls1 = {walls11, 4};

doorStruct door1 = {{110, 300}, {190,300}};
doorsStruct doors1 = {&door1, 1};

garbageStruct garbage1 = {150, 150};
garbagesStruct garbages1 = {&garbage1, 1};

elementStruct element1[2] = {{200, 0, 50, 50}, {250, 0, 50, 70}};
furnitureStruct furniture11 = {element1, 2};
furnituresStruct furnitures1 = {&furniture11, 1};

roomStruct room1 = {&walls1, &doors1, &garbages1, &furnitures1, 1};

/* II ROOM */
wallStruct walls21[4] = {
    {{300, 0}, {300, 300}},
    {{300, 300}, {880, 300}},
    {{880, 300}, {880, 0}},
    {{0, 0}, {880, 0}}};
wallsStruct walls2 = {walls21, 4};

doorsStruct doors2 = {NULL, 0};

garbagesStruct garbages2 = {NULL, 0};

furnituresStruct furnitures2 = {NULL, 0};

roomStruct room2 = {&walls2, &doors2, &garbages2, &furnitures2, 2};

/* II ROOM */
wallStruct walls31[4] = {
    {{300, 0}, {300, 300}},
    {{300, 300}, {880, 300}},
    {{880, 300}, {880, 0}},
    {{0, 0}, {880, 0}}};
wallsStruct walls3 = {walls31, 4};

doorsStruct doors3 = {NULL, 0};

garbagesStruct garbages3 = {NULL, 0};

furnituresStruct furnitures3 = {NULL, 0};

roomStruct room3 = {&walls3, &doors3, &garbages3, &furnitures3, 2};

/************************************/
roomStruct* roomsArray[2] = {&room1, &room2};
roomsStruct roomsInFlat = {roomsArray, 2};

void createJsonObj(json_object* jObj)
{
    createFlatJson(&roomsInFlat, jObj);
}