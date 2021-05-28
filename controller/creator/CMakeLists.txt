set(creator_files
        ./creator.h
        ./creator.c
        ./controller.h
        ./controller.c
        ./encoder/encoder.h
        ./encoder/encoder.c
        )
add_library(creator ${creator_files})