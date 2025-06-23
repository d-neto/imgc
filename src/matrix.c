#include <stdio.h>

#include "matrix.h"

void print_mat(matrix_t mat){
    printf("+- Matrix ");
    int max = mat.c * 9 - 9;
    for(int i = 0; i < max; ++i) putc('-', stdout);
    putc('\n', stdout);
    for(int i = 0; i < mat.r; ++i){
        for(int j = 0; j < mat.c; ++j){
            printf("%*.3f ", 8, MAT_AT(mat, i, j));
        }
        printf("\n");
    }
}

matrix_t mat(int r, int c, double * data){
    matrix_t m = create_matrix(r, c);
    for (int i = 0; i < r * c; ++i){
        m.data[i] = data[i];
    }
    return m;
}

matrix_t create_matrix(int r, int c){
    matrix_t m = {
        .c = c,
        .r = r,
        .data = NULL,
    };
    m.data = ALLOC(sizeof(*m.data)*c*r);
    for(int i = 0; i < c*r; ++i) m.data[i] = 0.0;
    return m;
}

void free_matrix(matrix_t * matrix){
    if(!matrix) return;
    matrix->c = 0;
    matrix->r = 0;
    FREE(matrix->data);
}

matrix_t gaussian_kernel(int size, double sigma){
    assert((size % 2) != 0);
    matrix_t mat = create_matrix(size, size);

    int center = size / 2;
    double sig2 = sigma * sigma;
    double sig2x = 2.0 * sig2;
    double norm = 1.0 / (2.0 * M_PI * sig2);
    double sum = 0.0;

    for(int i = 0; i < size; ++i){
        for(int j = 0; j < size; ++j){
            double dx = j - center;
            double dy = i - center;
            double val = norm * exp(-((dx*dx + dy*dy) / sig2x));
            MAT_AT(mat, i, j) = val;
            sum += val;
        }
    }

    for(int i = 0; i < size; ++i){
        for(int j = 0; j < size; ++j){
            MAT_AT(mat, i, j) /= sum;
        }
    }

    return mat;
}
matrix_t mean_kernel(int size){
    assert((size % 2) != 0);
    matrix_t mat = create_matrix(size, size);
    double value = 1.0 / (double) (size*size);
    for(int i = 0; i < size; ++i){
        for(int j = 0; j < size; ++j){
            MAT_AT(mat, i, j) = value;
        }
    }

    return mat;
}

matrix_t sobel_x_kernel_x5(){
    matrix_t mat = create_matrix(5, 5);
    long double sobel_values[] = {
        -1, -2, 0, 2, 1,   
        -1, -2, 0, 2, 1,  
        -2, -4, 0, 4, 2,  
        -1, -2, 0, 2, 1,  
        -1, -2, 0, 2, 1,      
    };
    for (int i = 0; i < 25; ++i) {
        mat.data[i] = sobel_values[i];
    }
    return mat;
}

matrix_t sobel_y_kernel_x5() {
    matrix_t mat = create_matrix(5, 5);
    long double sobel_values[] = {
        -1, -1, -2, -1, -1,
        -2, -2, -4, -2, -2, 
         2,  2,  4,  2,  2, 
         1,  1,  2,  1,  1,
         0,  0,  0,  0,  0,
    };
    for (int i = 0; i < 25; ++i) {
        mat.data[i] = sobel_values[i];
    }
    return mat;
}

matrix_t sobel_y_kernel() {
    matrix_t mat = create_matrix(3, 3);
    long double sobel_values[] = {
        -1,  0,  1,
        -2,  0,  2,
        -1,  0,  1
    };
    for (int i = 0; i < 9; ++i) {
        mat.data[i] = sobel_values[i];
    }
    return mat;
}

matrix_t sobel_x_kernel() {
    matrix_t mat = create_matrix(3, 3);
    long double sobel_values[] = {
        -1, -2, -1,
         0,  0,  0,
         1,  2,  1
    };
    for (int i = 0; i < 9; ++i) {
        mat.data[i] = sobel_values[i];
    }
    return mat;
}

matrix_t sobel_45_kernel_x5() {
    matrix_t mat = create_matrix(5, 5);
    long double sobel_values[] = {
        0,   -1,  -2,  -1,  0,
        1,    0,  -1,  -2,  -1,
        2,    1,   0,  -1,  -2,
        1,    2,   1,   0,  -1,
        0,    1,   2,   1,   0
    };
    for (int i = 0; i < 25; ++i) {
        mat.data[i] = sobel_values[i];
    }
    return mat;
}

matrix_t sobel_45_kernel() {
    matrix_t mat = create_matrix(3, 3);
    long double sobel_values[] = {
         0, -1, -2,
         1,  0, -1,
         2,  1,  0
    };
    for (int i = 0; i < 9; ++i) {
        mat.data[i] = sobel_values[i];
    }
    return mat;
}

matrix_t sobel_135_kernel_x5() {
    matrix_t mat = create_matrix(5, 5);
    long double sobel_values[] = {
         0,    1,   2,   1,   0,
        -1,    0,   1,   2,   1,
        -2,   -1,   0,   1,   2,
        -1,   -2,  -1,   0,   1,
         0,   -1,  -2,  -1,   0
    };
    for (int i = 0; i < 25; ++i) {
        mat.data[i] = sobel_values[i];
    }
    return mat;
}

matrix_t sobel_135_kernel() {
    matrix_t mat = create_matrix(3, 3);
    long double sobel_values[] = {
         2,  1,  0,
         1,  0, -1,
         0, -1, -2
    };
    for (int i = 0; i < 9; ++i) {
        mat.data[i] = sobel_values[i];
    }
    return mat;
}

matrix_t laplacian_4c_kernel() {
    matrix_t mat = create_matrix(3, 3);
    long double laplacian_values[] = {
         0, -1,  0,
        -1,  4, -1,
         0, -1,  0
    };
    for (int i = 0; i < 9; ++i) {
        mat.data[i] = laplacian_values[i];
    }
    return mat;
}

matrix_t laplacian_kernel() {
    matrix_t mat = create_matrix(3, 3);
    long double laplacian_values[] = {
        -1, -1,  -1,
        -1,  8,  -1,
        -1, -1,  -1
    };
    for (int i = 0; i < 9; ++i) {
        mat.data[i] = laplacian_values[i];
    }
    return mat;
}

matrix_t disc(int radius){
    matrix_t matrix = create_matrix(radius * 2 + 1, radius * 2 + 1);
    double r2 = radius*radius;

    int center_x = matrix.c/2;
    int center_y = matrix.r/2;

    for (int y = -radius; y <= radius; ++y) {
        for (int x = -radius; x <= radius; ++x) {
            if (x*x + y*y <= r2) {
                MAT_AT(matrix, center_x + x, center_y + y) = 1;
            }
        }
    }

    return matrix;
}