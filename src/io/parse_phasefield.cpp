#include "parse_phasefield.h"

#include <regex>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>

bool parse_phasefield(std::ifstream& infile, std::vector<double>& additional_params) {
    std::string line;

    std::regex param_regex("(EPS|TAU|SIGMA|L|Tm|T)\\s*=\\s*(\\d+.\\d+)\\s*");
    std::smatch m;
    std::vector<bool> complete = { false, false , false, false, false, false };
    std::string descriptors[6] = { "EPS", "TAU", "SIGMA", "L", "Tm", "T" };
    additional_params.resize(6);


    // go to beginning
    infile.clear();
    infile.seekg(0, std::ios::beg);
    while (std::getline(infile, line)) {
        if (std::regex_match(line, m, param_regex)) {
            for (int i = 0; i < 6; i++) {
                if ((m[1] == descriptors[i]) && !complete[i]) {
                    additional_params[i] = std::stod(m[2]);
                    complete[i] = true;
                }
            }
        }
        if (all_true(complete)) return true;
    }

    std::cout << "Missing Declarations of domain parameters:" << std::endl;
    for (int i = 0; i < 6; i++) {
        if (!complete[i]) {
            std::cout << descriptors[i] << std::endl;
        }
    }
    return false;
}

bool all_true(std::vector<bool> complete) {
    bool done = true;

    for (bool item : complete) {
        done = done && item;
    }

    return done;
}