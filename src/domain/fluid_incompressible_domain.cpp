#include "fluid_incompressible_domain.h"
#include "container.h"
#include <random>
#include <limits>

#include "omp.h"

FluidIncompressibleDomain::FluidIncompressibleDomain(SimParams& p, std::vector<double>& extra_params): SimulationDomain(p, 2), NU(extra_params[0]), u(fields[0]), v(fields[1]), old_u(old[0]), old_v(old[1]) {
    field_descriptors.push_back("Velocity in X");
    field_descriptors.push_back("Velocity in Y");

    field_value_limits.push_back({ -HUGE_VAL, HUGE_VAL });
    field_value_limits.push_back({ -HUGE_VAL, HUGE_VAL });
}

FluidIncompressibleDomain::~FluidIncompressibleDomain() {
}

bool FluidIncompressibleDomain::timestep(long t) {
    old_u.swap(u);
    old_v.swap(v);

#pragma omp parallel for collapse(2)
    for (int j = 0; j < NY; j++) {
        for (int i = 0; i < NX; i++) {
            u[idx(i, j)] = old_u[idx(i, j)] + DELTA_T * ( NU * (d2dx(old_u, i, j) + d2dy(old_u, i, j)) - u[idx(i, j)] * ddx(old_u, i, j) - v[idx(i, j)] * ddy(old_u, i, j));
            v[idx(i, j)] = old_v[idx(i, j)] + DELTA_T * ( NU * (d2dx(old_v, i, j) + d2dy(old_v, i, j)) - u[idx(i, j)] * ddx(old_v, i, j) - v[idx(i, j)] * ddy(old_v, i, j));
        }
    }

    return t < N_TIMESTEPS;
}
