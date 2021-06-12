cmake_minimum_required(VERSION 3.15)

project(Sim2D)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(Sim2D "${CMAKE_CURRENT_LIST_DIR}/src/main.cpp")

add_subdirectory(src)

# src
target_include_directories(Sim2D PUBLIC "${CMAKE_CURRENT_LIST_DIR}/src")

get_target_property(Sim2D_SOURCES Sim2D SOURCES)
message("${Sim2D_SOURCES}")

# math
target_link_libraries(Sim2D math)

# io
target_link_libraries(Sim2D io)

# domain
target_link_libraries(Sim2D domain)
