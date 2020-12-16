# 2D-Simulation Framework with live viewing written in C++
This is a framework for implementing boundary conditions in 

## Updates
- Diffusion simulation is working
- Currently, boundary conditions 

## TODOS:
1. Implement field descriptors
2. Implement different boundary conditions
3. Implement serialization

--- 

## Implementing a new Simulation Domain Class

A new simulation domain class needs to implement the functions `bool timestep(long t)` and `void init()`.

### concentration_domain.h:
``` cpp
class ConcentrationDomain : public SimulationDomain {
    private: 
        std::vector<double>& c; // Concentration scalar field
        std::vector<double>& old_c; // Previous timestep concentration values;
        double D; // Diffusion Coefficient
        
        void init();
    
    public:
        ConcentrationDomain(SimParams& p, std::vector<double>& additional_params);
        ~ConcentrationDomain();
        bool timestep(long t);
};
```

### concentration_domain.cpp:
The constructor should initialise the superclass with the base simulation parameters and a number of scalar fields. In this case, one (the concentration field) is sufficient. 
It is recommended to define a reference to the scalar field and a previous version of the scalar field as a member variable to increase readability.
```cpp
ConcentrationDomain::ConcentrationDomain(SimParams& p, std::vector<double>& d): SimulationDomain(p, 1), D(d[0]), c(fields[0]), old_c(old[0]) {
    init();
}
```
`void init()` initialises scalar field values if you need something else than zero as the initial condition.
```cpp
void ConcentrationDomain::init() {
    for(int j = 0; j < NY; j++) {
        for (int i = 0; i < NX / 2; i++) {
            c[idx(i, j)] = 1.0;
        }
    }
}
```
`bool timestep(long t)` specifies how the scalar field (or fields) evolve per timestep. `long t` is passed to this function to specify the simulation endpoint by returning `false`. You might also want to check for stationary states and use that as a cancelling condition.
```cpp
bool ConcentrationDomain::timestep(long t) {
    for (int i = 0; i < NX * NY; i++) {
        old_c[i] = c[i];
    }
    for (int j = 0; j < NY; j++) {
        for (int i = 0; i < NX; i++) {
            c[idx(i, j)] = old_c[i] + D * (d2dx(c, i, j) + d2dy(c, i, j)) * DELTA_T;
        }
    }
    return t < N_TIMESTEPS;
}
```
