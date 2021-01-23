# INFILE SYNTAX

Infiles tell the solver what to simulate. They specify initial conditions and the simulation type. 
Define constants by `KEY = VALUE` syntax. Infiles are processed line by line, but whitespace other than line breaks is ignored. 

First, we define the rendering options:
``` python
# Window Resolution for render [int]
res_x = 800
res_y = 700

# Number of timesteps between each render [int]
timeskip = 50

```


Then, we define basic simulation domain options:
1. Simulation type
2. Domain size in voxels
3. Spatial size unit
4. Timestep size
5. Number of timesteps to simulate
6. Boundary conditions

``` python
simtype = 0
# Simulation 

# Number of voxels in x/y direction
NX = 100
NY = 100

# Spatial step size
DELTA = 1.0

# Timestep Size
DELTA_T = 0.08

# Number of Timesteps
N_TIMESTEPS = 200000

# Boundary Types
## Possible Types: 
## periodic, constant_val, constant_grad
b_type = (periodic, periodic)

```


Next, we define simulation type specific constants.
``` python
# Diffusion Simulation
D = 1.0

# Phasefield Simulation
EPS = 5.0
TAU = 5.0
SIGMA = 1.0
L = 1.0
Tm = 1.0
T = 0.95

```

Defining geometry works like this:
``` python
# Syntax: 
# shape(field id) value, startx, starty, endx, endy
# Currently defined shapes: ellipse and rectangle
rect(0) 1.0, 20, 20, 80, 80 
ellipse(0) 0.0, 20, 20, 80, 80

# Apply gaussian smoothing to field.
# This resolves some simulation artifacts caused by discontinuities.
# Syntax: smooth(field_id, n_iterations)
smooth(0, 3)
```
Depending on the simulation type, setting initial conditions in other fields is also possible. Non-Initialised field values will default to zero.