#include "helpers.h"

void print_cube_coords(float CUBE_X, float CUBE_Y, float CUBE_Z) {
    printf("X: %.2f Y: %.2f Z: %.2f\n", CUBE_X, CUBE_Y, CUBE_Z);
}

void transform(float x, float y, float z,
            float *x_obj, float *y_obj, float *z_obj) {
    *x_obj += x;
    *y_obj += y;
    *z_obj += z;
}

void rotate_OX(float rad,
                float *x_obj, float *y_obj, float *z_obj) {
    float y = *y_obj, z = *z_obj;
    *y_obj = (float)(cos(rad) * (y) - sin(rad) * (z));
    *z_obj = (float)(sin(rad) * (y) + cos(rad) * (z));
}

void rotate_OY(float rad,
                float *x_obj, float *y_obj, float *z_obj) {
    float x = *x_obj, z = *z_obj;
    *x_obj = (float)(cos(rad) * (x) + sin(rad) * (z));
    *z_obj = (float)(-sin(rad) * (x) + cos(rad) * (z));
}

void rotate_OZ(float rad,
                float *x_obj, float *y_obj, float *z_obj) {
    float x = *x_obj, y = *y_obj;
    *x_obj = (float)(cos(rad) * (x) - sin(rad) * (y));
    *y_obj = (float)(sin(rad) * (x) + cos(rad) * (y));
}

void scale(float scalar_x, float scalar_y, float scalar_z,
            float *x_obj, float *y_obj, float *z_obj) {
    *x_obj = *x_obj * scalar_x;
    *y_obj = *y_obj * scalar_y;
    *z_obj = *z_obj * scalar_z;
}

void copy_deep_mesh(Mesh* dest, Mesh* src) {
    dest->x = src->x;
    dest->y = src->y;
    dest->z = src->z;
    dest->num_vertices = src->num_vertices;
    dest->num_indices = src->num_indices;

    dest->vertices = malloc(3 * src->num_vertices * sizeof(float));
    dest->indices = malloc(src->num_indices * sizeof(int));
    memcpy(dest->vertices, src->vertices, 3 * dest->num_vertices * sizeof(float));
    memcpy(dest->indices, src->indices, dest->num_indices * sizeof(int));
}

void free_mesh(Mesh *a) {
    free(a->vertices);
    free(a->indices);
    free(a);
}

Mesh* generate_cube_mesh(float scale, float start_x, float start_y, float start_z) {
    Mesh* cube = malloc(sizeof(Mesh));
    cube->x = start_x;
    cube->y = start_y;
    cube->z = start_z;

    float vertices[] = {
        cube->x + 0.0f, cube->y + 0.0f, cube->z + 0.0f,
        cube->x + 1.0f, cube->y + 0.0f, cube->z + 0.0f,
        cube->x + 1.0f, cube->y + 1.0f, cube->z + 0.0f,
        cube->x + 0.0f, cube->y + 1.0f, cube->z + 0.0f,
        cube->x + 0.0f, cube->y + 0.0f, cube->z + 1.0f,
        cube->x + 1.0f, cube->y + 0.0f, cube->z + 1.0f,
        cube->x + 1.0f, cube->y + 1.0f, cube->z + 1.0f,
        cube->x + 0.0f, cube->y + 1.0f, cube->z + 1.0f
    };

    int indices[] = {
        // 0, 1, 2, 0, 2, 3,
        // 0, 5, 1, 0, 4, 5,
        // 0, 3, 7, 0, 7, 4,
        // 2, 1, 5, 2, 5, 6,
        // 2, 7, 3, 2, 6, 7,
        // 4, 6, 5, 4, 7, 6
        0, 1, 2,  0, 2, 3,
        4, 6, 5,  4, 7, 6,
        4, 0, 3,  4, 3, 7,
        1, 5, 6,  1, 6, 2,
        3, 2, 6,  3, 6, 7,
        4, 5, 1,  4, 1, 0
    };

    int num_vertices = 3 * 8 * sizeof(float);
    cube->num_vertices = 8;
    cube->vertices = malloc(num_vertices);
    for (int i = 0; i < num_vertices / sizeof(float); i++) {
        cube->vertices[i] = vertices[i] * scale;
    }

    int num_indices = 3 * 2 * 6 * sizeof(int);
    cube->num_indices = 3 * 2 * 6;
    cube->indices = malloc(num_indices);
    for (int i = 0; i < num_indices / sizeof(int); i++) {
        cube->indices[i] = indices[i];
    }

    return cube;
}
