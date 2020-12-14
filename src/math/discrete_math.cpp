#include "discrete_math.h"
#include <limits>

double ddx(double** s, long x, long y, long NX) {
    double sp, sm;
    if (x < NX - 1)
        sp = s[x + 1][y];
    else
        #ifdef BOUNDARY_PERIODIC
        #else
            sp = 0.0f;
        #endif
    if (x > 0)
        sm = s[x - 1][y];
    else
        #ifdef BOUNDARY_PERIODIC
        #else
            sm = 0.0f;
        #endif
    return (sp - sm) / 2.0f;
}

double ddy(double** s, long x, long y, long NY) {
    double sp, sm;
    if (y < NY - 1)
        sp = s[x][y + 1];
    else
        #ifdef BOUNDARY_PERIODIC
        #else
            sp = 0.0f;
        #endif
    if (y > 0)
        sm = s[x][y - 1];
    else
        #ifdef BOUNDARY_PERIODIC
        #else
            sm = 0.0f;
        #endif
    return (sp - sm) / 2.0f;
}

double d2dx(double** s, long x, long y, long NX) {
    double sp, sm;
    if (x < NX - 2)
        sp = s[x + 2][y];
    else
        #ifdef BOUNDARY_PERIODIC
        #else
            sp = 0.0f;
        #endif
    if (x > 1)
        sm = s[x - 2][y];
    else
        #ifdef BOUNDARY_PERIODIC
        #else
            sm = 0.0f;
        #endif
    return (sp + sm - 2 * s[x][y]) / 4.0f;
}

double d2dy(double** s, long x, long y, long NY) {
    double sp, sm;
    if (y < NY - 2)
        sp = s[x][y + 2];
    else
        #ifdef BOUNDARY_PERIODIC
        #else
            sp = 0.0f;
        #endif
    if (y > 1)
        sm = s[x][y - 2];
    else
        #ifdef BOUNDARY_PERIODIC
        #else
            sm = 0.0f;
        #endif
    return (sp + sm - 2 * s[x][y]) / 4.0f;
}


void smooth(int n_iterations, double** s, double** tmp, long NX, long NY) {
    for (int it = 0; it < n_iterations; it++) {
        for (long j = 1; j < NY - 1; j++) {
            for (long i = 1; i < NX - 1; i++) {
                tmp[i][j] = (1.0 * s[i - 1][j - 1] + 2.0 * s[i][j - 1] + 1.0 * s[i + 1][j - 1] +
                             2.0 * s[i - 1][j    ] + 4.0 * s[i][j    ] + 2.0 * s[i + 1][j    ] + 
                             1.0 * s[i - 1][j + 1] + 2.0 * s[i][j + 1] + 1.0 * s[i + 1][j + 1]) / 16.0;
            }
        }

        for (long j = 1; j < NY - 1; j++) {
            for (long i = 1; i < NX - 1; i++) {
                s[i][j] = tmp[i][j];
            }
        }
    }
}

void find_min_max(interval& iv, double* s, long NX, long NY) {
    iv = {.max = -std::numeric_limits<double>::max(),
          .min =  std::numeric_limits<double>::max()};
    for (long j = 0; j < NY; j++) {
        for (long i = 0; i < NX; i++) {
            if (iv.min > s[i + j * NX]) iv.min = s[i + j * NX];
            if (iv.max < s[i + j * NX]) iv.max = s[i + j * NX];
        }
    }
}
