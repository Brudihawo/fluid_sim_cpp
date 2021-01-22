# 2D-Simulation Framework with live viewing written in C++
This is a framework for implementing 2D scalar field evolution algorithms and viewing them in real time using [Dear ImGui](https://github.com/ocornut/imgui) and the plotting library [ImPlot](https://github.com/epezent/implot).

## Updates
- Diffusion simulation is working
- Infile Parsing Done

## TODOS:
2. Implement Phase-Field Method (Grandchem etc) for 2 components
3. Update docs
3. Implement serialization (write_timesteps and end file format (also, file reader -> Plotting with matplotlib or *implot*?))
4. Update "Implementing a new Simulation Domain Class" with information on field descriptors and field limit values.
5. Add visual/console mode to infile
--- 

## Infile Parsing:
Using infiles to input initial scalar fields, more complex simulations have become possible. 
Applying gaussian smoothing to the domain may fix sime artifacts.

We now can make stuff like this:

![](img/diffusion_better_geometry.png)



## Implementing a new Simulation Domain Class

A new simulation domain class needs to implement the functions `bool timestep(long t)` and `void init()`, as well as setting the upper and lower bound for field values.

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
ConcentrationDomain::ConcentrationDomain(SimParams& p, std::vector<double>& d):
        SimulationDomain(p, 1), D(d[0]),
        c(fields[0]), old_c(old[0]) {
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

This domains `init()` function creates half of the domain with c=1, the other half with c=0. A simulation frame can look something like this:

![Simulation Frame](img/diffusion_171220.png)