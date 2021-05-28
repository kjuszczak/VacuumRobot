#include "garbages.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

#include "../../pscommon/constants.h"

void createGarbagesInRoom(garbagesStruct* garbages, uint16_t leftTopX, uint16_t leftTopY, uint16_t rightDownX, uint16_t rightDownY)
{
    leftTopX += 20;
    leftTopY += 20;
    rightDownX -= 20;
    rightDownY -= 20; 

    srand(time(NULL));
    size_t limit = rand() % MAX_NUMBERS_OF_GARBAGES_IN_ROOM;
    for (size_t i = 0; i < limit; i++)
    {
        uint16_t x = (rand() % (rightDownX - leftTopX)) + leftTopX;
        uint16_t y = (rand() % (rightDownY - leftTopY)) + leftTopY;
        allocateGarbage(garbages, x, y, 1);
    }
}

void allocateGarbage(garbagesStruct* garbages, uint16_t x, uint16_t y, uint16_t weight)
{
    if (garbages == NULL)
    {
        return;
    }

    if (findGarbage(garbages, x, y, 0) != -1)
    {
        return;
    }

    if (garbages->numOfGarbages < MAX_NUMBERS_OF_GARBAGES_IN_ROOM)
    {
        pthread_mutex_lock(garbages->garbagesMutex);
        garbages->garbages[garbages->numOfGarbages].x = x;
        garbages->garbages[garbages->numOfGarbages].y = y;
        garbages->garbages[garbages->numOfGarbages].weight = weight;
        garbages->numOfGarbages++;
        pthread_mutex_unlock(garbages->garbagesMutex);
    }
}

void deleteGarbage(garbagesStruct* garbages, size_t indexToDelete)
{
    if (garbages == NULL || garbages->numOfGarbages == 0)
    {
        return;
    }
    pthread_mutex_lock(garbages->garbagesMutex);
    garbages->numOfGarbages--;
    for (size_t i = 0; i < garbages->numOfGarbages; i++)
    {
        if (i >= indexToDelete)
        {
            garbages->garbages[i] = garbages->garbages[i + 1];
        }
    }
    pthread_mutex_unlock(garbages->garbagesMutex);
}

void deleteGarbageIfVacuumed(garbagesStruct* garbages, uint16_t x, uint16_t y)
{
    size_t indexToDelete = findGarbage(garbages, x, y, 10);
    if (indexToDelete != -1)
    {
        deleteGarbage(garbages, indexToDelete);
    }
}

int findGarbage(garbagesStruct* garbages, uint16_t x, uint16_t y, uint16_t radius)
{
    int indexToDelete = -1;
    if (garbages->numOfGarbages == 0)
    {
        return -1;
    }

    for (int i = 0; i < garbages->numOfGarbages; i++)
    {
        uint16_t diffX = garbages->garbages[i].x < x ? (x - garbages->garbages[i].x) : (garbages->garbages[i].x - x);
        uint16_t diffY = garbages->garbages[i].y < y ? (y - garbages->garbages[i].y) : (garbages->garbages[i].y - y);
        if ((diffX <= radius) && (diffY <= radius))
        {
            indexToDelete = i;
            break;
        }
    }
    return indexToDelete;
}
