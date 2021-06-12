#pragma once 

#include <fstream>
#include <vector>

bool parse_phasefield(std::ifstream& infile, std::vector<double>& additional_params);
bool all_true(std::vector<bool> complete);