#include "camera.h"

Camera* init_camera(Matrix* pos, Matrix* target, Matrix* up) {
    Camera* camera = malloc(sizeof(Camera));
    camera->pos = pos;
    camera->target = target;
    camera->up = up;
    camera->xfov = M_PI / 8.0;
    camera->yfov = M_PI / 6.0;

    return camera;
}

Matrix* get_view_matrix(Camera* camera) {
    Matrix* aux = sub_mat(camera->pos, camera->target);
    Matrix* zaxis = normalize(aux);
    free(aux);

    aux = cross3D(camera->up, zaxis);
    Matrix* xaxis = normalize(aux);
    free(aux);

    Matrix* yaxis = cross3D(zaxis, xaxis);

    Matrix* view_matrix = init_matrix(4, 4);
    for (int i = 0; i < view_matrix->height - 1; i++) {
        switch (i) {
            case 0:
                aux = xaxis;
                break;
            case 1:
                aux = yaxis;
                break;
            case 2:
                aux = zaxis;
        }
        for (int j = 0; j < view_matrix->width - 1; j++) {
            view_matrix->matrix[i][j] = aux->matrix[j][0];
        }
        view_matrix->matrix[i][3] = -dot(aux, camera->pos);
    }
    view_matrix->matrix[3][0] = 0.0;
    view_matrix->matrix[3][1] = 0.0;
    view_matrix->matrix[3][2] = 0.0;
    view_matrix->matrix[3][3] = 1.0;

    free(xaxis);
    free(yaxis);
    free(zaxis);

    return view_matrix;
}

void free_camera(Camera* camera) { // might be a problem
    free(camera->pos);
    free(camera->target);
    free(camera->up);
    free(camera);
}