#include <stdlib.h>
#include <omp.h>
#include <vector>
#include <time.h>   // ✅ FIX
#include "utils.h"

using namespace std;   // ✅ FIX

// ---------------- OPTIMIZED INTERPOLATION ----------------
void interpolation_optimized(double *mesh_value, Points *points) {

    int size = GRID_X * GRID_Y;

#pragma omp parallel for
    for (int i = 0; i < size; i++) {
        mesh_value[i] = 0.0;
    }

    int num_threads = omp_get_max_threads();

    vector<vector<double>> local_mesh(num_threads, vector<double>(size, 0.0));

#pragma omp parallel
    {
        int tid = omp_get_thread_num();

#pragma omp for
        for (long long i = 0; i < NUM_Points; i++) {

            int ix = (int)(points[i].x * NX);
            int iy = (int)(points[i].y * NY);

            if (ix >= NX) ix = NX - 1;
            if (iy >= NY) iy = NY - 1;

            int index = iy * GRID_X + ix;

            local_mesh[tid][index] += 1.0;
        }
    }

#pragma omp parallel for
    for (int i = 0; i < size; i++) {
        for (int t = 0; t < num_threads; t++) {
            mesh_value[i] += local_mesh[t][i];
        }
    }
}

// ---------------- PARALLEL IMMEDIATE MOVER ----------------
void mover_immediate_parallel(Points *points, double dx, double dy) {

#pragma omp parallel
    {
        unsigned int seed = time(NULL) ^ omp_get_thread_num();

#pragma omp for
        for (long long i = 0; i < NUM_Points; i++) {

            double rx = ((double)rand_r(&seed) / RAND_MAX) * 2 * dx - dx;
            double ry = ((double)rand_r(&seed) / RAND_MAX) * 2 * dy - dy;

            points[i].x += rx;
            points[i].y += ry;

            if (points[i].x < 0.0 || points[i].x > 1.0 ||
                points[i].y < 0.0 || points[i].y > 1.0) {

                points[i].x = (double) rand_r(&seed) / RAND_MAX;
                points[i].y = (double) rand_r(&seed) / RAND_MAX;
            }
        }
    }
}