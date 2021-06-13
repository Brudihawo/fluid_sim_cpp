// Copyright 2021 Hawo Höfer
#include <string>
#include <regex>
#include <iostream>
#include <sstream>

#include "container.h"
#include "parse_infile.h"
#include "filewriter.h"

static void sim(SimulationDomain& domain, const ViewParams& view_ps, std::string fprefix) {
  long framecount = 0;
  std::cout << "Starting Simulation" << std::endl;
  for (int64_t t = 0; domain.timestep(t); t++) {
    if (t % view_ps.timeskip == 0) {
      std::ostringstream fname;
      fname << fprefix << "_" << t << ".vti";
      write_vtk(domain, fname.str());
      std::cout << "Wrote Frame " << framecount << " to File " << fname.str() << std::endl;
      framecount++;
    }
  }
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << "You need to pass an inputfile to start a simulation"
      << std::endl;
    return 0;
  } else {
    SimulationDomain* domain;
    ViewParams view_ps;
    debug_print("Reading file: " + std::string(argv[1]));
    parse_infile(std::string(argv[1]), domain, view_ps);
    debug_print("Infile Read!");

    sim(*domain, view_ps, std::string(argv[2]));
  }
}
