#include "concentration_domain.h"

// FIXME: Why is there a seg fault on init in line 8?
void ConcentrationDomain::init() {
    old = new double[NX * NY];
    c = &fields[0][0];
    for (int i = 0; i < NX * NY; i++) {
        c[i] = 0.0;
        old[i] = 0.0;
    }
    c[idx(0, 0)] = 1.0;
}

ConcentrationDomain::ConcentrationDomain(SimParams& p, double* d): SimulationDomain(p), D(d[0]) {
    init();
}

ConcentrationDomain::~ConcentrationDomain() {
    delete[] old;
}

bool ConcentrationDomain::timestep(long t) {
    for (int i = 0; i < NX * NY; i++) {
        old[i] = c[i];
    }
    for (int j = 0; j < NY; j++) {
        for (int i = 0; i < NX; i++) {
            c[idx(i, j)] = old[i] + D * (d2dx(c, i, j) + d2dy(c, i, j)) * DELTA_T;
        }
    }
    return t < N_TIMESTEPS;
}
