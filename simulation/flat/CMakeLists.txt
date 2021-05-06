set(flat_files
        ./db/creator.c
        ./db/creator.h
        ./db/jsonCreator.c
        ./db/jsonCreator.h
        ./db/garbages.c
        )
add_library(flat ${flat_files})