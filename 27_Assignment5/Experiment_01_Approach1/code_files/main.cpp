#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <time.h>
#include <omp.h>
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

        string filename = "grid_" + to_string(NX) + "x" + to_string(NY) + "_deferred.csv";
        ofstream file(filename);

        file << "Particles,InterpTime,MoverTime,TotalTime,PPC,TimePerParticle\n";

        cout << "\nGRID " << NX << "x" << NY << " (Deferred)" << endl;

        for (int p = 0; p < particles.size(); p++) {

            NUM_Points = particles[p];

            double *mesh_value = (double *)calloc(GRID_X * GRID_Y, sizeof(double));
            Points *points = (Points *)malloc(NUM_Points * sizeof(Points));

            initializepoints(points);

            double interp_time = 0.0;
            double mover_time = 0.0;

            for (int iter = 0; iter < Maxiter; iter++) {

                double s1 = omp_get_wtime();
                interpolation(mesh_value, points);
                double e1 = omp_get_wtime();

                double s2 = omp_get_wtime();
                mover_deferred(points, dx, dy);
                double e2 = omp_get_wtime();

                interp_time += (e1 - s1);
                mover_time += (e2 - s2);
            }

            double total_time = interp_time + mover_time;

            double PPC = (double)NUM_Points / (NX * NY);
            double time_per_particle = total_time / NUM_Points;

            file << NUM_Points << ","
                 << interp_time << ","
                 << mover_time << ","
                 << total_time << ","
                 << PPC << ","
                 << time_per_particle << "\n";

            cout << "Done: " << NUM_Points << endl;

            free(mesh_value);
            free(points);
        }

        file.close();
    }

    return 0;
}