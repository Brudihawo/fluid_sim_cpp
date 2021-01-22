add_library(parsing INTERFACE)

target_include_directories(parsing INTERFACE "${CMAKE_CURRENT_LIST_DIR}")
file(GLOB parsing_SRC "*.cpp")
target_sources(parsing INTERFACE "${parsing_SRC}")