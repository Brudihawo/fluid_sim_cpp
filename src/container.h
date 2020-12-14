#pragma once

typedef struct interval_t {
    double max;
    double min;
} interval;

typedef struct sim_params_t {
    long NX;
    long NY;
    double DELTA_T;
    long N_TIMESTEPS;
    double NU;
    double v_init;
    int dist_center_init;
    int timeskip;
} sim_params;