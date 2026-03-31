#include <stdio.h>
#include <stdlib.h>
#include "utils.h"

// ---------------- INTERPOLATION (SERIAL) ----------------
void interpolation(double *mesh_value, Points *points) {

    for (int i = 0; i < GRID_X * GRID_Y; i++) {
        mesh_value[i] = 0.0;
    }

    for (long long i = 0; i < NUM_Points; i++) {

        int ix = (int)(points[i].x * NX);
        int iy = (int)(points[i].y * NY);

        if (ix >= NX) ix = NX - 1;
        if (iy >= NY) iy = NY - 1;

        int index = iy * GRID_X + ix;

        mesh_value[index] += 1.0;
    }
}

// ---------------- MOVER (DEFERRED INSERTION - SERIAL) ----------------
void mover_deferred(Points *points, double deltaX, double deltaY) {

    long long delete_count = 0;

    // Step 1: Move particles & mark invalid
    for (long long i = 0; i < NUM_Points; i++) {

        double rx = ((double)rand() / RAND_MAX) * 2 * deltaX - deltaX;
        double ry = ((double)rand() / RAND_MAX) * 2 * deltaY - deltaY;

        points[i].x += rx;
        points[i].y += ry;

        if (points[i].x < 0.0 || points[i].x > 1.0 ||
            points[i].y < 0.0 || points[i].y > 1.0) {

            points[i].x = -1.0; // mark invalid
            delete_count++;
        }
    }

    // Step 2: Compact valid particles
    long long idx = 0;

    for (long long i = 0; i < NUM_Points; i++) {
        if (points[i].x >= 0.0) {
            points[idx++] = points[i];
        }
    }

    // Step 3: Insert new particles
    for (long long i = idx; i < NUM_Points; i++) {
        points[i].x = (double) rand() / RAND_MAX;
        points[i].y = (double) rand() / RAND_MAX;
    }
}