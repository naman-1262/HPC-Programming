// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <time.h>
// #include <omp.h>

// #include "init.h"
// #include "utils.h"

// // Global variables
// int GRID_X, GRID_Y, NX, NY;
// int NUM_Points, Maxiter;
// double dx, dy;

// int main(int argc, char **argv) {

//     // Fixed Parameters
//     NX = 200;
//     NY = 200;
//     Maxiter = 10;
//     NUM_Points = 100000000;

//     // Since Number of points will be 1 more than number of cells
//     GRID_X = NX + 1;
//     GRID_Y = NY + 1;
//     dx = 1.0 / NX;
//     dy = 1.0 / NY;

//     // Fix Number of Threads
//     omp_set_num_threads(4);

//     // Allocate memory for grid and Points
//     double *mesh_value = (double *) calloc(GRID_X * GRID_Y, sizeof(double));
//     Points *points = (Points *) calloc(NUM_Points, sizeof(Points));

//     initializepoints(points);

//     printf("Iter\tInterp\t\tMover\t\tTotal\n");
//     for (int iter = 0; iter < Maxiter; iter++) {

//         // Interpolation timing
//         clock_t start_interp = clock();
//         interpolation(mesh_value, points);
//         clock_t end_interp = clock();

//         // Mover timing
//         clock_t start_move = clock();
//         mover_serial(points, dx, dy);
//         clock_t end_move = clock();

//         double interp_time = (double)(end_interp - start_interp) / CLOCKS_PER_SEC;
//         double move_time = (double)(end_move - start_move) / CLOCKS_PER_SEC;
//         double total = interp_time + move_time;

//         printf("%d\t%lf\t%lf\t%lf\n", iter+1, interp_time, move_time, total);
//     }

//     // Free memory
//     free(mesh_value);
//     free(points);

//     return 0;
// }


//  for experiment 1

// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <omp.h>
// #include <time.h>
// // #include <bits/stdc++.h>

// #include "init.h"
// #include "utils.h"

// // Global variables
// int GRID_X, GRID_Y, NX, NY;
// long long NUM_Points;
// long long Maxiter = 10;
// double dx, dy;

// int main() {

//     int Nx_values[3] = {250, 500, 1000};
//     int Ny_values[3] = {100, 200, 400};

//     long long particle_counts[5] = {
//         100,
//         10000,
//         1000000,
//         100000000,
//         1000000000
//     };

//     for (int config = 0; config < 3; config++) {

//         NX = Nx_values[config];
//         NY = Ny_values[config];

//         GRID_X = NX + 1;
//         GRID_Y = NY + 1;

//         dx = 1.0 / NX;
//         dy = 1.0 / NY;

//         printf("\n=============================\n");
//         printf("Configuration %d\n", config + 1);
//         printf("NX=%d NY=%d\n", NX, NY);
//         printf("=============================\n");

//         for (int pc = 0; pc < 5; pc++) {

//             NUM_Points = particle_counts[pc];

//             printf("Particles: %lld\n", NUM_Points);

//             Points *points = (Points *)malloc(NUM_Points * sizeof(Points));
//             double *mesh_value = (double *)calloc(GRID_X * GRID_Y, sizeof(double));

//             double total_time = 0.0;

//             for (int iter = 0; iter < Maxiter; iter++) {

//                 initializepoints(points);

//                 memset(mesh_value, 0, GRID_X * GRID_Y * sizeof(double));

//                 double start = omp_get_wtime();

//                 interpolation(mesh_value, points);

//                 double end = omp_get_wtime();

//                 total_time += (end - start);
//             }

//             printf("Total interpolation time = %lf seconds\n", total_time);

//             free(points);
//             free(mesh_value);
//         }
//     }

//     return 0;
// }



// experiment 2
// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <omp.h>
// #include <time.h>
// // #include <bits/stdc++.h>

// #include "init.h"
// #include "utils.h"

// // Global variables
// int GRID_X, GRID_Y, NX, NY;
// long long NUM_Points;
// long long Maxiter = 10;
// double dx, dy;

// int main() {

//     int Nx_values[3] = {250, 500, 1000};
//     int Ny_values[3] = {100, 200, 400};

//     long long particle_counts=100000000;
    

//     for (int config = 0; config < 3; config++) {

//         NX = Nx_values[config];
//         NY = Ny_values[config];

//         GRID_X = NX + 1;
//         GRID_Y = NY + 1;

//         dx = 1.0 / NX;
//         dy = 1.0 / NY;

//         printf("\n=============================\n");
//         printf("Configuration %d\n", config + 1);
//         printf("NX=%d NY=%d\n", NX, NY);
//         printf("=============================\n");
//         NUM_Points = particle_counts;

//         Points *points = (Points *)malloc(NUM_Points * sizeof(Points));
//         double *mesh_value = (double *)calloc(GRID_X * GRID_Y, sizeof(double));
        

//         for (int pc = 0; pc < 1; pc++) {

            

//             printf("Particles: %lld\n", NUM_Points);

            

//             double total_time = 0.0;

//             for (int iter = 0; iter < Maxiter; iter++) {
                
//                 initializepoints(points);

//                 memset(mesh_value, 0, GRID_X * GRID_Y * sizeof(double));

//                 double start = omp_get_wtime();

//                 interpolation(mesh_value, points);

//                 double end = omp_get_wtime();


//                 total_time += (end - start);
//             }

//             printf("Total interpolation time = %lf seconds\n", total_time);

//             free(points);
//             free(mesh_value);
//         }
//     }

//     return 0;
// }

// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <time.h>
// #include <omp.h>

// #include "init.h"
// #include "utils.h"

// // Global variables
// int GRID_X, GRID_Y, NX, NY;
// int NUM_Points, Maxiter;
// double dx, dy;

// int main(int argc, char **argv) {

//     // Fixed Parameters
//     NX = 200;
//     NY = 200;
//     Maxiter = 10;
//     NUM_Points = 100000000;

//     // Since Number of points will be 1 more than number of cells
//     GRID_X = NX + 1;
//     GRID_Y = NY + 1;
//     dx = 1.0 / NX;
//     dy = 1.0 / NY;

//     // Fix Number of Threads
//     omp_set_num_threads(4);

//     // Allocate memory for grid and Points
//     double *mesh_value = (double *) calloc(GRID_X * GRID_Y, sizeof(double));
//     Points *points = (Points *) calloc(NUM_Points, sizeof(Points));

//     initializepoints(points);

//     printf("Iter\tInterp\t\tMover\t\tTotal\n");
//     for (int iter = 0; iter < Maxiter; iter++) {

//         // Interpolation timing
//         clock_t start_interp = clock();
//         interpolation(mesh_value, points);
//         clock_t end_interp = clock();

//         // Mover timing
//         clock_t start_move = clock();
//         mover_serial(points, dx, dy);
//         clock_t end_move = clock();

//         double interp_time = (double)(end_interp - start_interp) / CLOCKS_PER_SEC;
//         double move_time = (double)(end_move - start_move) / CLOCKS_PER_SEC;
//         double total = interp_time + move_time;

//         printf("%d\t%lf\t%lf\t%lf\n", iter+1, interp_time, move_time, total);
//     }

//     // Free memory
//     free(mesh_value);
//     free(points);

//     return 0;
// }
// Exp 1(opti)
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <omp.h>
#include "init.h"
#include "utils.h"

// Global variables
int GRID_X, GRID_Y, NX, NY;
long long NUM_Points;
long long Maxiter = 10;
double dx, dy;

int main()
{
    int Nx_values[3] = {250, 500, 1000};
    int Ny_values[3] = {100, 200, 400};

    long long particle_counts[5] = {
        100,
        10000,
        1000000,
        100000000,
        1000000000
    };

    for (int config = 0; config < 3; config++)
    {
        NX = Nx_values[config];
        NY = Ny_values[config];

        GRID_X = NX + 1;
        GRID_Y = NY + 1;

        dx = 1.0 / NX;
        dy = 1.0 / NY;

        printf("\n=============================\n");
        printf("Configuration %d\n", config + 1);
        printf("NX=%d NY=%d\n", NX, NY);
        printf("=============================\n");

        for (int pc = 0; pc < 5; pc++)
        {
            NUM_Points = particle_counts[pc];

            printf("Particles: %lld\n", NUM_Points);

            // Allocate once per particle case
            Points *points = new Points[NUM_Points];
            double *mesh_value = new double[GRID_X * GRID_Y];

            double total_time = 0.0;

            for (int iter = 0; iter < Maxiter; iter++)
            {
                initializepoints(points);

                std::memset(mesh_value, 0,
                            GRID_X * GRID_Y * sizeof(double));

                double start = omp_get_wtime();

                interpolation(mesh_value, points);

                double end = omp_get_wtime();

                total_time += (end - start);
            }

            printf("Total interpolation time = %lf seconds\n",
                   total_time);

            delete[] points;
            delete[] mesh_value;
        }
    }

    return 0;
}

//  for experiment 1 and 2

// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <omp.h>
// #include <time.h>
// // #include <bits/stdc++.h>

// #include "init.h"
// #include "utils.h"

// // Global variables
// int GRID_X, GRID_Y, NX, NY;
// long long NUM_Points;
// long long Maxiter = 10;
// double dx, dy;

// int main() {

//     int Nx_values[3] = {250, 500, 1000};
//     int Ny_values[3] = {100, 200, 400};

//     long long particle_counts=100000000;
    

//     for (int config = 0; config < 3; config++) {

//         NX = Nx_values[config];
//         NY = Ny_values[config];

//         GRID_X = NX + 1;
//         GRID_Y = NY + 1;

//         dx = 1.0 / NX;
//         dy = 1.0 / NY;

//         printf("\n=============================\n");
//         printf("Configuration %d\n", config + 1);
//         printf("NX=%d NY=%d\n", NX, NY);
//         printf("=============================\n");
//         NUM_Points = particle_counts;

//         Points *points = (Points *)malloc(NUM_Points * sizeof(Points));
//         double *mesh_value = (double *)calloc(GRID_X * GRID_Y, sizeof(double));
        

//         for (int pc = 0; pc < 1; pc++) {

            

//             printf("Particles: %lld\n", NUM_Points);

            

//             double total_time = 0.0;

//             for (int iter = 0; iter < Maxiter; iter++) {
                
//                 initializepoints(points);

//                 memset(mesh_value, 0, GRID_X * GRID_Y * sizeof(double));

//                 double start = omp_get_wtime();

//                 interpolation(mesh_value, points);

//                 double end = omp_get_wtime();


//                 total_time += (end - start);
//             }

//             printf("Total interpolation time = %lf seconds\n", total_time);

//             free(points);
//             free(mesh_value);
//         }
//     }

//     return 0;
// }



// Experiment 3

// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <omp.h>
// #include "init.h"
// #include "utils.h"

// // Global variables
// int GRID_X, GRID_Y, NX, NY;
// long long NUM_Points;
// long long Maxiter;
// double dx, dy;

// int main() {

//     // Fixed parameters (Experiment 03)
//     NX = 1000;
//     NY = 400;
//     NUM_Points = 14000000;   // 14 million
//     Maxiter = 10;

//     GRID_X = NX + 1;
//     GRID_Y = NY + 1;

//     dx = 1.0 / NX;
//     dy = 1.0 / NY;

//     omp_set_num_threads(4);

//     printf("NX=%d NY=%d Points=%lld Iter=%lld Threads=4\n",
//            NX, NY, NUM_Points, Maxiter);

//     double *mesh_value = (double *)calloc(GRID_X * GRID_Y, sizeof(double));
//     Points *points = (Points *)malloc(NUM_Points * sizeof(Points));

//     // Initialize ONCE
//     initializepoints(points);

//     printf("Iter\tInterpolation\tMover\t\tTotal\n");

//     for (int iter = 0; iter < Maxiter; iter++) {

//         memset(mesh_value, 0, GRID_X * GRID_Y * sizeof(double));

//         double start_interp = omp_get_wtime();
//         interpolation(mesh_value, points);
//         double end_interp = omp_get_wtime();

//         double start_move = omp_get_wtime();

//         // Choose one:
//         // mover_serial(points, dx, dy);
//         mover_parallel(points, dx, dy);

//         double end_move = omp_get_wtime();

//         double interp_time = end_interp - start_interp;
//         double move_time = end_move - start_move;
//         double total_time = interp_time + move_time;

//         printf("%d\t%lf\t%lf\t%lf\n",
//                iter + 1, interp_time, move_time, total_time);
//     }

//     free(mesh_value);
//     free(points);

//     return 0;
// }

