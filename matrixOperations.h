#ifndef MATRIOPERATIONS_H
#define MATRIOPERATIONS_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "helpers.h"

typedef struct {
    int height;
    int width;
    float **matrix;
} Matrix;

Matrix* init_matrix(int height, int width);
Matrix* init_identity(int dimension);
Matrix* multiply_mat(Matrix* a, Matrix* b);
Matrix* add_mat(Matrix* a, Matrix* b);
Matrix* sub_mat(Matrix* a, Matrix* b);
Matrix* scale_mat(Matrix* a, float scalar);
Matrix* transpose(Matrix* a);
Matrix* normalize(Matrix* a);
Matrix* cross3D(Matrix* a, Matrix* b);
Matrix* get_projection_matrix(float xfov, float yfov, float znear, float zfar, float aspect_ratio);
float dot(Matrix* a, Matrix* b);
void apply_mat_to_mesh(Mesh* mesh, Matrix* matrix);
void apply_projmat_to_mesh(Mesh* mesh, Matrix* matrix);
void rotate_mesh_OX(Mesh* mesh, float rad);
void rotate_mesh_OY(Mesh* mesh, float rad);
void rotate_mesh_OZ(Mesh* mesh, float rad);

void print_matrix(Matrix* matrix);
void free_matrix(Matrix* matrix);

#endif // MATRIXOPERATIONS_H