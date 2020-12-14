cmake_minimum_required(VERSION 3.19)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
set(CMAKE_BUILD_TYPE Debug)

set(TOP_LIB_DIR C:/Users/hawoh/workspace/libraries)

project(FluidSim2D)
add_executable(FluidSim2D "${CMAKE_CURRENT_LIST_DIR}/src/main.cpp")
add_subdirectory(src)
add_subdirectory(res)

# khr
set(khr_DIR "${TOP_LIB_DIR}/khr")
find_package(khr REQUIRED)
target_link_libraries(FluidSim2D khr)

# glad
set(glad_DIR "${TOP_LIB_DIR}/glad")
find_package(glad REQUIRED)
target_link_libraries(FluidSim2D glad)

# glfw
set(glfw3_DIR "${TOP_LIB_DIR}/glfw-3.3.2.bin.WIN64/")
find_package(glfw3 REQUIRED)
target_link_libraries(FluidSim2D glfw3)

# imgui
target_link_libraries(FluidSim2D imgui)

# implot
target_link_libraries(FluidSim2D implot)

# src
target_include_directories(FluidSim2D PUBLIC "${CMAKE_CURRENT_LIST_DIR}/src")
get_target_property(FluidSim2D_SOURCES FluidSim2D SOURCES)
message("${FluidSim2D_SOURCES}")

# math
target_link_libraries(FluidSim2D math)
