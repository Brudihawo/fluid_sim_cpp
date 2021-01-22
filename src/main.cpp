#include <string>
#include <regex>
#include <iostream>
#include <fstream>

#include "glad/gl.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "implot.h"

#include "glfw_imgui_defs.h"
#include "container.h"

#include "parse_infile.h"


static void sim(GLFWwindow* window, SimulationDomain*& domain, ViewParams& view_ps) {
    for (long t = 0; domain->timestep(t); t++) {
        if (t % view_ps.timeskip == 0) {
            display(window, t, domain->get_data());
        }
        if (glfwWindowShouldClose(window)) break;
    }
}


int main(int argc, char* argv[]) {

    if (argc < 2) {
        std::cout << "You need to pass an inputfile to start a simulation" << std::endl;
        return 0;
    }
    else {
        SimulationDomain* domain;
        ViewParams view_ps;
        debug_print("Reading file: " + std::string(argv[1]));
        parse_infile(std::string(argv[1]), domain, view_ps);
        debug_print("Infile Read!");

        GLFWwindow* window = glfw_imgui_init(view_ps);
        if (window == nullptr) return -1;

        while (!glfwWindowShouldClose(window)) {
            sim(window, domain, view_ps);
        }

        ImPlot::DestroyContext();
        ImGui::DestroyContext();
    }
}