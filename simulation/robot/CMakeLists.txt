set(robot_files
        ./creator.h
        ./creator.c
        ./robot.h
        ./robot.c
        ./sensors/sensor.h
        ./sensors/sensor.c
        ./encoder/encoder.h
        ./encoder/encoder.c
        ./motors/motor.h
        ./motors/motor.c
        ./visualization/jsonCreator.c
        ./visualization/jsonCreator.h
        )
add_library(robot ${robot_files})