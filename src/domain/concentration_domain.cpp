#include "concentration_domain.h"
#include <omp.h>

ConcentrationDomain::ConcentrationDomain(SimParams& p, std::vector<double>& additional_params): SimulationDomain(p, 1), D(additional_params[0]), c(fields[0]), old_c(old[0]) {
  field_descriptors.push_back("Concentration");
  field_value_limits.push_back({ 0.0, 1.0 });
}

ConcentrationDomain::~ConcentrationDomain() {
}

bool ConcentrationDomain::timestep(long t) {
  old_c.swap(c);

#pragma omp parallel for collapse(2)
  for (int j = 0; j < NY; j++) {
    for (int i = 0; i < NX; i++) {
      c[idx(i, j)] = old_c[idx(i, j)] 
        + D * (d2dx(old_c, i, j) + d2dy(old_c, i, j)) * DELTA_T;
    }
  }
  return t < N_TIMESTEPS;
}
