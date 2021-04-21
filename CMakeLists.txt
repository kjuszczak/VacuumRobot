add_executable(sim_main ./simulation/src/main.c)
target_link_libraries(sim_main pthread)
target_link_libraries(sim_main json-c)