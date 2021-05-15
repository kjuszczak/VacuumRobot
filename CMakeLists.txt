include(${CMAKE_CURRENT_SOURCE_DIR}/simulation/CMakeLists.txt)
include(${CMAKE_CURRENT_SOURCE_DIR}/controller/CMakeLists.txt)

add_executable(vacuum_robot main.c)