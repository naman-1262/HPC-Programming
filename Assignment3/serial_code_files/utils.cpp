#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

void interpolation(double *mesh_value, Points *points)
{
    memset(mesh_value, 0, GRID_X * GRID_Y * sizeof(double));

    for (int p = 0; p < NUM_Points; p++)
    {
        double x = points[p].x;
        double y = points[p].y;

        int i = (int)(x / dx);
        int j = (int)(y / dy);

        if (i >= NX) i = NX - 1;
        if (j >= NY) j = NY - 1;

        double Xi = i * dx;
        double Yj = j * dy;

        double ddx = (x - Xi);
        double ddy = (y - Yj);

        double w_ij     = (dx- ddx) * (dy - ddy);
        double w_i1j    = ddx * (dy - ddy);
        double w_ij1    = (dx - ddx) * ddy;
        double w_i1j1   = ddx * ddy;

        double f = 1.0;

        int idx_ij   = j * GRID_X + i;
        int idx_i1j  = j * GRID_X + (i + 1);
        int idx_ij1  = (j + 1) * GRID_X + i;
        int idx_i1j1 = (j + 1) * GRID_X + (i + 1);

        mesh_value[idx_ij]   += w_ij   * f;
        mesh_value[idx_i1j]  += w_i1j  * f;
        mesh_value[idx_ij1]  += w_ij1  * f;
        mesh_value[idx_i1j1] += w_i1j1 * f;
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
