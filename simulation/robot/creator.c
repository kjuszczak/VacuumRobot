#include "creator.h"
#include "visualization/jsonCreator.h"

robotStruct robot = {500, 150, -45};

void createRobotUpdateJson(json_object* jObj)
{
    createRobotJson(&robot, jObj);
}