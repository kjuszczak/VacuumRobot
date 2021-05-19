include_directories(${CMAKE_CURRENT_SOURCE_DIR}/controller/creator)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/controller/creator)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/controller/creator)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/controller/io)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/controller/io)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/controller/io)

add_executable(robot_controller ${CMAKE_CURRENT_SOURCE_DIR}/controller/main.c)

target_link_libraries(robot_controller creator)
target_link_libraries(robot_controller io_controller)
target_link_libraries(robot_controller pthread rt)