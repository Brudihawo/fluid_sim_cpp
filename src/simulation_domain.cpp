#include "simulation_domain.h"
#include <iostream>

SimulationDomain::SimulationDomain(SimParams& p) :NX(p.NX), NY(p.NY), DELTA(p.DELTA), DELTA_T(p.DELTA_T), N_SCALAR_FIELDS(p.N_SCALAR_FIELDS) {
    init();
    std::cout << "Simulation Domain Initialised" << std::endl;
}

void SimulationDomain::init() {
    fields = new double*[N_SCALAR_FIELDS];
    for (int n = 0; n < N_SCALAR_FIELDS; n++) {
        fields[n] = new double[NX * NY];
        for (int i = 0; i < NX * NY; i++) {
            fields[n][i] = 0.0; 
        }
    }
}

SimulationDomain::~SimulationDomain() {
    for (int n = 0; n < N_SCALAR_FIELDS; n++) {
        delete[] fields[n];
    }
    delete[] fields;
}

bool SimulationDomain::timestep(long t) {
    return false;
}

long SimulationDomain::idx(long x, long y) {
    return x + y * NX;
}

double SimulationDomain::ddx(double* s, long x, long y) {
    double sp, sm;
    if (x < NX - 1)
        sp = s[idx(x + 1, y)];
    else
        #ifdef BOUNDARY_PERIODIC
        #else
            return ddx(s, x - 1, y);
        #endif
    if (x > 0)
        sm = s[idx(x - 1, y)];
    else
        #ifdef BOUNDARY_PERIODIC
        #else
            return ddx(s, x + 1, y);
        #endif
    return (sp - sm) / DELTA; 
}

double SimulationDomain::ddy(double* s, long x, long y) {
    double sp, sm;
    if (y < NY - 1)
        sp = s[idx(x, y + 1)];
    else
        #ifdef BOUNDARY_PERIODIC
        #else
            return ddy(s, x, y - 1);
        #endif
    if (y > 0)
        sm = s[idx(x, y - 1)];
    else
        #ifdef BOUNDARY_PERIODIC
        #else
            return ddy(s, x, y + 1);
        #endif
    return (sp - sm) / DELTA;
}

double SimulationDomain::d2dx(double* s, long x, long y) {
    double sp, sm;
    if (x < NX - 2)
        sp = s[idx(x + 2, y)];
    else
        #ifdef BOUNDARY_PERIODIC
        #else
            return 0;
        #endif
    if (x > 1)
        sm = s[idx(x - 2, y)];
    else
        #ifdef BOUNDARY_PERIODIC
        #else
            return 0;
        #endif
    return (sp + sm - 2 * s[idx(x, y)]) / (DELTA * DELTA);
}

double SimulationDomain::d2dy(double* s, long x, long y) {
    double sp, sm;
    if (y < NY - 2)
        sp = s[idx(x, y + 2)];
    else
        #ifdef BOUNDARY_PERIODIC
        #else
            return 0;
        #endif
    if (y > 1)
        sm = s[idx(x, y - 2)];
    else
        #ifdef BOUNDARY_PERIODIC
        #else
            return 0;
        #endif
    return (sp + sm - 2 * s[idx(x, y)]) / (DELTA * DELTA);
}