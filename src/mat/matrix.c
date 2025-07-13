#include <stdio.h>
#include "matrix.h"

static double randd(){
    return (double) rand() / (double) RAND_MAX;
}

void mat_print(mat_t mat){
    printf("+- Matrix ");
    int max = mat.c * 9 - 1;
    for (int i = 0; i < max; ++i) putc('-', stdout);
    putc('\n', stdout);
    for (int i = 0; i < mat.r; ++i) {
        for (int j = 0; j < mat.c; ++j) {
            printf("%*.3f ", 8, MAT_AT(mat, i, j));
        }
        putc('\n', stdout);
    }
}

mat_t mat(int r, int c){
    mat_t m = mat_create(r, c);
    return m;
}

mat_t mat_randd(int r, int c){
    mat_t m = mat_create(r, c);
    int size = r*c;
    for(int i = 0; i < size; ++i){
        m.data[i] = randd();
    }
    return m;
}

mat_t mat_fill(mat_t _mat, double v){
    assert(_mat.data);
    int size = _mat.r*_mat.c;
    for(int i = 0; i < size; ++i){
        _mat.data[i] = v;
    }
    return _mat;
}

void mat_set(mat_t matrix, double val){
    for(int i = 0; i < matrix.r; ++i)
        for(int j = 0; j < matrix.r; ++j)
            MAT_AT(matrix, i, j) = val;
}

mat_t mat_normalize(mat_t mat) {
    double min = MAT_AT(mat, 0, 0);
    double max = min;
    for (int i = 0; i < mat.r; ++i) {
        for (int j = 0; j < mat.c; ++j) {
            double val = MAT_AT(mat, i, j);
            if (val < min) min = val;
            if (val > max) max = val;
        }
    }
    double range = max - min;
    if (range == 0.0) range = 1.0;
    for (int i = 0; i < mat.r; ++i) {
        for (int j = 0; j < mat.c; ++j) {
            double *ref = &MAT_AT(mat, i, j);
            *ref = (*ref - min) / range;
        }
    }
    return mat;
}

mat_t mat_slice(mat_t source, int r, int c, int offset, int step) {
    return (mat_t){
        .r = r,
        .c = c,
        .stride = source.stride,
        .offset = source.offset + offset,
        .step = step,
        .data = source.data
    };
}

mat_t mat_create(int r, int c){
    mat_t m = {
        .offset = 0,
        .stride = c,
        .c = c,
        .r = r,
        .step = 1,
        .data = NULL,
    };
    m.data = ALLOC(sizeof(*m.data)*c*r);
    for(int i = 0; i < c*r; ++i) m.data[i] = 0.0;
    return m;
}

void mat_free(mat_t * matrix){
    if(!matrix) return;
    matrix->c = 0;
    matrix->r = 0;
    FREE(matrix->data);
}

mat_t gaussian_kernel(int size, double sigma){
    assert((size % 2) != 0);
    mat_t mat = mat_create(size, size);

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
mat_t mean_kernel(int size){
    assert((size % 2) != 0);
    mat_t mat = mat_create(size, size);
    double value = 1.0 / (double) (size*size);
    for(int i = 0; i < size; ++i){
        for(int j = 0; j < size; ++j){
            MAT_AT(mat, i, j) = value;
        }
    }
    return mat;
}

mat_t sobel_x_kernel_x5(){
    mat_t mat = mat_create(5, 5);
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

mat_t sobel_y_kernel_x5() {
    mat_t mat = mat_create(5, 5);
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

mat_t sobel_y_kernel() {
    mat_t mat = mat_create(3, 3);
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

mat_t sobel_x_kernel() {
    mat_t mat = mat_create(3, 3);
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

mat_t sobel_45_kernel_x5() {
    mat_t mat = mat_create(5, 5);
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

mat_t sobel_45_kernel() {
    mat_t mat = mat_create(3, 3);
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

mat_t sobel_135_kernel_x5() {
    mat_t mat = mat_create(5, 5);
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

mat_t sobel_135_kernel() {
    mat_t mat = mat_create(3, 3);
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

mat_t laplacian_4c_kernel() {
    mat_t mat = mat_create(3, 3);
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

mat_t laplacian_kernel() {
    mat_t mat = mat_create(3, 3);
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

mat_t disc(int radius){
    mat_t matrix = mat_create(radius * 2 + 1, radius * 2 + 1);
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

mat_t dot(mat_t m1, mat_t m2){
    assert(m1.c == m2.r);
    mat_t result = mat(m1.r, m2.c);
    for(int i = 0; i < m1.r; ++i){
        for(int j = 0; j < m2.c; ++j){
            for(int k = 0; k < m1.c; ++k){
                MAT_AT(result, i, j) += MAT_AT(m1, i, k) * MAT_AT(m2, k, j);
            }
        }
    }
    return result;
}

mat_t dot_out(mat_t dest, mat_t m1, mat_t m2){
    assert(m1.c == m2.r);
    assert(dest.r == m1.r && dest.c == m2.c);
    for(int i = 0; i < m1.r; ++i){
        for(int j = 0; j < m2.c; ++j){
            for(int k = 0; k < m1.c; ++k){
                MAT_AT(dest, i, j) += MAT_AT(m1, i, k) * MAT_AT(m2, k, j);
            }
        }
    }
    return dest;
}