#include <json-c/json.h>

#include "../rooms/rooms.h"

void createFlatJson(const roomsStruct* roomsInFlat, json_object* jObj);
void createGarbageJson(const roomsStruct* roomsInFlat, json_object* jObj);