#pragma once
#include "container.h"

class SimulationDomain {
    protected:
        double** fields;
        int N_SCALAR_FIELDS;
        double* extra_params;

        long NX;
        long NY;
        double DELTA;
        double DELTA_T;
        long N_TIMESTEPS;

        long idx(long x, long y);
        void init();
        
    public:
        SimulationDomain(SimParams& p);
        virtual ~SimulationDomain() = 0;

        virtual bool timestep(long t) = 0;
        inline DomainData get_data() {return {fields, N_SCALAR_FIELDS, NX, NY};} 

        double ddx(double* s, long  x, long  y);
        double ddy(double* s, long  x, long  y);
        double d2dx(double* s, long  x, long  y);
        double d2dy(double* s, long  x, long  y);
};