#ifndef HELPERS_H
#define HELPERS_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

typedef struct {
    float x, y, z;
    float* vertices;
    int* indices;
    int num_vertices;
    int num_indices;
} Mesh;

void print_cube_coords(float CUBE_X, float CUBE_Y, float CUBE_Z);
void transform(float x, float y, float z, float *x_obj, float *y_obj, float *z_obj);
void rotate_OX(float rad, float *x_obj, float *y_obj, float *z_obj);
void rotate_OY(float rad, float *x_obj, float *y_obj, float *z_obj);
void rotate_OZ(float rad, float *x_obj, float *y_obj, float *z_obj);
void scale(float scalar_x, float scalar_y, float scalar_z, float *x_obj, float *y_obj, float *z_obj);
void copy_deep_mesh(Mesh* dest, Mesh* src);
void free_mesh(Mesh *a);
Mesh* generate_cube_mesh(float scale, float start_x, float start_y, float start_z);

#endif // HELPERS_H