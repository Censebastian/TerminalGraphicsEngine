#ifndef CAMERA_H
#define CAMERA_H

#include <stdlib.h>
#include <math.h>
#include "matrixOperations.h"

typedef struct {
    Matrix* pos;
    Matrix* target;
    Matrix* up;
    float xfov;
    float yfov;
} Camera;

Camera* init_camera(Matrix* pos, Matrix* target, Matrix* up);
Matrix* get_view_matrix(Camera* camera);
void free_camera(Camera* camera);

#endif // CAMERA_H