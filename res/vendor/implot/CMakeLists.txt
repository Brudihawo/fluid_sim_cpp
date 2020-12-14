add_library(implot INTERFACE)
target_include_directories(implot INTERFACE "${CMAKE_CURRENT_LIST_DIR}")
file(GLOB implot_SOURCES "*.cpp")
target_sources(implot INTERFACE "${implot_SOURCES}")