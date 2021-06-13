add_library(io INTERFACE)

target_include_directories(io INTERFACE "${CMAKE_CURRENT_LIST_DIR}")
target_include_directories(io INTERFACE "${CMAKE_CURRENT_LIST_DIR}/../domain")

file(GLOB io_SRC "*.cpp")
target_sources(io INTERFACE "${io_SRC}")




