#include "jsonCreator.h"
#include <stdio.h>

void createWalls(json_object* jWalls, const wallStruct* wallObj)
{
    json_object *jWall = json_object_new_object();
    json_object *startX = json_object_new_int(wallObj->startXY[0]);
    json_object *startY = json_object_new_int(wallObj->startXY[1]);
    json_object *endX = json_object_new_int(wallObj->endXY[0]);
    json_object *endY = json_object_new_int(wallObj->endXY[1]);       
    json_object_object_add(jWall, "start_x", startX);
    json_object_object_add(jWall, "start_y", startY);
    json_object_object_add(jWall, "end_x", endX);
    json_object_object_add(jWall, "end_y", endY);
    json_object_array_add(jWalls, jWall);    
}

void createDoors(json_object* jDoors, const doorStruct* doorObj)
{
    json_object *jDoor = json_object_new_object();
    json_object *startX = json_object_new_int(doorObj->startXY[0]);
    json_object *startY = json_object_new_int(doorObj->startXY[1]);
    json_object *endX = json_object_new_int(doorObj->endXY[0]);
    json_object *endY = json_object_new_int(doorObj->endXY[1]);       
    json_object_object_add(jDoor, "start_x", startX);
    json_object_object_add(jDoor, "start_y", startY);
    json_object_object_add(jDoor, "end_x", endX);
    json_object_object_add(jDoor, "end_y", endY);
    json_object_array_add(jDoors, jDoor);    
}

void createFurnitures(json_object* jFurnitures, const furnitureStruct* furnitureObj)
{
    json_object *jFurniture = json_object_new_object();
    json_object *jFurnitureArray = json_object_new_array();
    for(size_t i = 0; i < furnitureObj->numOfElements; i++)
    {
        json_object *jElement = json_object_new_object();
        json_object *x = json_object_new_int(furnitureObj->elements[i].x);
        json_object *y = json_object_new_int(furnitureObj->elements[i].y);
        json_object *length = json_object_new_int(furnitureObj->elements[i].length);
        json_object *width = json_object_new_int(furnitureObj->elements[i].width);       
        json_object_object_add(jElement, "x", x);
        json_object_object_add(jElement, "y", y);
        json_object_object_add(jElement, "length", length);
        json_object_object_add(jElement, "width", width);
        json_object_array_add(jFurnitureArray, jElement);
    }
    json_object_object_add(jFurniture, "furniture", jFurnitureArray);
    json_object_array_add(jFurnitures, jFurniture);
}

void createGarbages(json_object* jGarbages, const garbageStruct* garbageObj)
{
    json_object *jGarbage = json_object_new_object();
    json_object *x = json_object_new_int(garbageObj->x);
    json_object *y = json_object_new_int(garbageObj->y);
    json_object_object_add(jGarbage, "x", x);
    json_object_object_add(jGarbage, "y", y);
    json_object_array_add(jGarbages, jGarbage);
}

void createFlatJson(const roomsStruct* roomsInFlat, json_object* jObj)
{
    json_object *id = json_object_new_string("flat");
    json_object_object_add(jObj, "id", id);

    json_object *flatSize = json_object_new_int(880);
    json_object_object_add(jObj, "length", flatSize);
    json_object_object_add(jObj, "width", flatSize);

    json_object *jWalls = json_object_new_array();
    json_object *jDoors = json_object_new_array();
    json_object *jFurnitures = json_object_new_array();
    json_object *jGarbages = json_object_new_array();

    for(size_t i = 0; i < roomsInFlat->numOfRooms; i++)
    {
        roomStruct* iRoom = roomsInFlat->rooms[i];
        for(size_t w = 0; w < iRoom->walls->numOfWalls; w++)
        {
            createWalls(jWalls, &iRoom->walls->walls[w]);
        }
        for(size_t d = 0; d < iRoom->doors->numOfDoors; d++)
        {
            createDoors(jDoors, iRoom->doors->doors[d]);
        }
        for(size_t f = 0; f < iRoom->furnitures->numOfFurnitures; f++)
        {
            createFurnitures(jFurnitures, &iRoom->furnitures->furnitures[f]);
        }
        for(size_t g = 0; g < iRoom->garbages->numOfGarbages; g++)
        {
            createGarbages(jGarbages, iRoom->garbages->garbages[g]);
        }
    }
    json_object_object_add(jObj, "walls", jWalls);
    json_object_object_add(jObj, "doors", jDoors);
    json_object_object_add(jObj, "furnitures", jFurnitures);
    json_object_object_add(jObj, "garbages", jGarbages);
}