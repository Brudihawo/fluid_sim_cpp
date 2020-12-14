#include <iostream>
#include <limits>

#include "GLFW/glfw3.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "implot.h"

#include "discrete_math.h"
#include "container.h"

void glfw_error_callback(int error, const char* description) {
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

void imgui_render(GLFWwindow* window) {
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window);
}

void display(GLFWwindow *window, long timestep, double** u, double** v, long NX, long NY) {
    double values[2][NX * NY];

    for (long j = 0; j < NY; j++) {
        for (long i = 0; i < NX; i++) {
            values[0][i + j * NX] = u[i][j];
            values[1][i + j * NX] = v[i][j];
        }
    }
    
    interval iv[2];
    
    find_min_max(iv[0], values[0], NX, NY);
    find_min_max(iv[1], values[1], NX, NY);

    for (int i = 0; i < 2; i++) {
        if (iv[i].min == iv[i].max) {
            iv[i].min = 0.0;
            iv[i].max = 1.0;
        }
        if (iv[i].min == std::numeric_limits<double>::max())
            iv[i].min = 0.0;
        if (iv[i].max == -std::numeric_limits<double>::max())
            iv[i].max = 1.0;
    }

    glfwPollEvents();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("FluidSim2D");
    ImGui::Text("Timestep: %ld", timestep);
    ImGui::Text("Minimum u: %f; Maximum u: %f", iv[0].min, iv[0].max);
    ImGui::Text("Minimum v: %f; Maximum v: %f", iv[1].min, iv[1].max);

    static ImPlotColormap map = ImPlotColormap_Viridis;
    ImPlot::PushColormap(map);
    static ImPlotAxisFlags axes_flags = ImPlotAxisFlags_Lock | ImPlotAxisFlags_NoGridLines | ImPlotAxisFlags_NoTickMarks;

    if (ImPlot::BeginPlot("Velocity X", NULL, NULL, ImVec2(500, 500), ImPlotFlags_NoLegend, axes_flags, axes_flags)) { 
        ImPlot::PlotHeatmap("Velocity X", &values[0][0], NX, NY, iv[0].min, iv[0].max, NULL, ImPlotPoint(0, 0), ImPlotPoint(1, 1));
        ImPlot::EndPlot();
    }
    ImGui::SameLine();
    ImPlot::ShowColormapScale(iv[0].min, iv[0].max, 500);

    ImGui::SameLine();
    if (ImPlot::BeginPlot("Velocity Y", NULL, NULL, ImVec2(500, 500), ImPlotFlags_NoLegend, axes_flags, axes_flags)) { 
        ImPlot::PlotHeatmap("Velocity Y", &values[1][0], NX, NY, iv[1].min, iv[1].max, NULL, ImPlotPoint(0, 0), ImPlotPoint(1, 1));
        ImPlot::EndPlot();
    }
    ImGui::SameLine();
    ImPlot::ShowColormapScale(iv[1].min, iv[1].max, 500);

    ImPlot::PopColormap();
    ImGui::End();

    // RENDERING
    imgui_render(window);
    if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Escape))) {
        glfwSetWindowShouldClose(window, 1);    
    }
}

void sim_init_window(GLFWwindow* window, sim_params& params, bool& init_done) {
    static long sim_dim = 100;
    static long n_timesteps = 2000000;
    static double deltat = 0.001;
    static double v_init = 10.0;
    static double nu = 0.0001;
    static int timeskip = 100;

    glfwPollEvents();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("FluidSim2D - Initialisation");
    {
        ImGui::InputScalar("Simulation Domain Size", ImGuiDataType_S64, &sim_dim, NULL, NULL, "%d");
        ImGui::InputScalar("Timesteps", ImGuiDataType_S64, &n_timesteps, NULL, NULL, "%d");
        ImGui::InputDouble("Delta t", &deltat);
        ImGui::InputDouble("Initial Velocity X", &v_init);
        ImGui::InputDouble("Kinematic Viscosity", &nu);
        ImGui::InputInt("Timeskip", &timeskip, 1, 100);
    }
    ImGui::End();
    imgui_render(window);
    if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Space))) {
        init_done = true;
        params.NX = sim_dim;
        params.NY = sim_dim;
        params.N_TIMESTEPS = n_timesteps;
        params.DELTA_T = deltat;
        params.dist_center_init = 10;
        params.v_init = v_init;
        params.timeskip = timeskip;
    }
    if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Escape))) {
        glfwSetWindowShouldClose(window, 1);    
    }
}
