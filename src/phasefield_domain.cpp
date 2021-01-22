#include "phasefield_domain.h"
#include "discrete_math.h"

PhasefieldDomain::PhasefieldDomain(SimParams& p, std::vector<double> additional_params) :
    SimulationDomain(p, 1), phi(fields[0]), old_phi(old[0]),
    eps(additional_params[0]), tau(additional_params[1]), sigma(additional_params[2]),
    L(additional_params[3]), Tm (additional_params[4]), T(additional_params[5]) {
    field_descriptors.push_back("phi");
    field_value_limits.push_back({ 0.0, 1.0 });
}

PhasefieldDomain::~PhasefieldDomain() {
}

bool PhasefieldDomain::timestep(long t) {
    // Swap Pointers
    phi.swap(old_phi);
    double a, w, f;

    // calculate new phasefield values
    for (long y = 0; y < NY; y++) {
        for (long x = 0; x < NX; x++) {
            function_a(x, y, a);
            function_w(x, y, w);
            function_f(x, y, f);
            
            phi[idx(x, y)] = old_phi[idx(x, y)] + DELTA_T / tau * (a - w + f);
        }
    }
    return t < N_TIMESTEPS;
}


void PhasefieldDomain::function_a(long& x, long& y, double& a) {
    a = 2 * sigma * (d2dx(old_phi, x, y) + d2dy(old_phi, x, y));
}

void PhasefieldDomain::function_w(long& x, long& y, double& w) {
    w = 18 * sigma / (eps * eps) * (2 * whole_pow(old_phi[idx(x, y)], 3) - 3 * whole_pow(old_phi[idx(x, y)], 2) + old_phi[idx(x, y)]);
}

void PhasefieldDomain::function_f(long& x, long& y, double& f) {
    f = 6 * L / eps * (Tm - T) / Tm * (old_phi[idx(x, y)] - whole_pow(old_phi[idx(x, y)], 2));
}