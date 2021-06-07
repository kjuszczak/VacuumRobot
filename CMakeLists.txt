include(${CMAKE_CURRENT_SOURCE_DIR}/simulation/CMakeLists.txt)
include(${CMAKE_CURRENT_SOURCE_DIR}/controller/CMakeLists.txt)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/pscommon/logger)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/pscommon/logger)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/pscommon/logger)

add_executable(vacuum_robot main.c)
target_link_libraries(vacuum_robot logger)
target_link_libraries(vacuum_robot pthread rt)