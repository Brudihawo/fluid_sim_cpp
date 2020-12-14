file(GLOB imgui_SRC_FILES "*.cpp")

add_library(imgui INTERFACE)
target_include_directories(imgui INTERFACE "${CMAKE_CURRENT_LIST_DIR}")
file(GLOB imgui_SRC "*.cpp")
target_sources(imgui INTERFACE "${imgui_SRC}")
