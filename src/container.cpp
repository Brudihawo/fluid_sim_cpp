#include "container.h"
#include "discrete_math.h"
#include <random>

simulation_domain::simulation_domain(long nx, long ny, double delta, long n_timesteps, double delta_t, int tskip, double nu) :NX(ny), NY(ny), DELTA(delta), DELTA_T(delta_t), NU(nu) {
    init();
}

simulation_domain::simulation_domain(sim_params& p) :NX(p.NX), NY(p.NY), DELTA(p.DELTA), DELTA_T(p.DELTA_T), NU(p.NU) {
    init();
}

void simulation_domain::init() {
    u = new double[NX * NY];
    v = new double[NX * NY];
    p = new double[NX * NY];

    // INIT AS ZERO
    for (int index = 0; index < NX * NY; index++) {
        u[index] = 0.0;
        v[index] = 0.0;
        p[index] = 0.0;
    }
    double p_max = 2;
    for (int i = 0; i < NX; i++) {
        for (int j = 0; j < NY; j++) {
            p[idx(i, j)] = p_max - p_max / (NX * NX) * i * i; 
        }
    }
}

simulation_domain::~simulation_domain() {
    delete[] u;
    delete[] v;
    delete[] p;
}

bool simulation_domain::timestep(long t) {
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

void simulation_domain::smooth(int n_iterations) {
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

void simulation_domain::add_jitter() {
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

long simulation_domain::idx(long x, long y) {
    return x + y * NX;
}

double simulation_domain::ddx(double* s, long x, long y) {
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

double simulation_domain::ddy(double* s, long x, long y) {
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

double simulation_domain::d2dx(double* s, long x, long y) {
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

double simulation_domain::d2dy(double* s, long x, long y) {
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