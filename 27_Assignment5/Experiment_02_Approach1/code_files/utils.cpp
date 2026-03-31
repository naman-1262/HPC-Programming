#include <stdlib.h>
#include <omp.h>
#include "utils.h"

// ---------------- PARALLEL INTERPOLATION ----------------
void interpolation_parallel(double *mesh_value, Points *points) {

#pragma omp parallel for
    for (int i = 0; i < GRID_X * GRID_Y; i++) {
        mesh_value[i] = 0.0;
    }

#pragma omp parallel for
    for (long long i = 0; i < NUM_Points; i++) {

        int ix = (int)(points[i].x * NX);
        int iy = (int)(points[i].y * NY);

        if (ix >= NX) ix = NX - 1;
        if (iy >= NY) iy = NY - 1;

        int index = iy * GRID_X + ix;

#pragma omp atomic
        mesh_value[index] += 1.0;
    }
}

// ---------------- PARALLEL DEFERRED MOVER ----------------
void mover_deferred_parallel(Points *points, double dx, double dy) {

    // Step 1: Move + mark invalid (parallel)
#pragma omp parallel for
    for (long long i = 0; i < NUM_Points; i++) {

        double rx = ((double)rand() / RAND_MAX) * 2 * dx - dx;
        double ry = ((double)rand() / RAND_MAX) * 2 * dy - dy;

        points[i].x += rx;
        points[i].y += ry;

        if (points[i].x < 0.0 || points[i].x > 1.0 ||
            points[i].y < 0.0 || points[i].y > 1.0) {

            points[i].x = -1.0; // mark invalid
        }
    }

    // Step 2: Compact (sequential - avoids race)
    long long idx = 0;
    for (long long i = 0; i < NUM_Points; i++) {
        if (points[i].x >= 0.0) {
            points[idx++] = points[i];
        }
    }

    // Step 3: Insert new particles (parallel)
#pragma omp parallel for
    for (long long i = idx; i < NUM_Points; i++) {
        points[i].x = (double) rand() / RAND_MAX;
        points[i].y = (double) rand() / RAND_MAX;
    }
}