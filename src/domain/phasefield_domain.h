#pragma once

#include "simulation_domain.h"

class PhasefieldDomain : public SimulationDomain {
    private:
        std::vector<double>& phi;
        std::vector<double>& old_phi;

        // phasefield parameters
        double eps;
        double tau;
        double sigma;
        double L;

        double T;  // Temperature constant
        double Tm; // Phase change Temperature

        void function_a(long& x, long& y, double& a);
        void function_w(long& x, long& y, double& w);
        void function_f(long& x, long& y, double& f);

    public: 
        PhasefieldDomain(SimParams& p, std::vector<double> additional_params);
        ~PhasefieldDomain();

        bool timestep(long t) override;
};