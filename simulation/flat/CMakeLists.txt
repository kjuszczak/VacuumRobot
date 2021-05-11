set(flat_files
        ./db/creator.c
        ./db/creator.h
        ./db/jsonCreator.c
        ./db/jsonCreator.h
        ./db/garbages.c
        ./db/walls.c
        ./db/doors.c
        ./db/rooms.h
        ./db/rooms.c
        )
add_library(flat ${flat_files})