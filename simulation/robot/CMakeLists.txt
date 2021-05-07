set(robot_files
        ./robot.h
        ./creator.h
        ./creator.c
        ./visualization/jsonCreator.c
        ./visualization/jsonCreator.h
        )
add_library(robot ${robot_files})