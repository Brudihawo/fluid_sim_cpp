#include <iostream>
#include <fstream>
#include <regex>
#include "simulation_domain.h"
#include "concentration_domain.h"
#include "container.h"


bool read_infile(std::string filename, SimulationDomain* domain) {
    std::string line;
    std::ifstream infile(filename);

    std::regex simtype_regex("simtype (\d)\s-*");
    std::regex nxny_regex("(NX|NY) = (\\d+)\s-*");
    std::regex delta_regex("DELTA = (\\d+\\.\\d+)");
    std::regex deltat_regex("DELTA_T = (\\d+\\.\\d+)");
    std::regex ts_regex("N_TIMESTEPS = (\\d+)");
    std::regex btype_regex("\\((periodic|constant_val|constant_grad),\\s*(periodic|constant_val|constant_grad)\\)");
    std::regex d_regex("D = (\\d+.\\d+)");

    std::regex square_regex("(square|ellipse)\\((\\d+)\\) (\\d+\\.\\d+), (\\d+), (\\d+), (\\d+), (\\d+)\\s*");
    std::smatch regex_match;


    SimType simtype = SimType::NONE;
    SimParams params = { -1, -1, -1.0, -1.0, -1, 0, {BoundaryType::INVALID, BoundaryType::INVALID} };
    std::vector<double> additional_params;
    bool complete_params = false;

    if (infile.is_open()) {
        while (std::getline(infile, line)) {
            // Get basic simulation info for creating domain object
            if (line[0] == (char)"#") {
                continue;
            }

            if (!complete_params) {
                // SIMTYPE
                if (std::regex_match(line, regex_match, simtype_regex)) {
                    if (regex_match[0] == "0") {
                        simtype = SimType::CONCENTRATION;
                    }
                }

                if (simtype == SimType::CONCENTRATION) {
                    if (std::regex_match(line, regex_match, d_regex)) {
                        if (additional_params.size() < 1)
                            additional_params.push_back((double)std::stoi(regex_match[0], nullptr));
                    }
                }

                // NX/NY
                if (std::regex_match(line, regex_match, nxny_regex)) {
                    if (regex_match[0] == "NX") {
                        params.NX = (long)std::stoi(regex_match[1], nullptr);
                    } else {
                        params.NY = (long)std::stoi(regex_match[1], nullptr);
                    }
                }

                // DELTA
                if (std::regex_match(line, regex_match, delta_regex)) {
                    params.DELTA = (double)std::stoi(regex_match[0], nullptr);
                }

                // DELTA_T
                if (std::regex_match(line, regex_match, deltat_regex)) {
                    params.DELTA_T = (double)std::stoi(regex_match[0], nullptr);
                }

                // N_TIMESTEPS
                if (std::regex_match(line, regex_match, ts_regex)) {
                    params.N_TIMESTEPS = (double)std::stoi(regex_match[0], nullptr);
                }

                // Boundary Type
                if (std::regex_match(line, regex_match, btype_regex)) {
                    for (int i = 0; i < 2; i++) {
                        if (regex_match[i] == "periodic") {
                            params.b_type[i] = BoundaryType::PERIODIC;
                        } else if (regex_match[i] == "constant_val") {
                            params.b_type[i] = BoundaryType::CONSTANT_VALUE;
                        } else if (regex_match[i] == "constant_grad") {
                            params.b_type[i] = BoundaryType::CONSTANT_GRADIENT;
                        } else {
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
                        domain = new SimulationDomain(params, additional_params);
                    }
                    complete_params = true;
                    break;
                }
            }
            //if (std::regex_match(line, regex_match, square_regex)) {
            //    std::cout << "Shape: " << regex_match[0] << std::endl;
            //    std::cout << "Field ID: " << regex_match[1] << std::endl;
            //    std::cout << "Value: " << regex_match[2] << std::endl;
            //    std::cout << "XMin: " << regex_match[3] << std::endl;
            //    std::cout << "XMax: " << regex_match[5] << std::endl;
            //    std::cout << "YMin: " << regex_match[4] << std::endl;
            //    std::cout << "YMax: " << regex_match[6] << std::endl;
            //    if (regex_match[0] == "square") { // SHAPE

            //    }
            //}
        }
        infile.close();
    }
    else {
        std::cout << "Error, Unable to open File" << std::endl;
        return false;
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "You need to pass an inputfile to start a simulation" << std::endl;
        return;
    }
    else {
        if (read_infile())
    }


}