include_directories(sockets)
link_directories(sockets)
add_subdirectory(sockets)

include_directories(flat)
link_directories(flat)
add_subdirectory(flat)

include_directories(robot)
link_directories(robot)
add_subdirectory(robot)

include_directories(io)
link_directories(io)
add_subdirectory(io)

add_executable(sim_main main.c)

target_link_libraries(sim_main robot)
target_link_libraries(sim_main sockets)
target_link_libraries(sim_main flat)
target_link_libraries(sim_main io)

target_link_libraries(sim_main pthread rt)
target_link_libraries(sim_main json-c m)