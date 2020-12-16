#pragma once 
#include "container.h"
#include "simulation_domain.h"

class ConcentrationDomain : public SimulationDomain {
    private: 
        double* c; // Concentration scalar field
        double* old; // Temp old values
        double D; // Diffusion Coefficient
        

        void init();
    
    public:
        ConcentrationDomain(SimParams& p, double* additional_params);
        ~ConcentrationDomain();
        bool timestep(long t);
};