#include "incompressible_fluid_domain.h"
#include "container.h"
#include <random>

void IncompressibleFluidDomain::init() {
    // Initialise scalar fields
    fields = new double*[N_SCALAR_FIELDS];

    for (int i = 0; i < N_SCALAR_FIELDS; i++) {
        fields[i] = new double[NX * NY];
    }

    u = fields[0];
    v = fields[1];
    p = fields[2];

    // INIT AS ZERO
    for(int i = 0; i < N_SCALAR_FIELDS; i++) {
        for (int index = 0; index < NX * NY; index++) {
            fields[i][index] = 0.0;
        }
    }

    double p_max = 2;
    for (int i = 0; i < NX; i++) {
        for (int j = 0; j < NY; j++) {
            p[idx(i, j)] = p_max - p_max / (NX * NX) * i * i; 
        }
    }
}
IncompressibleFluidDomain::IncompressibleFluidDomain(SimParams& p, double* extra_params): SimulationDomain(p), NU(extra_params[0]) {
    init();
}

IncompressibleFluidDomain::~IncompressibleFluidDomain() {
    for (int i = 0; i < N_SCALAR_FIELDS; i++) {
        delete[] fields[i];
    }
    delete[] fields;
}

void IncompressibleFluidDomain::smooth(int n_iterations) {
    double tmp[NX * NY];
    double* fields[3] = {u, v, p};

    for (double* s: fields) {
        for (int it = 0; it < n_iterations; it++) {
            for (long j = 1; j < NY - 1; j++) {
                for (long i = 1; i < NX - 1; i++) {
                    tmp[idx(i, j)] = (1.0 * s[idx(i - 1, j - 1)] + 2.0 * s[idx(i, j - 1)] + 1.0 * s[idx(i + 1, j - 1)] +
                                    2.0 * s[idx(i - 1, j    )] + 4.0 * s[idx(i, j    )] + 2.0 * s[idx(i + 1, j    )] + 
                                    1.0 * s[idx(i - 1, j + 1)] + 2.0 * s[idx(i, j + 1)] + 1.0 * s[idx(i + 1, j + 1)]) / 16.0;
                }
            }

            for (int j = 1; j < NY - 1; j++) {
                tmp[idx(0, j)] = (2.0 * s[idx(0, j - 1)] + 1.0 * s[idx(1, j - 1)] +
                                4.0 * s[idx(0, j    )] + 2.0 * s[idx(1, j    )] + 
                                2.0 * s[idx(0, j + 1)] + 1.0 * s[idx(1, j + 1)]) / 12.0;

                tmp[idx(NY - 1, j)] = (1.0 * s[idx(NY - 2, j - 1)] + 2.0 * s[idx(NY - 1, j - 1)] +
                                       2.0 * s[idx(NY - 2, j    )] + 4.0 * s[idx(NY - 1, j    )] + 
                                       1.0 * s[idx(NY - 2, j + 1)] + 2.0 * s[idx(NY - 1, j + 1)]) / 12.0;
            }
            
            for(int i = 1; i < NX - 1; i++){
                tmp[idx(i, 0)] = (2.0 * s[idx(i - 1, 0)] + 4.0 * s[idx(i, 0)] + 2.0 * s[idx(i + 1, 0)] + 
                                1.0 * s[idx(i - 1, 1)] + 2.0 * s[idx(i, 1)] + 1.0 * s[idx(i + 1, 1)]) / 12.0;

                tmp[idx(i, NY - 1)] = (1.0 * s[idx(i - 1, NX - 2)] + 2.0 * s[idx(i, NX - 2)] + 1.0 * s[idx(i + 1, NX - 2)] +
                                    2.0 * s[idx(i - 1, NX - 1)] + 4.0 * s[idx(i, NX - 1)] + 2.0 * s[idx(i + 1, NX - 1)]) / 12.0;
            }

            tmp[idx(0, 0)] = (4.0 * s[idx(0, 0)] + 2.0 * s[idx(1, 0)] +
                            2.0 * s[idx(0, 1)] + 1.0 * s[idx(1, 1)]) / 9.0;

            tmp[idx(NX - 1, 0)] = (4.0 * s[idx(NX - 1, 0)] + 2.0 * s[idx(NX - 1, 1)] +
                                2.0 * s[idx(NX - 2, 0)] + 1.0 * s[idx(NX - 2, 1)]) / 9.0;

            tmp[idx(0, NY - 1)] = (4.0 * s[idx(0, NY - 1)] + 2.0 * s[idx(0, NY - 2)] +
                                2.0 * s[idx(1, NY - 1)] + 1.0 * s[idx(1, NY - 2)]) / 9.0;
                                
            tmp[idx(NX - 1, NY - 1)] = (4.0 * s[idx(NX - 1, NY - 1)] + 2.0 * s[idx(NX - 1, NY - 2)] +
                                    2.0 * s[idx(NX - 2, NY - 1)] + 1.0 * s[idx(NX - 2, NY - 2)]) / 7.0;

            for (long j = 0; j < NY; j++) {
                for (long i = 0; i < NX; i++) {
                    s[idx(i, j)] = tmp[idx(i, j)];
                }
            }
        }
    }
}

void IncompressibleFluidDomain::add_jitter() {
    std::random_device r;
    std::seed_seq seeds{r(), r(), r(), r(), r(), r(), r(), r()};
    std::normal_distribution<double> dist(0.0, 1);
    std::mt19937 en(seeds);    
    for (long index = 0; index < NX * NY; index++) {
        // u[index] += dist(en);
        // v[index] += dist(en);
        p[index] += dist(en);
    }
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
