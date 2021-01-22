add_library(domain INTERFACE)

target_include_directories(domain INTERFACE "${CMAKE_CURRENT_LIST_DIR}")
file(GLOB domain_SRC "*.cpp")
target_sources(domain INTERFACE "${domain_SRC}")
