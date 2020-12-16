#pragma once
#include "container.h"
#include "simulation_domain.h"

class IncompressibleFluidDomain: public SimulationDomain {
    private:
        double* u;
        double* v;
        double* p;

        double& NU;

        void init();
    public:
        IncompressibleFluidDomain(SimParams& p, double* extra_params);
        ~IncompressibleFluidDomain();
        bool timestep(long t) override;
        void smooth(int n_iterations);
        void add_jitter();
};