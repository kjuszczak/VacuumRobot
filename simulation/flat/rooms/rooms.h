#ifndef ROOMS_H
#define ROOMS_H

#include <stdint.h>

#include "walls/walls.h"
#include "doors/doors.h"
#include "garbages/garbages.h"
#include "furnitures/furnitures.h"

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
