#include "incompressible_fluid_domain.h"
#include "container.h"
#include <random>

void IncompressibleFluidDomain::init() {
    double p_max = 2;
    for (int i = 0; i < NX; i++) {
        for (int j = 0; j < NY; j++) {
            p[idx(i, j)] = p_max - p_max / (NX * NX) * i * i; 
        }
    }
}
IncompressibleFluidDomain::IncompressibleFluidDomain(SimParams& p, std::vector<double>& extra_params): SimulationDomain(p, 3), NU(extra_params[0]), u(fields[0]), v(fields[1]), p(fields[2]) {
    init();
}

IncompressibleFluidDomain::~IncompressibleFluidDomain() {
}

bool IncompressibleFluidDomain::timestep(long t) {
// FIXME: USE https://de.wikipedia.org/wiki/Navier-Stokes-Gleichungen#Navier-Stokes-Gleichungen_für_kompressible_Fluide
//        AND https://de.wikipedia.org/wiki/Euler-Gleichungen_(Strömungsmechanik)
//        for calculation of new flow field :) Also, learn to read :P
    if (t < N_TIMESTEPS) {
        for (int j = 0; j < NY; j++) {
            for (int i = 0; i < NX; i++) {
                u[idx(i, j)] += DELTA_T * ( NU * (d2dx(u, i, j) + d2dy(u, i, j)) - u[idx(i, j)] * ddx(u, i, j) - v[idx(i, j)] * ddy(u, i, j) - ddx(p, i, j));
                v[idx(i, j)] += DELTA_T * ( NU * (d2dx(v, i, j) + d2dy(v, i, j)) - u[idx(i, j)] * ddx(v, i, j) - v[idx(i, j)] * ddy(v, i, j) - ddy(p, i, j));
            }
        }
    }
    return t < N_TIMESTEPS;
}
