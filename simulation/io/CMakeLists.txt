set(io_files
        ../../pscommon/robotInput.h
        ./input/input.h
        ./input/input.c
        ./output/output.h
        ./output/output.c
        )
add_library(io ${io_files})