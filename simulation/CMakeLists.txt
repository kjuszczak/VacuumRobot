include_directories(${CMAKE_CURRENT_SOURCE_DIR}/simulation/sockets)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/simulation/sockets)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/simulation/sockets)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/simulation/flat)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/simulation/flat)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/simulation/flat)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/simulation/robot)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/simulation/robot)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/simulation/robot)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/simulation/io)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/simulation/io)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/simulation/io)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/pscommon/logger)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/pscommon/logger)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/pscommon/logger)

add_executable(robot_simulation ${CMAKE_CURRENT_SOURCE_DIR}/simulation/main.c)

target_link_libraries(robot_simulation robot)
target_link_libraries(robot_simulation sockets)
target_link_libraries(robot_simulation flat)
target_link_libraries(robot_simulation io)
target_link_libraries(robot_simulation logger)

target_link_libraries(robot_simulation pthread rt)
target_link_libraries(robot_simulation json-c m)