set(io_files
        ./output/output.h
        ./output/output.c
        ./input/input.h
        ./input/input.c
        ../creator/encoder/encoder.h
        ../creator/encoder/encoder.c
        )
add_library(io_controller ${io_files})