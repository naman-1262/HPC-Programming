#ifndef UTILS_H
#define UTILS_H

#include "init.h"

void interpolation_parallel(double *mesh_value, Points *points);
void mover_deferred_parallel(Points *points, double dx, double dy);

#endif