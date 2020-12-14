#pragma once
#include "container.h"

double ddx(double** s, long  x, long  y, long  NX);
double ddy(double** s, long  x, long  y, long  NY);
double d2dx(double** s, long  x, long  y, long  NX);
double d2dy(double** s, long  x, long  y, long  NY);

void smooth(int n_iterations, double** s, double** tmp, long NX, long  NY);

void find_min_max(interval& iv, double* s, long  NX, long  NY);
