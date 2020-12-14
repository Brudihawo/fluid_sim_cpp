#include <iostream>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "implot.h"

#include "discrete_math.h"
#include <limits>

void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
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

    for (auto i: iv) {
        if (i.min == i.max) {
            i.min = 0.0;
            i.max = 0.0;
        }
        if (i.min == std::numeric_limits<double>::max())
            i.min = 0.0;
        if (i.max == -std::numeric_limits<double>::max())
            i.max = 1.0;
    }

    glfwPollEvents();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("FluidSim2D");
    ImGui::Text("Timestep: %f", timestep);
    ImGui::Text("Minimum u: %f; Maximum u: %f", iv[0].min, iv[0].max);
    ImGui::Text("Minimum v: %f; Maximum v: %f", iv[1].min, iv[1].max);

    static ImPlotColormap map = ImPlotColormap_Viridis;
    ImPlot::PushColormap(map);
    static ImPlotAxisFlags axes_flags = ImPlotAxisFlags_Lock | ImPlotAxisFlags_NoGridLines | ImPlotAxisFlags_NoTickMarks;

    if (ImPlot::BeginPlot("Heatmap", NULL, NULL, ImVec2(500, 500), ImPlotFlags_NoLegend, axes_flags, axes_flags)) { 
        ImPlot::PlotHeatmap("Velocity X", &values[0][0], NX, NY, iv[0].min, iv[0].max, NULL, ImPlotPoint(0, 0), ImPlotPoint(1, 1));
        // ImPlot::PlotHeatmap("Velocity Y", &values[1][0], NX, NY, iv[1].min, iv[1].max, NULL, ImPlotPoint(0, 0), ImPlotPoint(1, 1));
        // ImGui::SameLine();
        // ImPlot::ShowColormapScale(iv[1].min, iv[1].max, 500);
        ImPlot::EndPlot();
    }
    
    ImGui::SameLine();
    ImPlot::ShowColormapScale(iv[0].min, iv[0].max, 500);

    ImPlot::PopColormap();
    ImGui::End();

    // RENDERING
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window);
}