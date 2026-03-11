#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include "utils.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <omp.h>
#include "utils.h"

// -----------------------------
// Optimized Serial Interpolation
// -----------------------------
void interpolation(double * __restrict mesh_value,
                   Points * __restrict points)
{
    const double inv_dx = 1.0 / dx;
    const double inv_dy = 1.0 / dy;

    #pragma omp simd
    for (long long p = 0; p < NUM_Points; p++)
    {
        const double x = points[p].x;
        const double y = points[p].y;

        int i = (int)(x * inv_dx);
        int j = (int)(y * inv_dy);

        if (i >= NX - 1) i = NX - 2;
        if (j >= NY - 1) j = NY - 2;

        // Faster local coordinate calculation
        const double dx_local = x * inv_dx - i;
        const double dy_local = y * inv_dy - j;

        const double one_minus_dx = 1.0 - dx_local;
        const double one_minus_dy = 1.0 - dy_local;

        const double w00 = one_minus_dx * one_minus_dy;
        const double w10 = dx_local * one_minus_dy;
        const double w01 = one_minus_dx * dy_local;
        const double w11 = dx_local * dy_local;

        const int base = j * GRID_X + i;

        mesh_value[base]                  += w00;
        mesh_value[base + 1]              += w10;
        mesh_value[base + GRID_X]         += w01;
        mesh_value[base + GRID_X + 1]     += w11;
    }
}
// Interpolation (Serial Code)
// void interpolation(double *mesh_value, Points *points) {}
// void interpolation(double *mesh_value, Points *points) {

//     const double inv_dx = 1.0 / dx;
//     const double inv_dy = 1.0 / dy;

//     for (int p = 0; p < NUM_Points; p++) {

//         double x = points[p].x;
//         double y = points[p].y;

//         int i = (int)(x * inv_dx);
//         int j = (int)(y * inv_dy);

//         // CRITICAL FIX
//         if (i >= NX - 1) i = NX - 2;
//         if (j >= NY - 1) j = NY - 2;

//         double x_i = i * dx;
//         double y_j = j * dy;

//         double dx_local = (x - x_i) * inv_dx;
//         double dy_local = (y - y_j) * inv_dy;

//         double one_minus_dx = 1.0 - dx_local;
//         double one_minus_dy = 1.0 - dy_local;

//         double w00 = one_minus_dx * one_minus_dy;
//         double w10 = dx_local * one_minus_dy;
//         double w01 = one_minus_dx * dy_local;
//         double w11 = dx_local * dy_local;

//         int base = j * GRID_X + i;

//         mesh_value[base]                  += w00;
//         mesh_value[base + 1]              += w10;
//         mesh_value[base + GRID_X]         += w01;
//         mesh_value[base + GRID_X + 1]     += w11;
//     }
// }

// // Stochastic Mover (Serial Code) 
// void mover_serial(Points *points, double deltaX, double deltaY) {}

// // Stochastic Mover (Parallel Code) 
// void mover_parallel(Points *points, double deltaX, double deltaY) {}

void mover_serial(Points *points, double deltaX, double deltaY) {

    for (long long p = 0; p < NUM_Points; p++) {

        double new_x, new_y;

        do {
            double rand_dx = ((double)rand() / RAND_MAX) * 2.0 * deltaX - deltaX;
            double rand_dy = ((double)rand() / RAND_MAX) * 2.0 * deltaY - deltaY;

            new_x = points[p].x + rand_dx;
            new_y = points[p].y + rand_dy;

        } while (new_x < 0.0 || new_x > 1.0 || new_y < 0.0 || new_y > 1.0);

        points[p].x = new_x;
        points[p].y = new_y;
    }
}

// -------------------
// Parallel Mover (OpenMP)
// -------------------
void mover_parallel(Points *points, double deltaX, double deltaY) {

    #pragma omp parallel
    {
        unsigned int seed = time(NULL) ^ omp_get_thread_num();

        #pragma omp for
        for (long long p = 0; p < NUM_Points; p++) {

            double new_x, new_y;

            do {
                double rand_dx = ((double)rand_r(&seed) / RAND_MAX) * 2.0 * deltaX - deltaX;
                double rand_dy = ((double)rand_r(&seed) / RAND_MAX) * 2.0 * deltaY - deltaY;

                new_x = points[p].x + rand_dx;
                new_y = points[p].y + rand_dy;

            } while (new_x < 0.0 || new_x > 1.0 || new_y < 0.0 || new_y > 1.0);

            points[p].x = new_x;
            points[p].y = new_y;
        }
    }
}


// Write mesh to file
void save_mesh(double *mesh_value) {

    FILE *fd = fopen("Mesh.out", "w");
    if (!fd) {
        printf("Error creating Mesh.out\n");
        exit(1);
    }

    for (int i = 0; i < GRID_Y; i++) {
        for (int j = 0; j < GRID_X; j++) {
            fprintf(fd, "%lf ", mesh_value[i * GRID_X + j]);
        }
        fprintf(fd, "\n");
    }

    fclose(fd);
}