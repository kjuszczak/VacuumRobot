set(flat_files
        ./creator.c
        ./creator.h
        ./visualization/jsonCreator.c
        ./visualization/jsonCreator.h
        ./rooms/garbages/garbages.c
        ./rooms/walls/walls.c
        ./rooms/doors/doors.c
        ./rooms/rooms.h
        ./rooms/rooms.c
        )
add_library(flat ${flat_files})