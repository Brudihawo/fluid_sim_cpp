#pragma once

#include <string>
#include "simulation_domain.h"

void write_vtk(void* data, std::string filename, long frame, double time);

void vtk_header();

void vtk_end();

