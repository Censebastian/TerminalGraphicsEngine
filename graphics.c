#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <time.h>
#include "camera.h"
#include "matrixOperations.h"
#include "helpers.h"

// #define ASPECT_RATIO 4/3
#define RENDER_RATE 1000.0 / 60.0

int SCREEN_HEIGHT;
int SCREEN_WIDTH;
float ASPECT_RATIO;

void compute_viewport_factors(float* sx, float* sy, float* tx, float* ty) {
    float yw_min = -1, yw_max = 1, xw_min = yw_min * ASPECT_RATIO, xw_max = yw_max * ASPECT_RATIO;
    float xv_min = 0, xv_max = SCREEN_WIDTH, yv_min = 0, yv_max = SCREEN_HEIGHT;

    (*sx) = (xv_max - xv_min) / (xw_max - xw_min);
    (*sy) = (yv_max - yv_min) / (yw_max - yw_min);
    (*tx) = xv_min - *sx * xw_min;
    (*ty) = yv_min - *sy * yw_min;
}

void to_viewport(Mesh* mesh, float sx, float sy, float tx, float ty) {
    for (int i = 0; i < mesh->num_vertices * 3; i += 3) {
        mesh->vertices[i] = mesh->vertices[i] * sx + tx;
        mesh->vertices[i + 1] = mesh->vertices[i + 1] * sy + ty;
    }
}

void bresenham(char** screen, int x0, int y0, int x1, int y1) {
    int dx = abs(x1 - x0), sx = (x0 < x1) ? 1 : -1;
    int dy = -abs(y1 - y0), sy = (y0 < y1) ? 1 : -1; 
    int err = dx + dy;

    while(1) {
        if (x0 >= 0 && x0 < SCREEN_WIDTH && y0 >= 0 && y0 < SCREEN_HEIGHT) {
            screen[y0][x0] = '$';
        }

        if (x0 == x1 && y0 == y1) {
            break;
        }

        int e2 = 2 * err;
        if (e2 >= dy) {
            err += dy;
            x0 += sx; 
        }
        if (e2 <= dx) {
            err += dx;
            y0 += sy;
        }
    }
}

void draw_wire_frame(char** screen, Mesh* mesh) {
    for (int i = 0; i < mesh->num_indices; i += 3) {
        for (int j = 0; j < 3; j++) {
            int x0 = (int)mesh->vertices[mesh->indices[i + j] * 3];
            int y0 = (int)mesh->vertices[mesh->indices[i + j] * 3 + 1];

            int x1 = (int)mesh->vertices[mesh->indices[i + (j + 1) % 3] * 3];
            int y1 = (int)mesh->vertices[mesh->indices[i + (j + 1) % 3] * 3 + 1];
            bresenham(screen, x0, y0, x1, y1);
        }
    }
}

void clear_screen(char** screen) {
    for (int h = 0; h < SCREEN_HEIGHT; h++) {
        for (int w = 0; w < SCREEN_WIDTH; w++) {
            screen[h][w] = ' ';
        }
    }
}

void render_screen(char** screen) {
    printf("\033[H");
    fflush(stdout);
    for (int h = SCREEN_HEIGHT - 1; h >= 0; h--) {
        for (int w = 0; w < SCREEN_WIDTH; w++) {
            putchar(screen[h][w]);
        }
        putchar('\n');
    }
}

double get_time_elapsed(struct timespec start, struct timespec end) {
    double s = (double)(end.tv_sec - start.tv_sec);
    double ns = (double)(end.tv_nsec - start.tv_nsec);

    return (s * 1000.0) + (ns / 1000000.0);
}

void print_origin(Mesh* mesh) {
    printf("%f %f %f\n", mesh->vertices[3], mesh->vertices[4], mesh->vertices[5]);
}

int main() {
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1) {
        printf("ERROR: IOCTL\n");
        return 1;
    }

    SCREEN_HEIGHT = w.ws_row;
    SCREEN_WIDTH = w.ws_col;
    ASPECT_RATIO = (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT;

    char** screen = malloc(SCREEN_HEIGHT * sizeof(char *));
    for (int i = 0; i < SCREEN_HEIGHT; i++) {
        screen[i] = malloc(SCREEN_WIDTH * sizeof(char));
    }

    Matrix* camera_pos = init_matrix(3, 1);
    camera_pos->matrix[0][0] = 15.0;
    camera_pos->matrix[1][0] = 7.0;
    camera_pos->matrix[2][0] = 0.0;

    Matrix* camera_target = init_matrix(3, 1);
    camera_target->matrix[0][0] = 0.0;
    camera_target->matrix[1][0] = 0.0;
    camera_target->matrix[2][0] = 0.0;

    Matrix* camera_up = init_matrix(3, 1);
    camera_up->matrix[0][0] = 0.0;
    camera_up->matrix[1][0] = 1.0;
    camera_up->matrix[2][0] = 0.0;

    Camera* camera = init_camera(camera_pos, camera_target, normalize(camera_up));
    Matrix* view_matrix = get_view_matrix(camera);
    Matrix* projection_matrix = get_projection_matrix(camera->xfov, camera->yfov, 1, 1024, ASPECT_RATIO);
    Mesh* cube = generate_cube_mesh(3.0, 0.0, 0.0, 0.0);

    float sx, sy, tx, ty;
    compute_viewport_factors(&sx, &sy, &tx, &ty);

    struct timespec last, curr;
    clock_gettime(CLOCK_MONOTONIC, &last);

    float xorientation = 0.0, yorientation = 0.0;
    float omega = 0.001;

    while (1) {
        clock_gettime(CLOCK_MONOTONIC, &curr);
        float delta_time = get_time_elapsed(last, curr);

        if (delta_time > RENDER_RATE) {
            last = curr;
            // compute model matrix <- make translation, rotation and scaling functions
            Mesh* pipeline_mesh = malloc(sizeof(Mesh));
            copy_deep_mesh(pipeline_mesh, cube);
            yorientation += omega * delta_time;
            rotate_mesh_OY(pipeline_mesh, yorientation);
            // print_origin(pipeline_mesh);

            // apply view and projection matrices
            apply_mat_to_mesh(pipeline_mesh, view_matrix);
            apply_projmat_to_mesh(pipeline_mesh, projection_matrix);

            // convert to screen coordinates
            to_viewport(pipeline_mesh, sx, sy, tx, ty);

            clear_screen(screen);
            draw_wire_frame(screen, pipeline_mesh);
            render_screen(screen);

            free_mesh(pipeline_mesh);
        }
    }

    free_camera(camera);
    free(camera_up);
    free(view_matrix);
    free(projection_matrix);
    free(cube);
}