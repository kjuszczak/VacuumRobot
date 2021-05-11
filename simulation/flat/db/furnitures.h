#ifndef FURNITURES_H
#define FURNITURES_H

#include "stdint.h"

typedef struct element
{
    uint16_t x;
    uint16_t y;
    uint16_t length;
    uint16_t width;
} elementStruct;

typedef struct furniture
{
    elementStruct* elements;
    uint16_t numOfElements;
} furnitureStruct;

typedef struct furnitures
{
    furnitureStruct* furnitures;
    uint16_t numOfFurnitures;
} furnituresStruct;

#endif // FURNITURES_H
