#include "simulation_domain.h"
#include <iostream>
#include <memory>
#include <random>


#define BOUNDARY_PERIODIC

SimulationDomain::SimulationDomain(SimParams& p, int n_scalar_fields) :NX(p.NX), NY(p.NY), DELTA(p.DELTA), DELTA_T(p.DELTA_T), N_SCALAR_FIELDS(n_scalar_fields), N_TIMESTEPS(p.N_TIMESTEPS) {
    init();
    std::cout << "Simulation Domain Initialised" << std::endl;
}

void SimulationDomain::init() {
    fields.reserve(N_SCALAR_FIELDS);
    old.reserve(N_SCALAR_FIELDS);

    for (int n = 0; n < N_SCALAR_FIELDS; n++) {
        fields.push_back(std::vector<double>(NX * NY));
        fields[n].reserve(NX * NY);
        old.push_back(std::vector<double>(NX * NY));
        old[n].reserve(NX * NY);
        for (int i = 0; i < NX * NY; i++) {
            fields[n].push_back(0.0);
            old[n].push_back(0.0);
        }
    }
}

SimulationDomain::~SimulationDomain() {
}

bool SimulationDomain::timestep(long t) {
    return false;
}

long SimulationDomain::idx(long x, long y) {
    return x + y * NX;
}

double SimulationDomain::ddx(std::vector<double>& s, long x, long y) {
    double sp, sm;
    if (x < NX - 1)
        sp = s[idx(x + 1, y)];
    else
        #ifdef BOUNDARY_PERIODIC
            sp = s[idx(0, y)];
        #else
            return ddx(s, x - 1, y);
        #endif
    if (x > 0)
        sm = s[idx(x - 1, y)];
    else
        #ifdef BOUNDARY_PERIODIC
            sm = s[idx(NX - 1, y)];
        #else
            return ddx(s, x + 1, y);
        #endif
    return (sp - sm) / DELTA; 
}

double SimulationDomain::ddy(std::vector<double>& s, long x, long y) {
    double sp, sm;
    if (y < NY - 1)
        sp = s[idx(x, y + 1)];
    else
        #ifdef BOUNDARY_PERIODIC
            sp = s[idx(x, 0)];
        #else
            return ddy(s, x, y - 1);
        #endif
    if (y > 0)
        sm = s[idx(x, y - 1)];
    else
        #ifdef BOUNDARY_PERIODIC
            sm = s[idx(x, NY - 1)];
        #else
            return ddy(s, x, y + 1);
        #endif
    return (sp - sm) / DELTA;
}

double SimulationDomain::d2dx(std::vector<double>& s, long x, long y) {
    double sp, sm;
    if (x < NX - 2)
        sp = s[idx(x + 2, y)];
    else
        #ifdef BOUNDARY_PERIODIC
            sp = s[idx(x - NX + 2, y)];
        #else
            return 0;
        #endif
    if (x > 1)
        sm = s[idx(x - 2, y)];
    else
        #ifdef BOUNDARY_PERIODIC
            sm = s[idx(NX - 2 + x, y)];
        #else
            return 0;
        #endif
    return (sp + sm - 2 * s[idx(x, y)]) / (DELTA * DELTA);
}

double SimulationDomain::d2dy(std::vector<double>& s, long x, long y) {
    double sp, sm;
    if (y < NY - 2)
        sp = s[idx(x, y + 2)];
    else
        #ifdef BOUNDARY_PERIODIC
            sp = s[idx(x, y - NY + 2)];
        #else
            return 0;
        #endif
    if (y > 1)
        sm = s[idx(x, y - 2)];
    else
        #ifdef BOUNDARY_PERIODIC
            sm = s[idx(x, NY - 2 + y)];
        #else
            return 0;
        #endif
    return (sp + sm - 2 * s[idx(x, y)]) / (DELTA * DELTA);
}
void SimulationDomain::add_noise(int field_index, double sigma) {
    std::random_device r;
    std::seed_seq seeds{r(), r(), r(), r(), r(), r(), r(), r()};
    std::normal_distribution<double> dist(0.0, sigma);
    std::mt19937 en(seeds);    
    for (long pos = 0; pos < NX * NY; pos++) {
        fields[field_index][pos] += dist(en);
    }
}

void SimulationDomain::smooth_gaussian(int field_index, int n_iterations) {
    std::vector<double>& s = fields[field_index];
    for (int it = 0; it < n_iterations; it++) {
        for (long j = 1; j < NY - 1; j++) {
            for (long i = 1; i < NX - 1; i++) {
                old[0][idx(i, j)] = 
                            (1.0 * s[idx(i - 1, j - 1)] + 2.0 * s[idx(i, j - 1)] + 1.0 * s[idx(i + 1, j - 1)] +
                            2.0 * s[idx(i - 1, j    )] + 4.0 * s[idx(i, j    )] + 2.0 * s[idx(i + 1, j    )] + 
                            1.0 * s[idx(i - 1, j + 1)] + 2.0 * s[idx(i, j + 1)] + 1.0 * s[idx(i + 1, j + 1)]) / 16.0;
            }
        }

        for (int j = 1; j < NY - 1; j++) {
            old[field_index][idx(0, j)] = 
                            (2.0 * s[idx(0, j - 1)] + 1.0 * s[idx(1, j - 1)] +
                            4.0 * s[idx(0, j    )] + 2.0 * s[idx(1, j    )] + 
                            2.0 * s[idx(0, j + 1)] + 1.0 * s[idx(1, j + 1)]) / 12.0;

            old[field_index][idx(NY - 1, j)] = 
                            (1.0 * s[idx(NY - 2, j - 1)] + 2.0 * s[idx(NY - 1, j - 1)] +
                            2.0 * s[idx(NY - 2, j    )] + 4.0 * s[idx(NY - 1, j    )] + 
                            1.0 * s[idx(NY - 2, j + 1)] + 2.0 * s[idx(NY - 1, j + 1)]) / 12.0;
        }
        
        for(int i = 1; i < NX - 1; i++){
            old[field_index][idx(i, 0)] = (2.0 * s[idx(i - 1, 0)] + 4.0 * s[idx(i, 0)] + 2.0 * s[idx(i + 1, 0)] + 
                            1.0 * s[idx(i - 1, 1)] + 2.0 * s[idx(i, 1)] + 1.0 * s[idx(i + 1, 1)]) / 12.0;

            old[field_index][idx(i, NY - 1)] = (1.0 * s[idx(i - 1, NX - 2)] + 2.0 * s[idx(i, NX - 2)] + 1.0 * s[idx(i + 1, NX - 2)] +
                                2.0 * s[idx(i - 1, NX - 1)] + 4.0 * s[idx(i, NX - 1)] + 2.0 * s[idx(i + 1, NX - 1)]) / 12.0;
        }

        old[field_index][idx(0, 0)] = (4.0 * s[idx(0, 0)] + 2.0 * s[idx(1, 0)] +
                        2.0 * s[idx(0, 1)] + 1.0 * s[idx(1, 1)]) / 9.0;

        old[field_index][idx(NX - 1, 0)] = (4.0 * s[idx(NX - 1, 0)] + 2.0 * s[idx(NX - 1, 1)] +
                            2.0 * s[idx(NX - 2, 0)] + 1.0 * s[idx(NX - 2, 1)]) / 9.0;

        old[field_index][idx(0, NY - 1)] = (4.0 * s[idx(0, NY - 1)] + 2.0 * s[idx(0, NY - 2)] +
                            2.0 * s[idx(1, NY - 1)] + 1.0 * s[idx(1, NY - 2)]) / 9.0;
                            
        old[field_index][idx(NX - 1, NY - 1)] = (4.0 * s[idx(NX - 1, NY - 1)] + 2.0 * s[idx(NX - 1, NY - 2)] +
                                2.0 * s[idx(NX - 2, NY - 1)] + 1.0 * s[idx(NX - 2, NY - 2)]) / 7.0;

        for (long j = 0; j < NY; j++) {
            for (long i = 0; i < NX; i++) {
                s[idx(i, j)] = old[field_index][idx(i, j)];
            }
        }
    }
}