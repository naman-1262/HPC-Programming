#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#ifdef _OPENMP
#include <omp.h>
#endif
#include "utils.h"

double min_val, max_val;

/*
 * interpolation (scatter: particle -> mesh)
 *
 * FIX 1: Per-thread private meshes are laid out in one contiguous block
 *         (nthreads × total) so the reduction is cache-friendly.
 *
 * FIX 2: Reduction loop restructured to:
 *             for t in threads:          <- outer
 *               for k in cells: ...      <- inner, stride-1 sequential read
 *         The old code did it the other way around, causing cache misses
 *         proportional to nthreads on every inner step.
 */
void interpolation(double *mesh_value, Points *points) {

    int total = GRID_X * GRID_Y;
    memset(mesh_value, 0, total * sizeof(double));

#ifdef _OPENMP
    int nthreads = omp_get_max_threads();

    /* One contiguous block: row t = private mesh for thread t */
    double *block = (double *) calloc((size_t)nthreads * total, sizeof(double));
    if (!block) { perror("calloc private mesh"); exit(1); }

    double **private_mesh = (double **) malloc(nthreads * sizeof(double *));
    if (!private_mesh) { perror("malloc ptrs"); exit(1); }
    for (int t = 0; t < nthreads; t++)
        private_mesh[t] = block + (size_t)t * total;

    /* Scatter: each thread accumulates into its own private mesh */
    #pragma omp parallel
    {
        int tid = omp_get_thread_num();
        double *lmesh = private_mesh[tid];

        #pragma omp for schedule(static)
        for (int p = 0; p < NUM_Points; p++) {
            if (points[p].is_void) continue;

            double x = points[p].x;
            double y = points[p].y;

            int ci = (int)(x / dx);
            int cj = (int)(y / dy);
            if (ci >= NX) ci = NX - 1;
            if (cj >= NY) cj = NY - 1;

            double Xi = ci * dx;
            double Yj = cj * dy;
            double lx = x - Xi;
            double ly = y - Yj;

            double w00 = (dx - lx) * (dy - ly);
            double w10 = ly        * (dx - lx);
            double w01 = lx        * (dy - ly);
            double w11 = lx        * ly;

            lmesh[ cj      * GRID_X + ci    ] += w00;
            lmesh[(cj + 1) * GRID_X + ci    ] += w10;
            lmesh[ cj      * GRID_X + ci + 1] += w01;
            lmesh[(cj + 1) * GRID_X + ci + 1] += w11;
        }
    }

    /*
     * Cache-friendly reduction:
     *   Outer loop = threads  →  each inner pass is a stride-1 sequential
     *   read through private_mesh[t][0..total-1].
     * Start by copying thread 0's data, then add threads 1..N-1 in parallel.
     */
    memcpy(mesh_value, private_mesh[0], total * sizeof(double));
    for (int t = 1; t < nthreads; t++) {
        double *src = private_mesh[t];
        #pragma omp parallel for schedule(static)
        for (int k = 0; k < total; k++)
            mesh_value[k] += src[k];
    }

    free(private_mesh);
    free(block);

#else
    for (int p = 0; p < NUM_Points; p++) {
        if (points[p].is_void) continue;

        double x = points[p].x;
        double y = points[p].y;

        int ci = (int)(x / dx);
        int cj = (int)(y / dy);
        if (ci >= NX) ci = NX - 1;
        if (cj >= NY) cj = NY - 1;

        double Xi = ci * dx;
        double Yj = cj * dy;
        double lx = x - Xi;
        double ly = y - Yj;

        double w00 = (dx - lx) * (dy - ly);
        double w10 = ly        * (dx - lx);
        double w01 = lx        * (dy - ly);
        double w11 = lx        * ly;

        mesh_value[ cj      * GRID_X + ci    ] += w00;
        mesh_value[(cj + 1) * GRID_X + ci    ] += w10;
        mesh_value[ cj      * GRID_X + ci + 1] += w01;
        mesh_value[(cj + 1) * GRID_X + ci + 1] += w11;
    }
#endif
}

/*
 * normalization
 */
void normalization(double *mesh_value) {

    int total = GRID_X * GRID_Y;
    min_val =  DBL_MAX;
    max_val = -DBL_MAX;

#ifdef _OPENMP
    #pragma omp parallel for reduction(min:min_val) reduction(max:max_val) schedule(static)
#endif
    for (int k = 0; k < total; k++) {
        if (mesh_value[k] < min_val) min_val = mesh_value[k];
        if (mesh_value[k] > max_val) max_val = mesh_value[k];
    }

    double range = max_val - min_val;

    if (range == 0.0) {
#ifdef _OPENMP
        #pragma omp parallel for schedule(static)
#endif
        for (int k = 0; k < total; k++) mesh_value[k] = 0.0;
        return;
    }

#ifdef _OPENMP
    #pragma omp parallel for schedule(static)
#endif
    for (int k = 0; k < total; k++)
        mesh_value[k] = 2.0 * (mesh_value[k] - min_val) / range - 1.0;
}

/*
 * mover (gather: mesh -> particle) — no race conditions.
 */
void mover(double *mesh_value, Points *points) {

#ifdef _OPENMP
    #pragma omp parallel for schedule(static)
#endif
    for (int p = 0; p < NUM_Points; p++) {
        if (points[p].is_void) continue;

        double x = points[p].x;
        double y = points[p].y;

        int ci = (int)(x / dx);
        int cj = (int)(y / dy);
        if (ci >= NX) ci = NX - 1;
        if (cj >= NY) cj = NY - 1;

        double Xi = ci * dx;
        double Yj = cj * dy;
        double lx = x - Xi;
        double ly = y - Yj;

        double w00 = (dx - lx) * (dy - ly);
        double w10 = ly        * (dx - lx);
        double w01 = lx        * (dy - ly);
        double w11 = lx        * ly;

        double Fi = w00 * mesh_value[ cj      * GRID_X + ci    ]
                  + w10 * mesh_value[(cj + 1) * GRID_X + ci    ]
                  + w01 * mesh_value[ cj      * GRID_X + ci + 1]
                  + w11 * mesh_value[(cj + 1) * GRID_X + ci + 1];

        double x_new = x + Fi * dx;
        double y_new = y + Fi * dy;

        if (x_new < 0.0 || x_new > 1.0 || y_new < 0.0 || y_new > 1.0)
            points[p].is_void = true;
        else {
            points[p].x = x_new;
            points[p].y = y_new;
        }
    }
}

/*
 * denormalization
 */
void denormalization(double *mesh_value) {
    int total = GRID_X * GRID_Y;
    double range = max_val - min_val;

#ifdef _OPENMP
    #pragma omp parallel for schedule(static)
#endif
    for (int k = 0; k < total; k++)
        mesh_value[k] = (mesh_value[k] + 1.0) / 2.0 * range + min_val;
}

/* Count void particles */
long long int void_count(Points *points) {
    long long int voids = 0;
    for (int i = 0; i < NUM_Points; i++)
        voids += (int)points[i].is_void;
    return voids;
}

/* Write mesh to file */
void save_mesh(double *mesh_value) {
    FILE *fd = fopen("Mesh.out", "w");
    if (!fd) { printf("Error creating Mesh.out\n"); exit(1); }
    for (int i = 0; i < GRID_Y; i++) {
        for (int j = 0; j < GRID_X; j++)
            fprintf(fd, "%lf ", mesh_value[i * GRID_X + j]);
        fprintf(fd, "\n");
    }
    fclose(fd);
}
