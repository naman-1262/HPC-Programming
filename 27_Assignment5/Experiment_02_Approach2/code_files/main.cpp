#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <omp.h>
#include <time.h>
#include "init.h"
#include "utils.h"

using namespace std;

// Globals
int GRID_X, GRID_Y, NX, NY;
long long NUM_Points;
int Maxiter;
double dx, dy;

int main() {

    srand(time(0));

    Maxiter = 10;
    NUM_Points = 14000000; // 14M particles

    vector<pair<int,int>> grids = {
        {250,100},
        {500,200},
        {1000,400}
    };

    vector<int> threads = {2,4,8,16};

    for (auto g : grids) {

        NX = g.first;
        NY = g.second;

        GRID_X = NX;
        GRID_Y = NY;

        dx = 1.0 / NX;
        dy = 1.0 / NY;

        string filename = "scaling_" + to_string(NX) + "x" + to_string(NY) + "_immediate.csv";
        ofstream file(filename);

        file << "Threads,InterpTime,MoverTime,TotalTime,Speedup\n";

        cout << "\nGRID " << NX << " x " << NY << " (Immediate Optimized)\n";

        double baseline_time = 0;

        for (int t : threads) {

            omp_set_num_threads(t);

            double *mesh_value = (double*)calloc(GRID_X * GRID_Y, sizeof(double));
            Points *points = (Points*)malloc(NUM_Points * sizeof(Points));

            initializepoints(points);

            double interp_time = 0.0;
            double mover_time = 0.0;

            for (int iter = 0; iter < Maxiter; iter++) {

                double s1 = omp_get_wtime();
                interpolation_optimized(mesh_value, points);
                double e1 = omp_get_wtime();

                double s2 = omp_get_wtime();
                mover_immediate_parallel(points, dx, dy);
                double e2 = omp_get_wtime();

                interp_time += (e1 - s1);
                mover_time += (e2 - s2);

                cout << "Threads " << t
                     << " | Iter " << iter+1
                     << " | Interp: " << (e1 - s1)
                     << " | Mover: " << (e2 - s2)
                     << endl;
            }

            double total_time = interp_time + mover_time;

            if (t == 2) baseline_time = total_time;

            double speedup = baseline_time / total_time;

            file << t << ","
                 << interp_time << ","
                 << mover_time << ","
                 << total_time << ","
                 << speedup << "\n";

            cout << "Threads " << t
                 << " TOTAL: " << total_time
                 << " | Speedup: " << speedup << endl;

            free(mesh_value);
            free(points);
        }

        file.close();
    }

    return 0;
}