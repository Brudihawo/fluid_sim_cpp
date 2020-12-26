#pragma once

#include <iostream>
#include <limits>
#include <vector>
#include "imgui.h"
#include "GLFW/glfw3.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "implot.h"

#include "discrete_math.h"
#include "container.h"

void glfw_error_callback(int error, const char* description);
GLFWwindow* glfw_imgui_init(ViewParams view_ps);
void imgui_render(GLFWwindow* window);
void display(GLFWwindow *window, long timestep, const DomainData& d, bool& restart_sim);
void sim_init_window(GLFWwindow* window, bool& init_done, SimType& sim_type, SimParams& sim_ps, ViewParams& view_ps, std::vector<double>& additional_values);
