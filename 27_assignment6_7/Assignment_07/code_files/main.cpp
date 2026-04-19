#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#ifdef _OPENMP
#include <omp.h>
#endif
#include "init.h"
#include "utils.h"

// Global variables
int GRID_X, GRID_Y, NX, NY;
int NUM_Points, Maxiter;
double dx, dy;

/*
 * wall_time() — always returns wall-clock seconds.
 *
 * FIX: The original code used clock(), which sums CPU time across ALL
 * threads.  With N threads, clock() returns approximately N × wall_time,
 * making every parallel run look SLOWER than serial.
 * omp_get_wtime() measures real elapsed time and is the correct timer
 * for OpenMP programs.  For the serial build we fall back to clock().
 */
static inline double wall_time(void) {
#ifdef _OPENMP
    return omp_get_wtime();
#else
    return (double)clock() / CLOCKS_PER_SEC;
#endif
}

int main(int argc, char **argv) {

    if (argc != 2) {
        printf("Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "rb");
    if (!file) { printf("Error opening input file\n"); exit(1); }

    fread(&NX,         sizeof(int), 1, file);
    fread(&NY,         sizeof(int), 1, file);
    fread(&NUM_Points, sizeof(int), 1, file);
    fread(&Maxiter,    sizeof(int), 1, file);

    GRID_X = NX + 1;
    GRID_Y = NY + 1;
    dx = 1.0 / NX;
    dy = 1.0 / NY;

    double *mesh_value = (double *) calloc(GRID_X * GRID_Y, sizeof(double));
    Points *points     = (Points *) calloc(NUM_Points, sizeof(Points));

    double total_int_time    = 0.0;
    double total_norm_time   = 0.0;
    double total_move_time   = 0.0;
    double total_denorm_time = 0.0;

    read_points(file, points);

    for (int iter = 0; iter < Maxiter; iter++) {

        double t0 = wall_time();
        interpolation(mesh_value, points);
        double t1 = wall_time();

        normalization(mesh_value);
        double t3 = wall_time();

        mover(mesh_value, points);
        double t4 = wall_time();

        denormalization(mesh_value);
        double t5 = wall_time();

        total_int_time    += t1 - t0;
        total_norm_time   += t3 - t1;
        total_move_time   += t4 - t3;
        total_denorm_time += t5 - t4;
    }

    save_mesh(mesh_value);

    double total_algo_time = total_int_time + total_norm_time
                           + total_move_time + total_denorm_time;

    printf("Total Interpolation Time   = %lf seconds\n", total_int_time);
    printf("Total Normalization Time   = %lf seconds\n", total_norm_time);
    printf("Total Mover Time           = %lf seconds\n", total_move_time);
    printf("Total Denormalization Time = %lf seconds\n", total_denorm_time);
    printf("Total Algorithm Time       = %lf seconds\n", total_algo_time);
    printf("Total Number of Voids      = %lld\n", void_count(points));

    int num_threads = 1;
#ifdef _OPENMP
    #pragma omp parallel
    {
        #pragma omp single
        num_threads = omp_get_num_threads();
    }
#endif
    printf("RESULT,%d,%d,%d,%d,%d,%lf\n",
           NX, NY, NUM_Points, Maxiter, num_threads, total_algo_time);

    free(mesh_value);
    free(points);
    fclose(file);
    return 0;
}
