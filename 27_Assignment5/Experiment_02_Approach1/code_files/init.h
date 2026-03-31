#ifndef INIT_H
#define INIT_H

#include <stdio.h>

typedef struct {
    double x, y;
} Points;

extern int GRID_X, GRID_Y, NX, NY;
extern long long NUM_Points;
extern int Maxiter;
extern double dx, dy;

void initializepoints(Points *points);

#endif