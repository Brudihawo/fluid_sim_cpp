#include <iostream>

#include "glad/gl.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "implot.h"

#include "glfw_imgui_defs.cpp"
#include "container.h"
#include "incompressible_fluid_domain.h"
#include "concentration_domain.h"


// #DEFINE BOUNDARY_PERIODIC
static void sim(GLFWwindow* window, SimType sim_type, SimParams& params, ViewParams& view_ps, std::vector<double>& additional_params) {
    ConcentrationDomain domain(params, additional_params);
    bool restart_sim = false;

    // SIMULATION
    for (long t = 0; domain.timestep(t); t++) {
        if (t % view_ps.timeskip == 0) {
            display(window, t, domain.get_data(), restart_sim);
        }
        if (glfwWindowShouldClose(window) | restart_sim) break;
    }
}


int main() {
    SimParams sim_ps = {
        .NX = 100,
        .NY = 100,
        .DELTA = 1.0,
        .DELTA_T = 0.0001,
        .N_TIMESTEPS = 20000,
    };
    ViewParams view_ps = {
        .XRES = 1200,
        .YRES = 600,
        .timeskip = 100
    };

    SimType sim_type = SimType::NONE;

    // FIXME: Additional params hardcoded
    std::vector<double> additional_params = {1.0};

    GLFWwindow* window = glfw_imgui_init(view_ps);
    if (window == nullptr) return -1;

    while (!glfwWindowShouldClose(window)) {
        bool init_done = false;
        while (!init_done & !glfwWindowShouldClose(window)) {
            sim_init_window(window, sim_type, sim_ps, view_ps, init_done);
        }
        if (init_done) {
            sim(window, sim_type, sim_ps, view_ps, additional_params);
        }
    }

    ImPlot::DestroyContext();
    ImGui::DestroyContext();
}