#ifndef __MATRIX_IMAGEC_H__
#define __MATRIX_IMAGEC_H__

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include "alloc.h"
#include "geom.h"

#define MAT_AT(m, i, j) ((m).data[(m).offset + (i)*(m).stride + (j)*(m).step])
#define M_PI 3.14159265358979323846

#define MAT(_rows, _cols, ...) (mat_t) { .r = _rows, .c = _cols, .stride = _cols, .offset = 0, .step = 1, .data = (double []) {__VA_ARGS__} }

typedef struct matrix mat_t;
struct matrix {
    int r;
    int c;
    int stride;
    int offset;
    int step;
    double * data;
};

void mat_print(mat_t mat);
void mat_free(mat_t * matrix);
mat_t mat(int r, int c);

mat_t mat_randd(int r, int c);
mat_t mat_fill(mat_t _mat, double val);
void mat_set(mat_t matrix, double val);

mat_t mat_create(int r, int c);
mat_t gaussian_kernel(int size, double sigma);
mat_t mean_kernel(int size);
mat_t sobel_x_kernel();
mat_t sobel_y_kernel();
mat_t sobel_45_kernel();
mat_t sobel_135_kernel();
mat_t sobel_x_kernel_x5();
mat_t sobel_y_kernel_x5();
mat_t sobel_45_kernel_x5();
mat_t sobel_135_kernel_x5();
mat_t laplacian_kernel();
mat_t laplacian_4c_kernel();
mat_t disc(int radius);

mat_t dot(mat_t m1, mat_t m2);
mat_t dot_out(mat_t dest, mat_t m1, mat_t m2);

mat_t mat_normalize(mat_t mat);

mat_t mat_slice(mat_t source, int r, int c, int offset, int step);
#endif /* __MATRIX_IMAGEC_H__ */