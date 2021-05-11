#include "rooms.h"
#include <unistd.h>
#include <stdio.h>

uint8_t getCurrentRoomWhereRobotIs(const roomsStruct* rooms, uint16_t x, uint16_t y)
{
    if (rooms == NULL)
    {
        fprintf(stderr, "Rooms is NULL\n");
        return -1;
    }
    for (size_t i = 0; i < rooms->numOfRooms - 1; i++)
    {
        uint16_t leftX = rooms->rooms[i]->walls->walls[0].startXY[0];
        uint16_t rightX = rooms->rooms[i]->walls->walls[2].startXY[0];
        uint16_t topY = rooms->rooms[i]->walls->walls[4].startXY[1];
        uint16_t downY = rooms->rooms[i]->walls->walls[1].startXY[1];
        if (((x >= leftX) && (x <= rightX)) && ((y >= downY) && (y <= topY)))
        {
            return rooms->rooms[i]->id;
        }
    }
    fprintf(stderr, "There is no room with robot!\n");
    return -1;
}
