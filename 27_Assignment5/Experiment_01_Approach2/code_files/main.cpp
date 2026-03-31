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

// Global variables
int GRID_X, GRID_Y, NX, NY;
long long NUM_Points;
int Maxiter;
double dx, dy;

int main() {

    srand(time(0));

    Maxiter = 10;

    vector<pair<int,int>> grids = {
        {250,100},
        {500,200},
        {1000,400}
    };

    vector<long long> particles = {
        100LL, 10000LL, 1000000LL, 100000000LL, 1000000000LL
    };

    for (int g = 0; g < grids.size(); g++) {

        NX = grids[g].first;
        NY = grids[g].second;

        GRID_X = NX;
        GRID_Y = NY;

        dx = 1.0 / NX;
        dy = 1.0 / NY;

        string filename = "grid_" + to_string(NX) + "x" + to_string(NY) + "_immediate.csv";
        ofstream file(filename);

        file << "Iteration,Particles,InterpTime,MoverTime,TotalTime\n";

        cout << "\n=============================\n";
        cout << "GRID " << NX << " x " << NY << " (Immediate)\n";
        cout << "=============================\n";

        for (int p = 0; p < particles.size(); p++) {

            NUM_Points = particles[p];

            double *mesh_value = (double *)calloc(GRID_X * GRID_Y, sizeof(double));
            Points *points = (Points *)malloc(NUM_Points * sizeof(Points));

            initializepoints(points);

            double total_interp = 0.0;
            double total_mover = 0.0;

            cout << "\nParticles: " << NUM_Points << endl;

            for (int iter = 0; iter < Maxiter; iter++) {

                double s1 = omp_get_wtime();
                interpolation(mesh_value, points);
                double e1 = omp_get_wtime();

                double s2 = omp_get_wtime();
                mover_immediate(points, dx, dy);
                double e2 = omp_get_wtime();

                double interp_time = (e1 - s1);
                double mover_time = (e2 - s2);
                double total_time = interp_time + mover_time;

                total_interp += interp_time;
                total_mover += mover_time;

                // PRINT
                cout << "Iteration " << iter + 1
                     << " | Particles " << NUM_Points
                     << " | Interp: " << interp_time
                     << " sec | Mover: " << mover_time
                     << " sec | Total: " << total_time
                     << " sec" << endl;

                // CSV
                file << iter + 1 << ","
                     << NUM_Points << ","
                     << interp_time << ","
                     << mover_time << ","
                     << total_time << "\n";
            }

            cout << "TOTAL → Interp: " << total_interp
                 << " | Mover: " << total_mover
                 << " | Total: " << (total_interp + total_mover)
                 << endl;

            free(mesh_value);
            free(points);
        }

        file.close();
    }

    return 0;
}