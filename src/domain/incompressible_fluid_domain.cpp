#include "incompressible_fluid_domain.h"
#include "container.h"
#include <random>

void IncompressibleFluidDomain::init() {
    for (int i = int(NX / 2 - 20); i < int(NX / 2 + 20); i++) {
        for (int j = int(NX / 2 - 20); j < int(NY / 2 + 20); j++) {
            u[idx(i, j)] = 1;
        }
    }
    smooth_gaussian(0, 5);
    smooth_gaussian(1, 5);
}
IncompressibleFluidDomain::IncompressibleFluidDomain(SimParams& p, std::vector<double>& extra_params): SimulationDomain(p, 2), NU(extra_params[0]), u(fields[0]), v(fields[1]), old_u(old[0]), old_v(old[1]) {
    field_descriptors.push_back("Velocity in X");
    field_descriptors.push_back("Velocity in Y");
    init();
}

IncompressibleFluidDomain::~IncompressibleFluidDomain() {
}

bool IncompressibleFluidDomain::timestep(long t) {
    old_u.swap(u);
    old_v.swap(v);
    for (int j = 0; j < NY; j++) {
        for (int i = 0; i < NX; i++) {
            u[idx(i, j)] = old_u[idx(i, j)] + DELTA_T * ( NU * (d2dx(old_u, i, j) + d2dy(old_u, i, j)) - u[idx(i, j)] * ddx(old_u, i, j) - v[idx(i, j)] * ddy(old_u, i, j));
            v[idx(i, j)] = old_v[idx(i, j)] + DELTA_T * ( NU * (d2dx(old_v, i, j) + d2dy(old_v, i, j)) - u[idx(i, j)] * ddx(old_v, i, j) - v[idx(i, j)] * ddy(old_v, i, j));
        }
    }

    return t < N_TIMESTEPS;
}
