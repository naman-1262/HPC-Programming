#ifndef UTILS_H
#define UTILS_H

#include "init.h"

void interpolation_optimized(double *mesh_value, Points *points);
void mover_immediate_parallel(Points *points, double dx, double dy);

#endif