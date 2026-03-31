#ifndef INIT_H
#define INIT_H

#include <stdio.h>

// Point structure
typedef struct {
    double x, y;
} Points;

// Global simulation parameters
extern int GRID_X, GRID_Y, NX, NY;
extern long long NUM_Points;
extern int Maxiter;
extern double dx, dy;

// Initialization
void initializepoints(Points *points);

#endif