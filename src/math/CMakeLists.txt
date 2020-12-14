add_library(math INTERFACE)

target_include_directories(math INTERFACE "${CMAKE_CURRENT_LIST_DIR}")
file(GLOB math_SRC "*.cpp")
target_sources(math INTERFACE "${math_SRC}")

