#pragma once
#include "container.h"
#include "simulation_domain.h"
#include <vector>

class FluidIncompressibleDomain : public SimulationDomain {
    private:
        std::vector<double>& u;
        std::vector<double>& v;
        std::vector<double>& old_u;
        std::vector<double>& old_v;
        
        double NU;

    public:
        FluidIncompressibleDomain(SimParams& p, std::vector<double>& extra_params);
        ~FluidIncompressibleDomain();
        bool timestep(long t) override;
};