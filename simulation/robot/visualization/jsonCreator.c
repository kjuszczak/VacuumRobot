#include "jsonCreator.h"

void createRobotJson(const robotStruct* robot, json_object* jObj)
{
    json_object *id = json_object_new_string("robot");
    json_object_object_add(jObj, "id", id);
    json_object *x = json_object_new_int(robot->x);
    json_object_object_add(jObj, "x", x);
    json_object *y = json_object_new_int(robot->y);
    json_object_object_add(jObj, "y", y);
    json_object *angle = json_object_new_int(robot->angle);
    json_object_object_add(jObj, "angle", angle);
}