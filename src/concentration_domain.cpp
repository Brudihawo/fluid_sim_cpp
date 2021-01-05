#include "concentration_domain.h"

void ConcentrationDomain::init() {
    for(int j = 0; j < NY; j++) {
        for (int i = 0; i < NX / 2; i++) {
            c[idx(i, j)] = 1.0;
        }
    }
}

ConcentrationDomain::ConcentrationDomain(SimParams& p, std::vector<double>& d): SimulationDomain(p, 1), D(d[0]), c(fields[0]), old_c(old[0]) {
    field_descriptors.push_back("Concentration");
    field_value_limits.push_back({ 0.0, 1.0 });
    init();
}

ConcentrationDomain::~ConcentrationDomain() {
}

bool ConcentrationDomain::timestep(long t) {
    for (int i = 0; i < NX * NY; i++) {
        old_c[i] = c[i];
    }
    for (int j = 0; j < NY; j++) {
        for (int i = 0; i < NX; i++) {
            c[idx(i, j)] = old_c[i] + D * (d2dx(c, i, j) + d2dy(c, i, j)) * DELTA_T;
        }
    }
    return t < N_TIMESTEPS;
}

bool ConcentrationDomain::value_guard_insert(int field_id, long x, long y, double value) {
    switch (field_id) {
        case 0: // Concentration Field
            if ((value <= 1.0) && (value >= 0.0)) {
                return set_value(field_id, x, y, value);
                break;
            }
        default: // Invalid field id
            return false;
            std::cerr << "Invalid Field ID: " << field_id << " on insert." << std::endl;
    }

    return false;
}
