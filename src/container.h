#pragma once
#include <vector>
#include <string>

// ADD SIMULATION TYPES HERE
enum class SimType {
    NONE = -1,
    CONCENTRATION,
    PHASEFIELD,
    FLUID_INCOMPRESSIBLE,
};

enum class BoundaryType {
    INVALID = -1,
    PERIODIC,
    CONSTANT_VALUE,
    CONSTANT_GRADIENT,
//    CONSTANT_CURVATURE,
//  ZERO // IS THIS SENSIBLE?
};

enum class Shape {
    RECTANGLE = 0,
    ELLIPSE
};

/* Domain data container@n 
double ** fields: Pointer to scalar field array@n
int N_SCALAR_FIELDS: Number of scalar fields@n
long NX: Resolution of fields in x@n
long NY: Resolution of fields in y*/
typedef struct DomainData{ 
    std::vector<std::vector<double>>& fields;
    std::vector<std::string>& field_descriptors;
    std::vector<std::pair<double, double>>& val_limits;
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
typedef struct SimParams {
    long NX;
    long NY;
    double DELTA;
    double DELTA_T;
    long N_TIMESTEPS;
    int N_SCALAR_FIELDS;
    BoundaryType b_type[2] = {BoundaryType::INVALID, BoundaryType::INVALID};
} SimParams;

/* View Configuration Parameters@n
XRES: Resolution in X@n
YRES: Resolution in Y@n
timeskip: Number of timesteps to calculate between rendered frames*/
typedef struct ViewParams {
    int XRES, YRES, timeskip;
} ViewParams;