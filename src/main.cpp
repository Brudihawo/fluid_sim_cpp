#include <iostream>

#include "glad/gl.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "implot.h"

#include "imgui_defs.cpp"

#include "discrete_math.h"


// #DEFINE BOUNDARY_PERIODIC

struct sim_params {
    long NX;
    long NY;
    double DELTA_T;
    long N_TIMESTEPS;
    double NU;
    double v_init;
    int dist_center_init;
};

void timestep(double DELTA_T, double** u, double** v, long NX, long NY, double NU) {
    for (int j = 0; j < NY; j++) {
        for (int i = 0; i < NX; i++) {
            u[i][j] += DELTA_T * ( NU * (d2dx(u, i, j, NX) + d2dy(u, i, j, NY)) - u[i][j] * ddx(u, i, j, NX) - v[i][j] * ddy(u, i, j, NY));
            v[i][j] += DELTA_T * ( NU * (d2dx(v, i, j, NX) + d2dy(v, i, j, NY)) - u[i][j] * ddx(v, i, j, NX) - v[i][j] * ddy(v, i, j, NY));
        }
    }
}

static void sim(GLFWwindow* window, sim_params& params) {
    long NX = params.NX;
    long NY = params.NY;
    long N_TIMESTEPS = params.N_TIMESTEPS;
    double DELTAT = params.DELTA_T;
    double NU = params.NU;
    double v_init = params.v_init;
    int dist_center_init = params.dist_center_init;

    double** u = new double*[NX];
    double** v = new double*[NX];
    double** tmp = new double*[NX];

    for (int i = 0; i < NX; i++) {
        u[i] = new double[NY];
        v[i] = new double[NY];
        tmp[i] = new double[NY];
    }

    // INIT AS ZERO
    for (int j = 0; j < NY; j++) {
        for (int i = 0; i < NX; i++) {
            u[i][j] = 0.0;
            v[i][j] = 0.0;
            tmp[i][j] = 0.0;
        }
    }

    // SET INITIAL VELOCITY IN CENTER
    for (int j = NY / 2 - dist_center_init; j < NY / 2 + dist_center_init; j++) {
        for (int i = NX / 2 - dist_center_init; i < NX / 2 + dist_center_init; i++) {
            u[i][j] = v_init;
        }
    }

    // TODO: Handle Boundary Conditions, dont just skip the boundary cells
    // GAUSSIAN SMOOTH VELOCITIES
    int n_smooth_iter = 5;
    smooth(n_smooth_iter, u, tmp, NX, NY);
    smooth(n_smooth_iter, v, tmp, NX, NY);

    // SIMULATION
    for (long t = 0; t < N_TIMESTEPS; t++) {
        timestep(DELTAT, u, v, NX, NY, NU);
        display(window, t, u, v, NX, NY);
        if (glfwWindowShouldClose(window)) break;
    }

    // CLEANUP
    for (int i = 0; i < NX; i++) {
        delete[] u[i];
        delete[] v[i];
        delete[] tmp[i];
    }
    delete[] u;
    delete[] v;
    delete[] tmp;
}


int main() {
    sim_params params = {
        .NX = 100,
        .NY = 100,
        .DELTA_T = 0.0001,
        .N_TIMESTEPS = 200000,
        .NU = 0.0000001,
        .v_init = 10.0,
        .dist_center_init = 5
    };
    
    int XRES = 1100;
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
    
    sim(window, params);

    ImPlot::DestroyContext();
    ImGui::DestroyContext();
}