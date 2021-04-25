#include "stdint.h"
#include "walls.h"
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

void createRooms(void);
void createRoom(void);