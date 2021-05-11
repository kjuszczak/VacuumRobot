set(robot_files
        ./creator.h
        ./creator.c
        ./robot.h
        ./robot.c
        ./sensors/sensor.h
        ./sensors/sensor.c
        ./visualization/jsonCreator.c
        ./visualization/jsonCreator.h
        )
add_library(robot ${robot_files})