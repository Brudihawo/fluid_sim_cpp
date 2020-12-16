#pragma once
#include "container.h"
#include "simulation_domain.h"

class IncompressibleFluidDomain: public SimulationDomain {
    private:
        std::vector<double>& u;
        std::vector<double>& v;
        std::vector<double>& p;

        double& NU;

        void init();
    public:
        IncompressibleFluidDomain(SimParams& p, std::vector<double>& extra_params);
        ~IncompressibleFluidDomain();

        bool timestep(long t) override;
};