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

#include "incompressible_fluid_domain.h"
#include "concentration_domain.h"


#define DEBUG
void debugprint(std::string str) {
#ifdef DEBUG
    std::cout << "DEBUG: " << str << std::endl;
#endif
}

bool infile_to_domain(std::string filename, SimulationDomain*& domain, ViewParams& view_ps) {
    std::string line;
    std::ifstream infile(filename);

    // line regexes
    std::regex simtype_regex("simtype\\s*=\\s*(\\d)\\s*");
    std::regex nxny_regex("(NX|NY)\\s*=\\s*(\\d+)\\s*");
    std::regex delta_regex("DELTA\\s*=\\s*(\\d+\\.\\d+)\\s*");
    std::regex deltat_regex("DELTA_T\\s*=\\s*(\\d+\\.\\d+)\\s*");
    std::regex ts_regex("N_TIMESTEPS\\s*=\\s*(\\d+)\\s*");
    std::regex btype_regex("b_type\\s*=\\s*\\((periodic|constant_val|constant_grad),\\s*(periodic|constant_val|constant_grad)\\)\\s*");
    std::regex d_regex("D\\s*=\\s*(\\d+.\\d+)\\s*");

    std::regex square_regex("(rect|ellipse)\\((\\d+)\\) (\\d+\\.\\d+), (\\d+), (\\d+), (\\d+), (\\d+)\\s*");
    
    std::regex res_regex("res_(x|y)\\s*=\\s*(\\d+)\\s*");
    std::regex tskip_regex("timeskip\\s*=\\s*(\\d+)\\s*");

    std::regex smooth_regex("smooth\\((\\d+)\\s*,\\s*(\\d+)\\)\\s*");
    
    std::smatch regex_match;



    SimType simtype = SimType::NONE;
    SimParams params = { -1, -1, -1.0, -1.0, -1, 0, {BoundaryType::INVALID, BoundaryType::INVALID} };
    std::vector<double> additional_params;
    bool complete_params = false;

    view_ps = { -1, -1, -1 };

    if (infile.is_open()) {
        while (std::getline(infile, line)) {
            std::cout << "Reading Line: " << line << std::endl;
            // Get basic simulation info for creating domain object
            if (line[0] == (char)"#"[0] || line.empty()) {
                continue;
            }

            if (!complete_params) {
                // SIMTYPE
                if (std::regex_match(line, regex_match, simtype_regex)) {
                    if (regex_match[1] == "0") {
                        simtype = SimType::CONCENTRATION;
                        params.N_SCALAR_FIELDS = 1;
                    }
                }

                if (simtype == SimType::CONCENTRATION) {
                    if (std::regex_match(line, regex_match, d_regex)) {
                        if (additional_params.size() < 1)
                            additional_params.push_back(std::stod(regex_match[1], nullptr));

                    }
                }

                // NX/NY
                if (std::regex_match(line, regex_match, nxny_regex)) {
                    if (regex_match[1] == "NX") {
                        params.NX = std::stol(regex_match[2], nullptr);
                    }
                    else {
                        params.NY = std::stol(regex_match[2], nullptr);
                    }
                }

                // DELTA
                if (std::regex_match(line, regex_match, delta_regex)) {
                    params.DELTA = std::stod(regex_match[1], nullptr);
                }

                // DELTA_T
                if (std::regex_match(line, regex_match, deltat_regex)) {
                    params.DELTA_T = std::stod(regex_match[1], nullptr);
                }

                // N_TIMESTEPS
                if (std::regex_match(line, regex_match, ts_regex)) {
                    params.N_TIMESTEPS = std::stod(regex_match[1], nullptr);
                }
                // Boundary Type
                if (std::regex_match(line, regex_match, btype_regex)) {
                    for (int i = 0; i < 2; i++) {
                        if (regex_match[i + 1] == "periodic") {
                            params.b_type[i] = BoundaryType::PERIODIC;
                        }
                        else if (regex_match[i + 1] == "constant_val") {
                            params.b_type[i] = BoundaryType::CONSTANT_VALUE;
                        }
                        else if (regex_match[i + 1] == "constant_grad") {
                            params.b_type[i] = BoundaryType::CONSTANT_GRADIENT;
                        }
                        else {
                            params.b_type[i] = BoundaryType::INVALID;
                        }
                    }
                }

                if (std::regex_match(line, regex_match, res_regex)) {
                    if (regex_match[1] == "x") {
                        view_ps.XRES = std::stoi(regex_match[2]);
                        if (view_ps.XRES > 1920) view_ps.XRES = 1920;
                        if (view_ps.XRES < 700) view_ps.XRES = 700;
                    }
                    else {
                        view_ps.YRES = std::stoi(regex_match[2]);
                        if (view_ps.YRES > 1920) view_ps.YRES = 1920;
                        if (view_ps.YRES < 700) view_ps.YRES = 700;
                    }
                }

                if (std::regex_match(line, regex_match, tskip_regex)) {
                    view_ps.timeskip = std::stoi(regex_match[1]);
                    if (view_ps.timeskip < 0) {
                        view_ps.timeskip = 100;
                        debugprint("TIMESKIP TOO LOW, SET TO 100!");
                    }
                }

                if ((params.NX > -1) && (params.NY > -1) &&
                    (params.DELTA > -1.0) && (params.DELTA_T > -1.0) &&
                    (params.N_TIMESTEPS > 1) && (params.N_SCALAR_FIELDS > 0) &&
                    (simtype > SimType::NONE) &&
                    (params.b_type[0] > BoundaryType::INVALID) && (params.b_type[1] > BoundaryType::INVALID) &&
                    (view_ps.XRES > -1) && (view_ps.YRES > -1) && (view_ps.timeskip > -1)) {

                    infile.clear();
                    infile.seekg(0, std::ios::beg);
                    if (simtype == SimType::CONCENTRATION)
                        domain = new ConcentrationDomain(params, additional_params);
                    complete_params = true;
                    break;
                }
            }
        }

        while (std::getline(infile, line)) {
            if (std::regex_match(line, regex_match, square_regex)) {
                debugprint("Shape: " + regex_match[1].str());
                debugprint("Field ID: " + regex_match[2].str());
                debugprint("Value: " + regex_match[3].str());
                debugprint("XMin: " + regex_match[4].str());
                debugprint("XMax: " + regex_match[6].str());
                debugprint("YMin: " + regex_match[5].str());
                debugprint("YMax: " + regex_match[7].str());

                if (regex_match[1] == "rect") { // SHAPE
                    domain->set_value_shape(Shape::RECTANGLE,
                        std::stoi(regex_match[2]),  // Field
                        std::stol(regex_match[4]),  // x0
                        std::stol(regex_match[6]),  // x1
                        std::stol(regex_match[5]),  // y0
                        std::stol(regex_match[7]),  // y1
                        std::stod(regex_match[3])); // value

                }
                else if (regex_match[1] == "ellipse") {
                    domain->set_value_shape(Shape::ELLIPSE,
                        std::stoi(regex_match[2]),  // Field
                        std::stol(regex_match[4]),  // x0
                        std::stol(regex_match[6]),  // x1
                        std::stol(regex_match[5]),  // y0
                        std::stol(regex_match[7]),  // y1
                        std::stod(regex_match[3])); // value
                }
            }
        }

        if (std::regex_match(line, regex_match, smooth_regex)) {
            int index = std::stoi(regex_match[1]);
            int iterations = std::stoi(regex_match[2]);
            if ((index < domain->get_data().N_SCALAR_FIELDS) &&
                (iterations > 0)) {
                domain->smooth_gaussian(index, iterations);
            }
        }
        infile.close();
        return true;
    }
    else {
        std::cout << "Error, Unable to open File" << std::endl;
        return false;
    }
}


static void sim(GLFWwindow* window, SimulationDomain& domain, ViewParams& view_ps) {
    for (long t = 0; domain.timestep(t); t++) {
        if (t % view_ps.timeskip == 0) {
            display(window, t, domain.get_data());
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
        debugprint("Reading file: " + std::string(argv[1]));
        infile_to_domain(argv[1], domain, view_ps);
        debugprint("Infile Read!");

        GLFWwindow* window = glfw_imgui_init(view_ps);
        if (window == nullptr) return -1;

        while (!glfwWindowShouldClose(window)) {
            sim(window, *domain, view_ps);
        }

        ImPlot::DestroyContext();
        ImGui::DestroyContext();
    }
}