#include "concentration_domain.h"

void ConcentrationDomain::init() {
    for(int j = 0; j < NY; j++) {
        for (int i = 0; i < NX / 2; i++) {
            c[idx(i, j)] = 1.0;
        }
    }
}

ConcentrationDomain::ConcentrationDomain(SimParams& p, std::vector<double> additional_params): SimulationDomain(p, 1), c(fields[0]), old_c(old[0]) {
    D = additional_params[0];
    field_descriptors.push_back("Concentration");
    field_value_limits.push_back({ 0.0, 1.0 });
    init();
}

ConcentrationDomain::~ConcentrationDomain() {
}

bool ConcentrationDomain::timestep(long t) {
    for (int i = 0; i < NX * NY; i++) {
        old_c[i] = c[i];
    }
    for (int j = 0; j < NY; j++) {
        for (int i = 0; i < NX; i++) {
            c[idx(i, j)] = old_c[i] + D * (d2dx(c, i, j) + d2dy(c, i, j)) * DELTA_T;
        }
    }
    return t < N_TIMESTEPS;
}
