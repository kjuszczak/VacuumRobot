set(logger_files
        ./log.h
        ./log.c
        ./logThread.h
        ./logThread.c
        )
add_library(logger ${logger_files})