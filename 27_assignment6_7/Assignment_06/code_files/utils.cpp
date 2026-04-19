// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <omp.h>
// #include "utils.h"

// // Parallel interpolation using thread-private grids
// void interpolation(double *mesh_value, Points *points) {

//     int num_threads = omp_get_max_threads();

//     // Allocate thread-private grids
//     double **mesh_private = (double **) malloc(num_threads * sizeof(double*));

//     for (int t = 0; t < num_threads; t++) {
//         mesh_private[t] = (double *) calloc(GRID_X * GRID_Y, sizeof(double));
//     }

//     #pragma omp parallel
//     {
//         int tid = omp_get_thread_num();

//         #pragma omp for
//         for (int k = 0; k < NUM_Points; k++) {

//             double x = points[k].x;
//             double y = points[k].y;

//             int i = x / dx;
//             int j = y / dy;

//             // Boundary handling
//             if (i >= NX) i = NX - 1;
//             if (j >= NY) j = NY - 1;

//             double X = i * dx;
//             double Y = j * dy;

//             double lx = x - X;
//             double ly = y - Y;

//             double w1 = (dx - lx) * (dy - ly);
//             double w2 = ly * (dx - lx);
//             double w3 = lx * (dy - ly);
//             double w4 = lx * ly;

//             // Update private grid
//             mesh_private[tid][j * GRID_X + i]         += w1;
//             mesh_private[tid][j * GRID_X + (i+1)]     += w3;
//             mesh_private[tid][(j+1) * GRID_X + i]     += w2;
//             mesh_private[tid][(j+1) * GRID_X + (i+1)] += w4;
//         }
//     }

//     // Reduction
//     #pragma omp parallel for
//     for (int idx = 0; idx < GRID_X * GRID_Y; idx++) {
//         for (int t = 0; t < num_threads; t++) {
//             mesh_value[idx] += mesh_private[t][idx];
//         }
//     }

//     // Free memory
//     for (int t = 0; t < num_threads; t++) {
//         free(mesh_private[t]);
//     }
//     free(mesh_private);
// }

// // Write mesh to file
// void save_mesh(double *mesh_value) {

//     FILE *fd = fopen("Mesh.out", "w");
//     if (!fd) {
//         printf("Error creating Mesh.out\n");
//         exit(1);
//     }

//     for (int i = 0; i < GRID_Y; i++) {
//         for (int j = 0; j < GRID_X; j++) {
//             fprintf(fd, "%lf ", mesh_value[i * GRID_X + j]);
//         }
//         fprintf(fd, "\n");
//     }

//     fclose(fd);
// }
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include "utils.h"

// Parallel interpolation using atomic updates on a single global mesh
void interpolation(double *mesh_value, Points *points) {

    #pragma omp parallel for schedule(static)
    for (int k = 0; k < NUM_Points; k++) {

        double x = points[k].x;
        double y = points[k].y;

        int i = (int)(x / dx);
        int j = (int)(y / dy);

        // Boundary handling
        if (i >= NX) i = NX - 1;
        if (j >= NY) j = NY - 1;

        double X = i * dx;
        double Y = j * dy;

        double lx = x - X;
        double ly = y - Y;

        double w1 = (dx - lx) * (dy - ly);
        double w2 = ly * (dx - lx);
        double w3 = lx * (dy - ly);
        double w4 = lx * ly;

        // Atomic updates to the shared mesh
        #pragma omp atomic
        mesh_value[j * GRID_X + i]         += w1;
        #pragma omp atomic
        mesh_value[j * GRID_X + (i + 1)]   += w3;
        #pragma omp atomic
        mesh_value[(j + 1) * GRID_X + i]   += w2;
        #pragma omp atomic
        mesh_value[(j + 1) * GRID_X + (i + 1)] += w4;
    }
}

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
