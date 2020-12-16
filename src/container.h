#pragma once
#include <vector>

/* Domain data container@n 
double ** fields: Pointer to scalar field array@n
int N_SCALAR_FIELDS: Number of scalar fields@n
long NX: Resolution of fields in x@n
long NY: Resolution of fields in y*/
typedef struct domain_data_t {
    std::vector<std::vector<double>>& fields;
    int N_SCALAR_FIELDS;
    long NX;
    long NY;
} DomainData;

/* Minimum parameters to create simulation@n
long NX: Resolution in X@n
long NY: Resolution in Y@n
double DELTA: Spatial Resolution@n
double DELTA_T: Timestep@n
long N_TIMESTEPS: Number of timesteps@n
int N_SCALAR_FIELS: Number of scalar fields*/ 
typedef struct sim_params_t {
    long NX;
    long NY;
    double DELTA;
    double DELTA_T;
    long N_TIMESTEPS;
    int N_SCALAR_FIELDS;
} SimParams;

/* View Configuration Parameters@n
XRES: Resolution in X@n
YRES: Resolution in Y@n
timeskip: Number of timesteps to calculate between rendered frames*/
typedef struct view_params_t {
    int XRES, YRES, timeskip;
} ViewParams;

enum SimType {
    NONE = -1,
    FLUID_INCOMPRESSIBLE,
    CONCENTRATION,
};