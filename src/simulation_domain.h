#pragma once
#include "container.h"
#include <vector>

class SimulationDomain {
    protected:
        const int N_SCALAR_FIELDS;
        std::vector<std::vector<double>> fields;
        std::vector<std::vector<double>> old;
        
        const long NX;
        const long NY;
        const double DELTA;
        const double DELTA_T;
        const long N_TIMESTEPS;
        BoundaryType b_type[2]; // TODO: Find a way for this to be const

        std::vector<double> extra_params;

        long idx(long x, long y);
        void init();
        
    public:
        SimulationDomain(SimParams& p, int n_scalar_fields);
        virtual ~SimulationDomain() = 0;

        virtual bool timestep(long t) = 0;
        inline DomainData get_data() {return {fields, N_SCALAR_FIELDS, NX, NY};} 

        double ddx(std::vector<double>& s, long  x, long  y);
        double ddy(std::vector<double>& s, long  x, long  y);
        double d2dx(std::vector<double>& s, long  x, long  y);
        double d2dy(std::vector<double>& s, long  x, long  y);
        long neighbor_cell_index(long x, long y, int xoff, int yoff);

        void add_noise(int field_index, double sigma);
        void smooth_gaussian(int field_index, int n_iterations);
};