#pragma once 

#include <fstream>
#include <vector>

bool parse_fluid_incompressible(std::ifstream& infile, std::vector<double>& additional_params);