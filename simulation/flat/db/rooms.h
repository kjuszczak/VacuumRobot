#include "stdint.h"
#include "walls.h"
#ifndef ROOMS_H
#define ROOMS_H

#include "doors.h"
#include "garbages.h"
#include "furnitures.h"

typedef struct room
{
    wallsStruct* walls;
    doorsStruct* doors;
    garbagesStruct* garbages;
    furnituresStruct* furnitures;
    uint16_t id;
} roomStruct;


typedef struct rooms
{
    roomStruct** rooms;
    uint16_t numOfRooms;
} roomsStruct;

uint8_t getCurrentRoomWhereRobotIs(const roomsStruct* rooms, uint16_t x, uint16_t y);

#endif
