#include "incompressible_fluid_domain.h"
#include "container.h"
#include <random>

void IncompressibleFluidDomain::init() {
    // double p_max = 2;
    // for (int i = 0; i < NX; i++) {
    //     for (int j = 0; j < NY; j++) {
    //         p[idx(i, j)] = p_max - p_max / (NX * NX) * i * i; 
    //     }
    // }
}
IncompressibleFluidDomain::IncompressibleFluidDomain(SimParams& p, std::vector<double>& extra_params): SimulationDomain(p, 2), NU(extra_params[0]), u(fields[0]), v(fields[1]), old_u(old[0]), old_v(old[1]) {
    field_descriptors.push_back("Velocity in X");
    field_descriptors.push_back("Velocity in Y");
    init();
}

IncompressibleFluidDomain::~IncompressibleFluidDomain() {
}

bool IncompressibleFluidDomain::timestep(long t) {
        for (int i = 0; i < NX * NY; i++) {
            old_u[i] = u[i];
            old_v[i] = v[i];
        }
        for (int j = 0; j < NY; j++) {
            for (int i = 0; i < NX; i++) {
                u[idx(i, j)] += DELTA_T * ( NU * (d2dx(old_u, i, j) + d2dy(old_u, i, j)) - u[idx(i, j)] * ddx(old_u, i, j) - v[idx(i, j)] * ddy(old_u, i, j));
                v[idx(i, j)] += DELTA_T * ( NU * (d2dx(old_v, i, j) + d2dy(old_v, i, j)) - u[idx(i, j)] * ddx(old_v, i, j) - v[idx(i, j)] * ddy(old_v, i, j));
            }
        }

    return t < N_TIMESTEPS;
}
