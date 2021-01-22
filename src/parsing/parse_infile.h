#pragma once

#include <string>
#include "container.h"
#include "simulation_domain.h"

void debug_print(std::string str);
bool parse_infile(std::string filename, SimulationDomain*& domain, ViewParams& view_ps);