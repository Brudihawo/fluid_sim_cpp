#include "simulation_domain.h"
#include <iostream>
#include <fstream>
#include <memory>
#include <random>
#include <regex>
#include "container.h"

SimulationDomain::SimulationDomain(SimParams& p, int n_scalar_fields) 
    :NX(p.NX), NY(p.NY),
    DELTA(p.DELTA), DELTA_T(p.DELTA_T),
    N_SCALAR_FIELDS(n_scalar_fields), N_TIMESTEPS(p.N_TIMESTEPS) {
    b_type[0] = p.b_type[0];
    b_type[1] = p.b_type[1];
    init();
}

void SimulationDomain::init() {
    fields.reserve(N_SCALAR_FIELDS);
    old.reserve(N_SCALAR_FIELDS);

    for (int n = 0; n < N_SCALAR_FIELDS; n++) {
        fields.push_back(std::vector<double>((double)NX * (double)NY));
        old.push_back(std::vector<double>((double)NX * (double)NY));
        std::fill(fields[n].begin(), fields[n].end(), 0.0);
        std::fill(old[n].begin(), old[n].end(), 0.0);
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

// Spatial derivative in respect to x
double SimulationDomain::ddx(std::vector<double>& s, long x, long y) {
    double sp, sm;
    sp = s[idx(x, y, +1, 0)];
    sm = s[idx(x, y, -1, 0)];
    return (sp - sm) / (2 * DELTA); 
}

// Spatial derivative in respect to y
double SimulationDomain::ddy(std::vector<double>& s, long x, long y) {
    double sp, sm;
    sp = s[idx(x, y, 0, +1)];
    sm = s[idx(x, y, 0, -1)];
    return (sp - sm) / ( 2 * DELTA);
}

// Second spatial derivative in respect to x
double SimulationDomain::d2dx(std::vector<double>& s, long x, long y) {
    double sp, sm;
    sp = s[idx(x, y, +2, 0)];
    sm = s[idx(x, y, -2, 0)];
    return (sp + sm - 2 * s[idx(x, y)]) / (4 * DELTA * DELTA);
}

// Second spatial derivative in respect to y
double SimulationDomain::d2dy(std::vector<double>& s, long x, long y) {
    double sp, sm;
    sp = s[idx(x, y, 0, +2)];
    sm = s[idx(x, y, 0, -2)];
    return (sp + sm - 2 * s[idx(x, y)]) / (4 * DELTA * DELTA);
}

// Handle boundary types for neighboring cell indices
long SimulationDomain::idx(long x, long y, int xoff, int yoff) {
    if (b_type[0] == BoundaryType::PERIODIC) {
        if (x + xoff > NX - 1) {
            x = x - NX + 1 + xoff;
        } else if (x + xoff < 0) {
            x = NX + x + xoff;
        } else { 
            x += xoff;
        }
    } else if (b_type[0] == BoundaryType::CONSTANT_VALUE) {
        if (x + xoff > NX - 1) {
            x = NX - 1;
        } else if (x + xoff < 0) { 
            x = 0;
        } else {
            x += xoff;
        }
    }

    if (b_type[1] == BoundaryType::PERIODIC) {
        if (y + yoff > NY - 1) {
            y = y - NY + 1 + yoff;
        } else if (y + yoff < 0) {
            y = NY + y + yoff;
        } else {
            y += yoff;
        }
    } else if (b_type[1] == BoundaryType::CONSTANT_VALUE) {
        if (y + yoff > NY - 1) {
            y = NY - 1;
        } else if (y + yoff < 0) {
            y = 0;
        } else {
            y += yoff;
        }
    }
    return idx(x, y);
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
                    (1.0 * s[idx(i, j, -1, -1)] +  // Top Row
                     2.0 * s[idx(i, j,  0, -1)] +
                     1.0 * s[idx(i, j,  1, -1)] +

                     2.0 * s[idx(i, j, -1,  0)] +  // Center Row
                     4.0 * s[idx(i, j,  0,  0)] +
                     2.0 * s[idx(i, j,  1,  0)] +

                     1.0 * s[idx(i, j, -1,  1)] +  // Bottom Row
                     2.0 * s[idx(i, j,  0,  1)] +
                     1.0 * s[idx(i, j,  1,  1)]) / 16.0;
            }
        }
        s.swap(old[field_index]);
    }
}

bool SimulationDomain::set_value(int field_id, long x, long y, double value) {
    if ((field_id < fields.size()) && // field id acceptable
        (x < NX) && (x > -1) && (y < NY) && (y > -1)) { // x/y limits
        if ((value >= field_value_limits[field_id].second) && (value <= field_value_limits[field_id].second)) {
            fields[field_id][idx(x, y)] = value;
            return true;
        }
    }
    return false;
}

bool SimulationDomain::set_value_shape(Shape shape, int field_id, long x0, long x1, long y0, long y1, double value) {
    if ((field_id < fields.size()) && // field id acceptable
        (x0 > -1) && (x1 < NX) && (y0 > -1) && (y1 < NY)) { // x/y limits acceptable
        if ((value >= field_value_limits[field_id].first) && (value <= field_value_limits[field_id].second)) {
            // TODO: Only needed for circle (minimal memory overhead if not using circle)
            double a = ((double)x0 - x1) / 2.0;
            double b = ((double)y0 - y1) / 2.0;
            double xs = ((double)x0 + x1) / 2.0;
            double ys = ((double)y0 + y1) / 2.0;
            double xx, yy;
            for (long y = y0; y <= y1; y++) {
                for (long x = x0; x <= x1; x++) {
                    switch (shape) {
                        case Shape::RECTANGLE:
                            fields[field_id][idx(x, y)] = value;
                            break;
                        case Shape::ELLIPSE:
                            xx = ((double)x - xs) / a;
                            yy = ((double)y - ys) / b;
                            if (xx * xx + yy * yy <= 1.0) {
                                fields[field_id][idx(x, y)] = value;
                            }
                            break;
                        default:
                            break;
                    }
                }
            }

            return true;
        }
    }
    return false;
}
