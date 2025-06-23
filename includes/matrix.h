#ifndef __MATRIX_IMAGEC_H__
#define __MATRIX_IMAGEC_H__

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include "alloc.h"
#include "geom.h"

#define MAT_AT(mtx, i, j) ((mtx).data[((mtx).c * (i) + (j))])
#define M_PI 3.14159265358979323846

#define MAT(rc, ...) ({ vec2_t ms = vec2 rc; mat(ms.x, ms.y, ({ (double []) {__VA_ARGS__}; })); })
#define S_MAT(rc, ...) ({ vec2_t ms = vec2 rc; matrix_t matrix = { .r = ms.x, .c = ms.y, .data = { (double []) {__VA_ARGS__} } }; matrix; })

typedef struct matrix matrix_t;
typedef struct matrix {
    int r;
    int c;
    double * data;
} matrix;

void print_mat(matrix_t mat);
void free_matrix(matrix_t * matrix);
matrix_t mat(int r, int c, double * data);
matrix_t create_matrix(int r, int c);
matrix_t gaussian_kernel(int size, double sigma);
matrix_t mean_kernel(int size);
matrix_t sobel_x_kernel();
matrix_t sobel_y_kernel();
matrix_t sobel_45_kernel();
matrix_t sobel_135_kernel();
matrix_t sobel_x_kernel_x5();
matrix_t sobel_y_kernel_x5();
matrix_t sobel_45_kernel_x5();
matrix_t sobel_135_kernel_x5();
matrix_t laplacian_kernel();
matrix_t laplacian_4c_kernel();
matrix_t disc(int radius);

#endif /* __MATRIX_IMAGEC_H__ */