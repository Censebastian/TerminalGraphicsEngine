#include "matrixOperations.h"

Matrix* init_matrix(int height, int width) {
    Matrix* matrix = malloc(sizeof(Matrix));
    matrix->height = height;
    matrix->width = width;
    matrix->matrix = (float**)malloc(height * sizeof(float*));

    for (int i = 0; i < height; i++) {
        matrix->matrix[i] = (float*)calloc(width, sizeof(float));
    }

    if (matrix == NULL) {
        printf("WARNING: INITIALISING MATRIX FAILED\n");
    }
    return matrix;
}

Matrix* init_identity(int dimension) {
    Matrix* matrix = malloc(sizeof(Matrix));
    matrix->height = dimension;
    matrix->width = dimension;
    matrix->matrix = (float**)malloc(dimension * sizeof(float*));

    for (int i = 0; i < dimension; i++) {
        matrix->matrix[i] = (float*)calloc(dimension, sizeof(float));
        matrix->matrix[i][i] = 1.0f;
    }

    if (matrix == NULL) {
        printf("WARNING: INITIALISING IDENTITY MATRIX FAILED\n");
    }
    return matrix;
}

Matrix* multiply_mat(Matrix* a, Matrix* b) {
    if (a->width != b->height) {
        printf("ERROR: MATRIX a WIDTH DOESN'T MATCH MATRIX b HEIGHT");
        return NULL;
    }
    int n = a->width;
    Matrix* new_mat = init_matrix(n, n);

    for (int i = 0; i < a->height; i++) {
        for (int j = 0; j < b->width; j++) {
            for (int k = 0; k < n; k++) {
                new_mat->matrix[i][j] += a->matrix[i][k] * b->matrix[k][j];
            }
        }
    }

    return new_mat;
}

Matrix* add_mat(Matrix* a, Matrix* b) {
    if (a->height != b->height || a->width != b->width) {
        printf("ERROR: MATRIX a DOESN'T MATCH MATRIX b SIZE");
        return NULL;
    }
    int h = a->height;
    int w = a->width;
    Matrix* new_mat = init_matrix(h, w);

    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            new_mat->matrix[i][j] = a->matrix[i][j] + b->matrix[i][j];
        }
    }

    return new_mat;
}

Matrix* sub_mat(Matrix* a, Matrix* b) {
    if (a->height != b->height || a->width != b->width) {
        printf("ERROR: MATRIX a DOESN'T MATCH MATRIX b SIZE");
        return NULL;
    }
    int h = a->height;
    int w = a->width;
    Matrix* new_mat = init_matrix(h, w);

    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            new_mat->matrix[i][j] = a->matrix[i][j] - b->matrix[i][j];
        }
    }

    return new_mat;
}

Matrix* scale_mat(Matrix* a, float scalar) {
    int h = a->height;
    int w = a->width;
    Matrix* new_mat = init_matrix(h, w);

    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            new_mat->matrix[i][j] = a->matrix[i][j] * scalar;
        }
    }

    return new_mat;
}

Matrix* transpose(Matrix* a) {
    int h = a->width;
    int w = a->height;
    Matrix* new_mat = init_matrix(h, w);

    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            new_mat->matrix[i][j] = a->matrix[j][i];
        }
    }

    return new_mat;
}

Matrix* normalize(Matrix* a) {
    int h = a->height;
    int w = a->width;

    if (w != 1) {
        printf("ERROR: CAN'T NORMALIZE VECTOR, BAD SIZING\n");
    }

    float norm = 0;
    for (int i = 0; i < h; i++) {
        norm += a->matrix[i][0] * a->matrix[i][0];
    }
    norm = sqrt(norm);

    return scale_mat(a, 1 / norm);
}

float dot(Matrix* a, Matrix* b) {
    int ha = a->height;
    int wa = a->width;

    int hb = b->height;
    int wb = b->width;

    if (wa != 1 || wb != 1 || ha != hb) {
        printf("ERROR: CAN'T CALCULATE DOT PRODUCT, BAD SIZING\n");
        return 0.0;
    }

    float dot_product = 0;
    for (int i = 0; i < ha; i++) {
        dot_product += a->matrix[i][0] * b->matrix[i][0];
    }

    return dot_product;
}

Matrix* cross3D(Matrix* a, Matrix* b) {
    int ha = a->height;
    int wa = a->width;

    int hb = b->height;
    int wb = b->width;

    if (wa != 1 || wb != 1 || ha != 3 || ha != hb) {
        printf("ERROR: CAN'T CALCULATE CROSS PRODUCT, BAD SIZING\n");
        return NULL;
    }

    Matrix* new_mat = init_matrix(ha, wa);
    new_mat->matrix[0][0] = a->matrix[1][0] * b->matrix[2][0] - a->matrix[2][0] * b->matrix[1][0];
    new_mat->matrix[1][0] = a->matrix[2][0] * b->matrix[0][0] - a->matrix[0][0] * b->matrix[2][0];
    new_mat->matrix[2][0] = a->matrix[0][0] * b->matrix[1][0] - a->matrix[1][0] * b->matrix[0][0];

    return new_mat;
}

Matrix* get_projection_matrix(float xfov, float yfov, float znear, float zfar, float aspect_ratio) {
    Matrix* projection_matrix = init_matrix(4, 4);

    // projection_matrix->matrix[0][0] = 1 / (tan(xfov / 2.0) * aspect_ratio);
    projection_matrix->matrix[0][0] = 1 / tan(xfov / 2.0);
    projection_matrix->matrix[1][1] = 1 / tan(yfov / 2.0);
    projection_matrix->matrix[2][2] = -(znear + zfar) / (zfar - znear);
    projection_matrix->matrix[2][3] = -2 * (znear * zfar) / (zfar - znear);
    projection_matrix->matrix[3][2] = -1; // see this for bugs

    return projection_matrix;
}

void apply_mat_to_mesh(Mesh* mesh, Matrix* matrix) {
    Matrix* vector = init_matrix(4, 1);
    for (int i = 0; i < mesh->num_vertices * 3; i += 3) {
        vector->matrix[0][0] = mesh->vertices[i];
        vector->matrix[1][0] = mesh->vertices[i + 1];
        vector->matrix[2][0] = mesh->vertices[i + 2];
        vector->matrix[3][0] = 1;

        Matrix* aux = multiply_mat(matrix, vector);
        if (aux == NULL) {
            printf("ERROR: APPLYING MATRIX TO MESH FAILED\n");
            return;
        }
        mesh->vertices[i] = aux->matrix[0][0];
        mesh->vertices[i + 1] = aux->matrix[1][0];
        mesh->vertices[i + 2] = aux->matrix[2][0];

        free(aux);
    }
}

void apply_projmat_to_mesh(Mesh* mesh, Matrix* matrix) {
    Matrix* vector = init_matrix(4, 1);
    for (int i = 0; i < mesh->num_vertices * 3; i += 3) {
        vector->matrix[0][0] = mesh->vertices[i];
        vector->matrix[1][0] = mesh->vertices[i + 1];
        vector->matrix[2][0] = mesh->vertices[i + 2];
        vector->matrix[3][0] = 1;

        Matrix* aux = multiply_mat(matrix, vector);
        if (aux == NULL) {
            printf("ERROR: APPLYING MATRIX TO MESH FAILED\n");
            return;
        }

        if (aux->matrix[3][0] == 0.0) {
            aux->matrix[3][0] = 1.0;
        }

        mesh->vertices[i] = aux->matrix[0][0] / aux->matrix[3][0];
        mesh->vertices[i + 1] = aux->matrix[1][0] / aux->matrix[3][0];
        mesh->vertices[i + 2] = aux->matrix[2][0] / aux->matrix[3][0];

        free(aux);
    }
}

void rotate_mesh_OX(Mesh* mesh, float rad) {
    for (int i = 0; i < mesh->num_vertices * 3; i += 3) {
        rotate_OX(rad, &mesh->vertices[i], &mesh->vertices[i + 1], &mesh->vertices[i + 2]);
    }
}

void rotate_mesh_OY(Mesh* mesh, float rad) {
    for (int i = 0; i < mesh->num_vertices * 3; i += 3) {
        rotate_OY(rad, &mesh->vertices[i], &mesh->vertices[i + 1], &mesh->vertices[i + 2]);
    }
}

void rotate_mesh_OZ(Mesh* mesh, float rad) {
    for (int i = 0; i < mesh->num_vertices * 3; i += 3) {
        rotate_OZ(rad, &mesh->vertices[i], &mesh->vertices[i + 1], &mesh->vertices[i + 2]);
    }
}

void print_matrix(Matrix* matrix) {
    for (int i = 0; i < matrix->height; i++) {
        for (int j = 0; j < matrix->width; j++) {
            printf("%f ", matrix->matrix[i][j]);
        }
        printf("\n");
    }
}

void free_matrix(Matrix* matrix) {
    for (int i = 0; i < matrix->height; i++) {
        free(matrix->matrix[i]);
    }
    free(matrix->matrix);
}
