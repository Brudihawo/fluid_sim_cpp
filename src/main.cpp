#include <iostream>

#include "glad/gl.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "implot.h"

#include "imgui_defs.cpp"
#include "container.h"


// #DEFINE BOUNDARY_PERIODIC
static void sim(GLFWwindow* window, sim_params& params) {
    simulation_domain domain(params);
    double v_init = params.v_init;
    int dist_center_init = params.dist_center_init;
    int timeskip = params.timeskip;
    bool restart_sim = false;

    // GAUSSIAN SMOOTH VELOCITIES
    domain.smooth(1);

    // SIMULATION
    for (long t = 0; domain.timestep(t); t++) {
        if (t % timeskip == 0) {
            display(window, t, domain.get_data(), restart_sim);
        }
        if (glfwWindowShouldClose(window) | restart_sim) break;
    }
}


int main() {
    sim_params params = {
        .NX = 100,
        .NY = 100,
        .DELTA_T = 0.0001,
        .N_TIMESTEPS = 20000,
        .v_init = 1.0,
        .dist_center_init = 10,
        .timeskip = 100
    };
    
    int XRES = 1200;
    int YRES = 600;

    // glfw3 initialisation
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    GLFWwindow *window = glfwCreateWindow(XRES, YRES, "FluidSim2D", NULL, NULL);
    if (window == NULL)
        return 1;
    
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (gladLoadGL(glfwGetProcAddress) == 0) {
        fprintf(stderr, "Failed to initialise OpenGL loader!\n");
        return 1;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGuiIO &io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    while (!glfwWindowShouldClose(window)) {
        bool init_done = false;
        while (!init_done & !glfwWindowShouldClose(window)) {
            sim_init_window(window, params, init_done);
        }
        
        if (init_done) {
            sim(window, params);
        }
    }

    ImPlot::DestroyContext();
    ImGui::DestroyContext();
}