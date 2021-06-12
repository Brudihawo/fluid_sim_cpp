#include "parse_infile.h"

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <regex>

#include "container.h"

// Domain Types
#include "simulation_domain.h"
#include "concentration_domain.h"
#include "phasefield_domain.h"
#include "fluid_incompressible_domain.h"

// Parsing information
#include "parse_concentration.h"
#include "parse_phasefield.h"
#include "parse_fluid_incompressible.h"

#define DEBUG
void debug_print(std::string str) {
#ifdef DEBUG
  std::cout << "DEBUG: " << str << std::endl;
#endif
}


// Basic domain properties
bool parse_base(std::ifstream& infile, SimParams& params, ViewParams& view_ps, SimType& simtype) {
  std::string line;

  // basic parameters
  std::regex simtype_regex("simtype\\s*=\\s*(\\d)\\s*");
  std::regex nxny_regex("(NX|NY)\\s*=\\s*(\\d+)\\s*");
  std::regex delta_regex("DELTA\\s*=\\s*(\\d+\\.\\d+)\\s*");
  std::regex deltat_regex("DELTA_T\\s*=\\s*(\\d+\\.\\d+)\\s*");
  std::regex ts_regex("N_TIMESTEPS\\s*=\\s*(\\d+)\\s*");
  std::regex btype_regex("b_type\\s*=\\s*\\((periodic|constant_val|constant_grad),\\s*(periodic|constant_val|constant_grad)\\)\\s*");

  // visualisation
  std::regex res_regex("res_(x|y)\\s*=\\s*(\\d+)\\s*");
  std::regex tskip_regex("timeskip\\s*=\\s*(\\d+)\\s*");

  std::smatch m;

  // go to beginning
  infile.clear();
  infile.seekg(0, std::ios::beg);
  while (std::getline(infile, line)) {
    std::cout << "Reading Line: " << line << std::endl;
    // Get basic simulation info for creating domain object
    if (line[0] == (char)"#"[0] || line.empty()) {
      continue;
    }

    // SIMTYPE
    // ADD N_SCALAR_FIELDS AND DOMAIN TYPE ID HERE IF YOU IMPLEMENT A NEW SIMULATION DOMAIN
    if (std::regex_match(line, m, simtype_regex)) {
      if (m[1] == "0") {
        simtype = SimType::CONCENTRATION;
        params.N_SCALAR_FIELDS = 1;
      }
      else if (m[1] == "1") {
        simtype = SimType::PHASEFIELD;
        params.N_SCALAR_FIELDS = 1;
      }
      else if (m[1] == "2") {
        simtype = SimType::FLUID_INCOMPRESSIBLE;
        params.N_SCALAR_FIELDS = 2;
      }
    }

    // NX/NY
    if (std::regex_match(line, m, nxny_regex)) {
      if (m[1] == "NX") {
        params.NX = std::stol(m[2], nullptr);
      }
      else {
        params.NY = std::stol(m[2], nullptr);
      }
    }

    // DELTA
    if (std::regex_match(line, m, delta_regex)) {
      params.DELTA = std::stod(m[1], nullptr);
    }

    // DELTA_T
    if (std::regex_match(line, m, deltat_regex)) {
      params.DELTA_T = std::stod(m[1], nullptr);
    }

    // N_TIMESTEPS
    if (std::regex_match(line, m, ts_regex)) {
      params.N_TIMESTEPS = std::stod(m[1], nullptr);
    }
    // Boundary Type
    if (std::regex_match(line, m, btype_regex)) {
      for (int i = 0; i < 2; i++) {
        if (m[i + 1] == "periodic") {
          params.b_type[i] = BoundaryType::PERIODIC;
        }
        else if (m[i + 1] == "constant_val") {
          params.b_type[i] = BoundaryType::CONSTANT_VALUE;
        }
        else if (m[i + 1] == "constant_grad") {
          params.b_type[i] = BoundaryType::CONSTANT_GRADIENT;
        }
        else {
          params.b_type[i] = BoundaryType::INVALID;
        }
      }
    }

    // Visualisation parameters
    if (std::regex_match(line, m, res_regex)) {
      if (m[1] == "x") {
        view_ps.XRES = std::stoi(m[2]);
        if (view_ps.XRES > 1920) view_ps.XRES = 1920;
        if (view_ps.XRES < 700) view_ps.XRES = 700;
      }
      else {
        view_ps.YRES = std::stoi(m[2]);
        if (view_ps.YRES > 1920) view_ps.YRES = 1920;
        if (view_ps.YRES < 700) view_ps.YRES = 700;
      }
    }

    if (std::regex_match(line, m, tskip_regex)) {
      view_ps.timeskip = std::stoi(m[1]);
      if (view_ps.timeskip < 0) {
        view_ps.timeskip = 100;
        debug_print("TIMESKIP TOO LOW, SET TO 100!");
      }
    }

    // Check for completeness
    if ((params.NX > -1) && (params.NY > -1) &&
      (params.DELTA > -1.0) && (params.DELTA_T > -1.0) &&
      (params.N_TIMESTEPS > 1) && (params.N_SCALAR_FIELDS > 0) &&
      (simtype > SimType::NONE) &&
      (params.b_type[0] > BoundaryType::INVALID) && (params.b_type[1] > BoundaryType::INVALID) &&
      (view_ps.XRES > -1) && (view_ps.YRES > -1) && (view_ps.timeskip > -1))
      return true;
  }
  return false;
}


// initial conditions
void parse_initials(SimulationDomain*& domain, std::ifstream& infile) {
  std::string line;

  std::regex shape_regex("(rect|ellipse)\\s*\\((\\d+)\\)\\s*([+-]{0,1}\\d+\\.\\d+),\\s*(\\d+),\\s*(\\d+),\\s*(\\d+),\\s*(\\d+)\\s*");
  std::regex smooth_regex("smooth\\((\\d+)\\s*,\\s*(\\d+)\\)\\s*");
  std::smatch m;

  while (std::getline(infile, line)) {
    if (std::regex_match(line, m, shape_regex)) {
      debug_print("Shape: " + m[1].str());
      debug_print("Field ID: " + m[2].str());
      debug_print("Value: " + m[3].str());
      debug_print("XMin: " + m[4].str());
      debug_print("XMax: " + m[6].str());
      debug_print("YMin: " + m[5].str());
      debug_print("YMax: " + m[7].str());

      if (m[1] == "rect") { // SHAPE
        domain->set_value_shape(Shape::RECTANGLE,
          std::stoi(m[2]),  // Field
          std::stol(m[4]),  // x0
          std::stol(m[6]),  // x1
          std::stol(m[5]),  // y0
          std::stol(m[7]),  // y1
          std::stod(m[3])); // value

      }
      else if (m[1] == "ellipse") {
        domain->set_value_shape(Shape::ELLIPSE,
          std::stoi(m[2]),  // Field
          std::stol(m[4]),  // x0
          std::stol(m[6]),  // x1
          std::stol(m[5]),  // y0
          std::stol(m[7]),  // y1
          std::stod(m[3])); // value
      }
    }

    if (std::regex_match(line, m, smooth_regex)) {
      int index = std::stoi(m[1]);
      int iterations = std::stoi(m[2]);
      if ((index < domain->get_data().N_SCALAR_FIELDS) &&
        (iterations > 0)) {
        domain->smooth_gaussian(index, iterations);
      }
    }
  }
}


// parse infile values and create simulation domain
bool parse_infile(std::string filename, SimulationDomain*& domain, ViewParams& view_ps) {
  std::string line;
  std::ifstream infile(filename);

  std::vector<double> additional_params;

  SimType simtype = SimType::NONE;
  SimParams params = { -1, -1, -1.0, -1.0, -1, 0, {BoundaryType::INVALID, BoundaryType::INVALID} };
  view_ps = { -1, -1, -1 };

  if (infile.is_open()) {
    if (!parse_base(infile, params, view_ps, simtype)) {
      infile.close();
      return false;
    }
    // Domain Specifics
    if (simtype == SimType::CONCENTRATION) {
      if (parse_concentration(infile, additional_params)) {
        std::cout << "Found all parameters for concentration domain!" << std::endl;
        std::cout << "Creating domain..." << std::endl;
        domain = new ConcentrationDomain(params, additional_params);
      }
      else {
        std::cerr << "Error parsing concentration domain values! Aborting..." << std::endl;
        infile.close();
        return false;
      }
    }
    else if (simtype == SimType::PHASEFIELD) {
      if (parse_phasefield(infile, additional_params)) {
        std::cout << "Found all parameters for phasefield domain!" << std::endl;
        std::cout << "Creating domain..." << std::endl;
        domain = new PhasefieldDomain(params, additional_params);
      }
      else {
        std::cerr << "Error parsing phasefield domain values! Aborting..." << std::endl;
        infile.close();
        return false;
      }
    }
    else if (simtype == SimType::FLUID_INCOMPRESSIBLE) {
      if (parse_fluid_incompressible(infile, additional_params)) {
        std::cout << "Found all parameters for phasefield domain!" << std::endl;
        std::cout << "Creating domain..." << std::endl;
        domain = new FluidIncompressibleDomain(params, additional_params);
      }
      else {
        std::cerr << "Error parsing phasefield domain values! Aborting..." << std::endl;
        infile.close();
        return false;
      }
    }
    // Insert your simulation domain type here
    else {
      infile.close();
      return false;
    }

    // Initial conditions
    parse_initials(domain, infile);

    infile.close();
    return true;
  }

  else {
    std::cout << "Error, Unable to open File" << std::endl;
    return false;
  }
}

