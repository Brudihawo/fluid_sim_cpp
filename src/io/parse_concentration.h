#pragma once 

#include <fstream>
#include <vector>

bool parse_concentration(std::ifstream& infile, std::vector<double>& additional_params);