#ifndef FLAT_CREATOR_H
#define FLAT_CREATOR_H

#include <json-c/json.h>

#include "rooms/rooms.h"

void createFlatJsonObj(json_object* jObj);
void createGarbageUpdateJson(json_object* jObj);
roomsStruct* getFlatPtr();

#endif // FLAT_CREATOR_H
