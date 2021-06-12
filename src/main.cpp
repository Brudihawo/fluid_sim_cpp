#include <string>
#include <regex>
#include <iostream>
#include <fstream>

#include "container.h"
#include "parse_infile.h"
#include "filewriter.h"

static void sim(SimulationDomain*& domain, ViewParams& view_ps) {
  std::cout << "Starting Simulation" << std::endl;
  for (long t = 0; domain->timestep(t); t++) {
    if (t % view_ps.timeskip == 0) {
      // Write to file
    }
  } 
}


int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << "You need to pass an inputfile to start a simulation" << std::endl;
    return 0;
  } else {
    SimulationDomain* domain;
    ViewParams view_ps;
    debug_print("Reading file: " + std::string(argv[1]));
    parse_infile(std::string(argv[1]), domain, view_ps);
    debug_print("Infile Read!");

    sim(domain, view_ps);
  }
}
