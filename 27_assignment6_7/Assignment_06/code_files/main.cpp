// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <time.h>
// #include <omp.h>    // OpenMP header

// #include "init.h"
// #include "utils.h"

// // Global variables
// int GRID_X, GRID_Y, NX, NY;
// int NUM_Points, Maxiter;
// double dx, dy;

// int main(int argc, char **argv) {

//     if (argc != 2) {
//         printf("Usage: %s <input_file>\n", argv[0]);
//         return 1;
//     }

//     int thread_list[] = {1, 2, 4, 8, 16};
//     int num_cases = 5;
//     double times[5];

//     for (int tcase = 0; tcase < num_cases; tcase++) {

//         int threads = thread_list[tcase];
//         omp_set_num_threads(threads);   // Set OpenMP threads

//         FILE *file = fopen(argv[1], "rb");
//         if (!file) {
//             printf("Error opening input file\n");
//             exit(1);
//         }

//         // Read input parameters
//         fread(&NX, sizeof(int), 1, file);
//         fread(&NY, sizeof(int), 1, file);
//         fread(&NUM_Points, sizeof(int), 1, file);
//         fread(&Maxiter, sizeof(int), 1, file);

//         GRID_X = NX + 1;
//         GRID_Y = NY + 1;
//         dx = 1.0 / NX;
//         dy = 1.0 / NY;

//         double *mesh_value = (double *) calloc(GRID_X * GRID_Y, sizeof(double));
//         Points *points = (Points *) calloc(NUM_Points, sizeof(Points));

//         double total_time = 0.0;

//         for (int iter = 0; iter < Maxiter; iter++) {
//             memset(mesh_value, 0, GRID_X * GRID_Y * sizeof(double));
//             read_points(file, points);

//             double start = omp_get_wtime();
//             interpolation(mesh_value, points);   // Must be parallelized in utils.cpp
//             double end = omp_get_wtime();
//             total_time += (end - start);

//             // Save Mesh.out for reference on first thread, first iteration
//             if (threads == 1 && iter == 0) {
//                 save_mesh(mesh_value);
//             }
//         }

//         printf("Threads = %d | Time = %lf sec\n", threads, total_time);
//         times[tcase] = total_time;

//         free(mesh_value);
//         free(points);
//         fclose(file);
//     }

//     // =========================
//     // Generate Python Plot Script
//     // =========================
//     FILE *py = fopen("plot.py", "w");

//     fprintf(py, "import matplotlib.pyplot as plt\n\n");

//     fprintf(py, "threads = [1,2,4,8,16]\n");

//     fprintf(py, "times = [");
//     for (int i = 0; i < num_cases; i++) {
//         fprintf(py, "%lf", times[i]);
//         if (i != num_cases - 1) fprintf(py, ",");
//     }
//     fprintf(py, "]\n");

//     fprintf(py, "speedup = [times[0]/t for t in times]\n");
//     fprintf(py, "efficiency = [speedup[i]/threads[i] for i in range(len(threads))]\n\n");

//     // Time vs Threads
//     fprintf(py, "plt.figure(figsize=(6,4))\n");
//     fprintf(py, "plt.plot(threads, times, marker='o', linewidth=2)\n");
//     fprintf(py, "plt.xlabel('Number of Threads')\n");
//     fprintf(py, "plt.ylabel('Execution Time (s)')\n");
//     fprintf(py, "plt.title('Time vs Threads (%dx%d)')\n", NX, NY);
//     fprintf(py, "plt.grid(True)\n");
//     fprintf(py, "plt.tight_layout()\n");
//     fprintf(py, "plt.savefig('time_vs_threads_%dx%d.png', dpi=300)\n\n", NX, NY);

//     // Speedup vs Threads
//     fprintf(py, "plt.figure(figsize=(6,4))\n");
//     fprintf(py, "plt.plot(threads, speedup, marker='o', linewidth=2)\n");
//     fprintf(py, "plt.xlabel('Number of Threads')\n");
//     fprintf(py, "plt.ylabel('Speedup')\n");
//     fprintf(py, "plt.title('Speedup vs Threads (%dx%d)')\n", NX, NY);
//     fprintf(py, "plt.grid(True)\n");
//     fprintf(py, "plt.tight_layout()\n");
//     fprintf(py, "plt.savefig('speedup_vs_threads_%dx%d.png', dpi=300)\n\n", NX, NY);

//     // Efficiency vs Threads
//     fprintf(py, "plt.figure(figsize=(6,4))\n");
//     fprintf(py, "plt.plot(threads, efficiency, marker='o', linewidth=2)\n");
//     fprintf(py, "plt.xlabel('Number of Threads')\n");
//     fprintf(py, "plt.ylabel('Efficiency')\n");
//     fprintf(py, "plt.title('Efficiency vs Threads (%dx%d)')\n", NX, NY);
//     fprintf(py, "plt.grid(True)\n");
//     fprintf(py, "plt.tight_layout()\n");
//     fprintf(py, "plt.savefig('efficiency_vs_threads_%dx%d.png', dpi=300)\n\n", NX, NY);

//     fprintf(py, "plt.show()\n");

//     fclose(py);

//     // Run Python script automatically
//     system("python3 plot.py");

//     return 0;
// }
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <omp.h>
#include "init.h"
#include "utils.h"

// Global variables
int GRID_X, GRID_Y, NX, NY;
int NUM_Points, Maxiter;
double dx, dy;

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    int thread_list[] = {1, 2, 4, 8, 16};
    int num_cases = 5;
    double times[5];

    // Open file once to read parameters and pre‑load all points
    FILE *file = fopen(argv[1], "rb");
    if (!file) {
        printf("Error opening input file\n");
        exit(1);
    }

    fread(&NX, sizeof(int), 1, file);
    fread(&NY, sizeof(int), 1, file);
    fread(&NUM_Points, sizeof(int), 1, file);
    fread(&Maxiter, sizeof(int), 1, file);

    GRID_X = NX + 1;
    GRID_Y = NY + 1;
    dx = 1.0 / NX;
    dy = 1.0 / NY;

    // Pre‑read all points for all iterations into a flat array
    // Layout: [iter][point][x/y]
    double *all_points = (double*)malloc(Maxiter * NUM_Points * 2 * sizeof(double));
    if (!all_points) {
        printf("Memory allocation failed for points\n");
        exit(1);
    }
    size_t points_read = fread(all_points, sizeof(double), Maxiter * NUM_Points * 2, file);
    if (points_read != (size_t)(Maxiter * NUM_Points * 2)) {
        printf("Error reading points from file\n");
        exit(1);
    }
    fclose(file);

    for (int tcase = 0; tcase < num_cases; tcase++) {
        int threads = thread_list[tcase];
        omp_set_num_threads(threads);

        double *mesh_value = (double*)calloc(GRID_X * GRID_Y, sizeof(double));
        double total_time = 0.0;

        for (int iter = 0; iter < Maxiter; iter++) {
            // Reset mesh
            memset(mesh_value, 0, GRID_X * GRID_Y * sizeof(double));

            // Build Points array for this iteration from the pre‑loaded data
            Points *points = (Points*)malloc(NUM_Points * sizeof(Points));
            double *src = all_points + iter * NUM_Points * 2;
            for (int i = 0; i < NUM_Points; i++) {
                points[i].x = src[2*i];
                points[i].y = src[2*i + 1];
            }

            double start = omp_get_wtime();
            interpolation(mesh_value, points);
            double end = omp_get_wtime();
            total_time += (end - start);

            free(points);

            // Save Mesh.out for reference on first thread, first iteration
            if (threads == 1 && iter == 0) {
                save_mesh(mesh_value);
            }
        }

        printf("Threads = %d | Time = %lf sec\n", threads, total_time);
        times[tcase] = total_time;

        free(mesh_value);
    }

    free(all_points);

    // Generate Python plot script (same as before)
    FILE *py = fopen("plot.py", "w");
    fprintf(py, "import matplotlib.pyplot as plt\n\n");
    fprintf(py, "threads = [1,2,4,8,16]\n");
    fprintf(py, "times = [");
    for (int i = 0; i < num_cases; i++) {
        fprintf(py, "%lf", times[i]);
        if (i != num_cases - 1) fprintf(py, ",");
    }
    fprintf(py, "]\n");
    fprintf(py, "speedup = [times[0]/t for t in times]\n");
    fprintf(py, "efficiency = [speedup[i]/threads[i] for i in range(len(threads))]\n\n");
    fprintf(py, "plt.figure(figsize=(6,4))\n");
    fprintf(py, "plt.plot(threads, times, marker='o', linewidth=2)\n");
    fprintf(py, "plt.xlabel('Number of Threads')\n");
    fprintf(py, "plt.ylabel('Execution Time (s)')\n");
    fprintf(py, "plt.title('Time vs Threads (%dx%d)')\n", NX, NY);
    fprintf(py, "plt.grid(True)\n");
    fprintf(py, "plt.tight_layout()\n");
    fprintf(py, "plt.savefig('time_vs_threads_%dx%d.png', dpi=300)\n\n", NX, NY);
    fprintf(py, "plt.figure(figsize=(6,4))\n");
    fprintf(py, "plt.plot(threads, speedup, marker='o', linewidth=2)\n");
    fprintf(py, "plt.xlabel('Number of Threads')\n");
    fprintf(py, "plt.ylabel('Speedup')\n");
    fprintf(py, "plt.title('Speedup vs Threads (%dx%d)')\n", NX, NY);
    fprintf(py, "plt.grid(True)\n");
    fprintf(py, "plt.tight_layout()\n");
    fprintf(py, "plt.savefig('speedup_vs_threads_%dx%d.png', dpi=300)\n\n", NX, NY);
    fprintf(py, "plt.figure(figsize=(6,4))\n");
    fprintf(py, "plt.plot(threads, efficiency, marker='o', linewidth=2)\n");
    fprintf(py, "plt.xlabel('Number of Threads')\n");
    fprintf(py, "plt.ylabel('Efficiency')\n");
    fprintf(py, "plt.title('Efficiency vs Threads (%dx%d)')\n", NX, NY);
    fprintf(py, "plt.grid(True)\n");
    fprintf(py, "plt.tight_layout()\n");
    fprintf(py, "plt.savefig('efficiency_vs_threads_%dx%d.png', dpi=300)\n\n", NX, NY);
    fprintf(py, "plt.show()\n");
    fclose(py);

    system("python3 plot.py");
    return 0;
}