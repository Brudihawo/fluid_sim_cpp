cmake_minimum_required(VERSION 3.19)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
set(CMAKE_BUILD_TYPE Debug)

set(TOP_LIB_DIR C:/Users/hawoh/workspace/libraries)

project(Sim2D)
add_executable(Sim2D "${CMAKE_CURRENT_LIST_DIR}/src/main.cpp")
add_subdirectory(src)
add_subdirectory(res)

# khr
set(khr_DIR "${TOP_LIB_DIR}/khr")
find_package(khr REQUIRED)
target_link_libraries(Sim2D khr)

# glad
set(glad_DIR "${TOP_LIB_DIR}/glad")
find_package(glad REQUIRED)
target_link_libraries(Sim2D glad)

# glfw
set(glfw3_DIR "${TOP_LIB_DIR}/glfw-3.3.2.bin.WIN64/")
find_package(glfw3 REQUIRED)
target_link_libraries(Sim2D glfw3)

# imgui
target_link_libraries(Sim2D imgui)

# implot
target_link_libraries(Sim2D implot)

# src
target_include_directories(Sim2D PUBLIC "${CMAKE_CURRENT_LIST_DIR}/src")
target_sources(Sim2D PUBLIC "${CMAKE_CURRENT_LIST_DIR}/src/simulation_domain.cpp"
                            "${CMAKE_CURRENT_LIST_DIR}/src/concentration_domain.cpp"
                            "${CMAKE_CURRENT_LIST_DIR}/src/incompressible_fluid_domain.cpp")
get_target_property(Sim2D_SOURCES Sim2D SOURCES)
message("${Sim2D_SOURCES}")

# math
target_link_libraries(Sim2D math)
