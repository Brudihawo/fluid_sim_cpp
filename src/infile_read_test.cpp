#include <iostream>
#include <fstream>
#include <regex>
#include "simulation_domain.h"
#include "concentration_domain.h"
#include "container.h"

#define DEBUG

void debugprint(std::string str) {
#ifdef DEBUG
    std::cout << "DEBUG: " << str << std::endl;
#endif
}

bool read_infile(std::string filename, SimulationDomain*& domain) {
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
    std::smatch regex_match;


    SimType simtype = SimType::NONE;
    SimParams params = { -1, -1, -1.0, -1.0, -1, 0, {BoundaryType::INVALID, BoundaryType::INVALID} };
    std::vector<double> additional_params;
    bool complete_params = false;

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
                if ((params.NX > -1) && (params.NY > -1) &&
                    (params.DELTA > -1.0) && (params.DELTA_T > -1.0) &&
                    (params.N_TIMESTEPS > 1) && (params.N_SCALAR_FIELDS > 0) &&
                    (simtype > SimType::NONE) &&
                    (params.b_type[0] > BoundaryType::INVALID) && (params.b_type[1] > BoundaryType::INVALID)) {

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
                                            std::stol(regex_match[3])); // value

                } else if (regex_match[1] == "ellipse") {
                    domain->set_value_shape(Shape::ELLIPSE,           
                                            std::stoi(regex_match[2]),  // Field
                                            std::stol(regex_match[4]),  // x0
                                            std::stol(regex_match[6]),  // x1
                                            std::stol(regex_match[5]),  // y0
                                            std::stol(regex_match[7]),  // y1
                                            std::stol(regex_match[3])); // value
                }
            }
        }
        infile.close();
    } else {
        std::cout << "Error, Unable to open File" << std::endl;
        return false;
    }
}
// TODO: Implement Infile Parsing in main
// TODO: Add live visualisation and timeskip parsing 
int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "You need to pass an inputfile to start a simulation" << std::endl;
        return 0;
    }
    else {
        SimulationDomain* domain;
        std::cout << "Reading file: " << argv[1] << std::endl;
        read_infile(argv[1], domain);
        std::cout << "Infile Read!" << std::endl;
    }
    return 1;
}