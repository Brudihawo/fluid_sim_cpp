#include "glad/gl.h"
#include "glfw_imgui_defs.h"


#include <iostream>
#include <limits>
#include <vector>
#include <string>

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

GLFWwindow* glfw_imgui_init(ViewParams view_ps) {
        // glfw3 initialisation
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return nullptr;

    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    GLFWwindow *window = glfwCreateWindow(view_ps.XRES, view_ps.YRES, "Sim2D", NULL, NULL);
    if (window == NULL)
        return nullptr;
    
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (gladLoadGL(glfwGetProcAddress) == 0) {
        fprintf(stderr, "Failed to initialise OpenGL loader!\n");
        return nullptr;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    return window;
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

void display(GLFWwindow *window, long timestep, DomainData const& d) {    
    std::vector<std::pair<double, double>> iv;
    iv.reserve(d.N_SCALAR_FIELDS);
    
    for (int n = 0; n < d.N_SCALAR_FIELDS; n++) {
        iv.push_back({0.0, 0.0});
        find_min_max(iv[n], d.fields[n], d.NX * d.NY);
        if (iv[n].first == iv[n].second) {
            iv[n].first = 0.0;
            iv[n].second = 1.0;
        }
        if (iv[n].first == -std::numeric_limits<double>::max())
            iv[n].first = 0.0;
        if (iv[n].second == std::numeric_limits<double>::max())
            iv[n].second = 1.0;
    }

    glfwPollEvents();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    if (d.N_SCALAR_FIELDS > 1) 
        ImGui::SetNextWindowSize(ImVec2(1200, 700));
    else 
        ImGui::SetNextWindowSize(ImVec2(600, 700));
    ImGui::Begin("SIM DATA");
    ImGui::Text("Simulation Timestep: %ld", timestep);

    static ImPlotColormap map = ImPlotColormap_Viridis;
    ImPlot::PushColormap(map);
    static ImPlotAxisFlags axes_flags = ImPlotAxisFlags_Lock | ImPlotAxisFlags_NoGridLines, ImPlotAxesFlags_NoTickMarks;
    for (int n = 0; n < d.N_SCALAR_FIELDS; n++) {
        if (ImPlot::BeginPlot(d.field_descriptors[n].c_str(), NULL, NULL, ImVec2(500, 500), ImPlotFlags_NoLegend, axes_flags, axes_flags)) {
            ImPlot::PlotHeatmap("DATA", d.fields[n].data(),d.NX, d.NY, iv[n].first, iv[0].second, NULL, ImPlotPoint(0, 0), ImPlotPoint(1, 1));
            ImPlot::EndPlot();
        }
        
        ImGui::SameLine();
        ImPlot::ShowColormapScale(iv[n].first, iv[n].second, 500);
        if (!(n % 2)) {
            ImGui::SameLine();
        }
    }
    
    ImPlot::PopColormap();
    ImGui::End();

    // RENDERING
    imgui_render(window);
    if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Escape))) {
        glfwSetWindowShouldClose(window, 1);    
    }
}

// DEPRECATED. USE INFILE INSTEAD
// void sim_init_window(GLFWwindow* window, bool& init_done, SimType& sim_type, SimParams& sim_ps, ViewParams& view_ps, std::vector<double>& additional_values) {
//    long sim_dim = sim_ps.NX;
//    double delta = sim_ps.DELTA;
//    double delta_t = sim_ps.DELTA_T;
//    long n_timesteps = sim_ps.N_TIMESTEPS;
//    int n_scalar_fields = sim_ps.N_SCALAR_FIELDS;
//    int timeskip = view_ps.timeskip;
//
//    glfwPollEvents();
//    ImGui_ImplOpenGL3_NewFrame();
//    ImGui_ImplGlfw_NewFrame();
//    ImGui::NewFrame();
//
//    // TODO: Improve GUI
//    ImGui::SetNextWindowSize(ImVec2(500, 500), ImGuiCond_Once);
//    ImGui::Begin("Sim2D - Initialisation");
//    {
//        ImGui::InputScalar("Simulation Domain Size", ImGuiDataType_S64, &sim_dim, NULL, NULL, "%d");
//        ImGui::InputDouble("Delta", &delta);
//        ImGui::InputDouble("Delta t", &delta_t);
//        ImGui::InputScalar("Timesteps", ImGuiDataType_S64, &n_timesteps, NULL, NULL, "%d");
//
//        ImGui::InputInt("Timeskip", &timeskip, 1, 100);
//
//        static const char* current_item = NULL;
//        if (ImGui::BeginCombo("Simulation Type", current_item)) {
//            const char* items[] = {"Concentration", "Incompressible Fluid" };
//            for (const char* item: items) {
//                bool is_selected = (current_item == item);
//                if (ImGui::Selectable(item, is_selected))
//                    current_item = item;
//                if (is_selected)
//                    ImGui::SetItemDefaultFocus();
//            }
//            ImGui::EndCombo();
//        }
//        if (current_item) {
//            std::string item_str = current_item;
//            if (item_str == "Concentration") {
//                if (ImGui::BeginChild("ConcDomainOpts", ImVec2(500, 30))) {
//                    double d = 1.0;
//                    if (additional_values.size() > 0)
//                        d = additional_values[0];
//                    ImGui::InputDouble("Diffusion Coefficient", &d, 0.0001, 0.001);
//                    if (additional_values.size() < 1) {
//                        additional_values.push_back(d);
//                    }
//                    else {
//                        additional_values[0] = d;
//                    }
//                    sim_type = SimType::CONCENTRATION;
//                    sim_ps.N_SCALAR_FIELDS = 1;
//                    ImGui::EndChild();
//                }
//            }
//            else if (item_str == "Incompressible Fluid") {
//                if (ImGui::BeginChild("IncFlDomainOpts", ImVec2(500, 30))) {
//                    double nu = 1.0;
//                    if (additional_values.size() > 0)
//                        nu = additional_values[0];
//                    ImGui::InputDouble("Kinematic Viscosity", &nu, 0.0001, 0.001);
//                    if (additional_values.size() < 1) {
//                        additional_values.push_back(nu);
//                    }
//                    else {
//                        additional_values[0] = nu;
//                    }
//                    sim_ps.N_SCALAR_FIELDS = 2;
//                    sim_type = SimType::FLUID_INCOMPRESSIBLE;
//                    ImGui::EndChild();
//                }
//            }
//        }
//    }
//    if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Space))
//        || ImGui::Button("Begin")) {
//        init_done = true;
//    }
//
//    if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Escape))) {
//        glfwSetWindowShouldClose(window, 1);    
//    }
//    
//    ImGui::End();
//    imgui_render(window);
//
//    sim_ps.NX = sim_dim;
//    sim_ps.NY = sim_dim;
//    sim_ps.N_TIMESTEPS = n_timesteps;
//    sim_ps.DELTA = delta;
//    sim_ps.DELTA_T = delta_t;
//
//    view_ps.timeskip = timeskip;
//}
