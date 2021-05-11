#include "garbages.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

void allocateGarbage(garbagesStruct* garbages, uint16_t x, uint16_t y, uint16_t weight)
{
    if (garbages == NULL)
    {
        return;
    }
    garbageStruct* newGarbage = malloc(1 * sizeof(garbageStruct));
    if (!newGarbage) 
    {
        fprintf(stderr, "Cannot allocate.\n");
    }
    newGarbage->x = x;
    newGarbage->y = y;
    newGarbage->weight = weight;
    pthread_mutex_lock(garbages->garbagesMutex);
    uint16_t newNumOfGarbages = garbages->numOfGarbages + 1;
    garbageStruct** tmpGarbages = malloc(newNumOfGarbages * sizeof(garbageStruct*));
    if (!tmpGarbages)
    {
        fprintf(stderr, "Cannot allocate.\n");
    }
    garbages->numOfGarbages++;
    for (size_t i = 0; i < garbages->numOfGarbages - 1; i++)
    {
        tmpGarbages[i] = garbages->garbages[i];
    }
    free(garbages->garbages);
    garbages->garbages = tmpGarbages;
    garbages->garbages[garbages->numOfGarbages - 1] = newGarbage;
    pthread_mutex_unlock(garbages->garbagesMutex);
}

void deleteGarbage(garbagesStruct* garbages, uint16_t x, uint16_t y)
{
    if (garbages == NULL || garbages->numOfGarbages == 0)
    {
        return;
    }
    pthread_mutex_lock(garbages->garbagesMutex);
    size_t indexToDelete = garbages->numOfGarbages;
    for (size_t i = 0; i < garbages->numOfGarbages; i++)
    {
        if (garbages->garbages[i]->x == x && garbages->garbages[i]->y == y)
        {
            indexToDelete = i;
            break;
        }     
    }
    if (indexToDelete == garbages->numOfGarbages)
    {
        return;
    }

    garbageStruct** tmpGarbages = malloc((garbages->numOfGarbages - 1) * sizeof(garbageStruct*));
    if (!tmpGarbages) 
    {
        fprintf(stderr, "Cannot delete.\n");
    }

    free(garbages->garbages[indexToDelete]);
    for (size_t i = 0; i < garbages->numOfGarbages; i++)
    {
        if (i < indexToDelete)
        {
            tmpGarbages[i] = garbages->garbages[i];
        }
        else if (i == indexToDelete)
        {
            continue;
        }
        else
        {
            tmpGarbages[i-1] = garbages->garbages[i];
        }
    }
    garbages->numOfGarbages--;
    free(garbages->garbages);
    garbages->garbages = tmpGarbages;
    pthread_mutex_unlock(garbages->garbagesMutex);
}

void deleteGarbageIfVacuumed(garbagesStruct* garbages, uint16_t x, uint16_t y)
{
    pthread_mutex_lock(garbages->garbagesMutex);
    for (size_t i = 0; i < garbages->numOfGarbages; i++)
    {
        if ((garbages->garbages[i]->x == x) && (garbages->garbages[i]->x == y))
        {
            deleteGarbage(garbages, x, y);
        }
    }
    pthread_mutex_unlock(garbages->garbagesMutex);
}
