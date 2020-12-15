#pragma once

typedef struct interval_t {
    double max;
    double min;
} interval;

// Possibly add X, Y coordinates
typedef struct domain_data_t {
    const double* u;
    const double* v;
    const double* p;
    const long NX;
    const long NY;
} domain_data;

typedef struct sim_params_t {
    long NX;
    long NY;
    double DELTA_T;
    double DELTA;
    long N_TIMESTEPS;
    double NU;
    double v_init;
    int dist_center_init;
    int timeskip;
} sim_params;

class simulation_domain {
    private:
        double* u;
        double* v;
        double* p;
        
        long NX;
        long NY;
        double DELTA;

        double DELTA_T;
        long N_TIMESTEPS;

        double NU;

        void init();
        long idx(long x, long y);

    public:
        simulation_domain(long nx, long ny, double delta, long n_timesteps, double delta_t, int tskip, double nu);
        simulation_domain(sim_params& params);
        ~simulation_domain();

        bool timestep(long t);
        void smooth(int n_iterations);
        void add_jitter();

        inline const domain_data get_data() {return {u, v, p, NX, NY};}

        // void set_cell(long x, long y, double p, double u, double v);

        double ddx(double* s, long  x, long  y);
        double ddy(double* s, long  x, long  y);
        double d2dx(double* s, long  x, long  y);
        double d2dy(double* s, long  x, long  y);
};