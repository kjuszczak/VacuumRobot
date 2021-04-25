include_directories(sockets)
link_directories(sockets)
add_subdirectory(sockets)

include_directories(flat)
link_directories(flat)
add_subdirectory(flat)

add_executable(sim_main main.c)

target_link_libraries(sim_main sockets)
target_link_libraries(sim_main flat)

target_link_libraries(sim_main pthread)
target_link_libraries(sim_main json-c)