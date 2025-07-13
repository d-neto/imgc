#ifndef __IMGC_H__
#define __IMGC_H__

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include "geom.h"
#include "matrix.h"
#include "alloc.h"
#include "array.h"
#include "imgcmacros.h"

/**
 * A control value to pixels that is ignored on all calcs & can 
 * be used as an alpha = 0 in a RGBA image instead black pixel
 */
#define VOID_PIXEL -69.420

/**
 * Represet Colors
 */
typedef int hex_t;

typedef struct image {
    int w;
    int h;
    int channels;
    mat_t pixels;

    /**
     * Stride matrixes using .data
     */
    mat_t r;
    mat_t g;
    mat_t b;

    /**
     * Rect bound representing the
     * limits of the image
     */
    rect_t bounds;

    /**
     * Control if the object 
     * is original or an slice
     * of the original image
     */
    int _slice;
} img_t;


typedef struct {
    int label;
    int area;
    
    /**
     * Centroid
     */
    float cx, cy;

    rect_t bounding_box;
    vec2_arr pixels;
} conn_component_t;

typedef enum pixel_conn_type {PXLCONN_4, PXLCONN_8} pconnection_t;

/**
 * Conected Components struct that can be extracted 
 * with img_extract_cc(img_t source)
 */
typedef struct extracted_connected_components {
    int count;
    int capacity;
    conn_component_t * items;
    pconnection_t conn_type;
} extract_cc_t;

img_t img_load(char * _fmt_filename, ...);
void img_write(img_t image, char *_fmt_filename, ...);

void img_upgrade(img_t * image, int channels);
img_t img_create(int w, int h, int ch);
img_t from_bounds(img_t source);
img_t img_clone(img_t source);
img_t rgb2gray(img_t image);
img_t img_double(img_t image);
img_t img_paste(img_t src, img_t dest, int x, int y);
img_t with_all(double value, int w, int h, int channels);
img_t fill_all(img_t src, double value);
img_t fill(img_t src, int x, int y, double value);
img_t img_zeros(int w, int h, int channels);
img_t img_floodfl(img_t src, int x, int y, hex_t color);
img_t img_norm_nonzr(img_t input);
void img_free(img_t * image);
void img_upd_view(img_t * source);

/**
 * Display
 */
void img_show(img_t image, char * win_name, ...);
void wait_until();
void shutdown_display();


/**
 * Texts
 */
void draw_txt(img_t source, int x, int y, int size, hex_t color,  char * _fmt_text, ...);


/**
 * Intensity Transformation methods declaration
 */

img_t img_bright(img_t image, double b);
img_t img_bright_ch(img_t image, int channels, ...);
img_t img_bright_channel(img_t image, int channel, double b);
img_t img_negative(img_t image);
img_t img_log(img_t image, double c);
img_t img_pow(img_t image, double gamma, double c);
img_t img_contrast_stretch(img_t image);


/**
 * Transform methods declaration
 */

img_t img_crop(img_t image, geometry_t geometry_bounds);
img_t img_cut(img_t image, geometry_t geometry_bounds);
img_t img_resize(img_t src, int w, int h);


/**
 * Blending methods declaration
 */

img_t img_sum(img_t im1, img_t im2);
img_t img_sumoffs(img_t im1, img_t im2, int off_x, int off_y);
img_t img_sum__spec(img_t im1, img_t im2, int off_x, int off_y, int ignore_alpha);

img_t img_sub(img_t im1, img_t im2);
img_t img_suboffs(img_t im1, img_t im2, int off_x, int off_y);
img_t img_sub__spec(img_t im1, img_t im2, int off_x, int off_y, int ignore_alpha);

img_t img_multiply(img_t im1, img_t im2);
img_t img_multiply_self(img_t src, int times);
img_t img_multiplyoffs(img_t im1, img_t im2, int off_x, int off_y);
img_t img_multiply__spec(img_t im1, img_t im2, int off_x, int off_y, int ignore_alpha);

img_t img_blend(img_t im1, img_t im2, double b_alpha);
img_t img_blendoffs(img_t im1, img_t im2, double b_alpha, int off_x, int off_y);
img_t img_blend__spec(img_t im1, img_t im2, double b_alpha, int off_x, int off_y, int ignore_alpha);

img_t img_screen(img_t im1, img_t im2);
img_t img_screenoffs(img_t im1, img_t im2, int off_x, int off_y);
img_t img_screen__spec(img_t im1, img_t im2, int off_x, int off_y, int ignore_alpha);

img_t img_and(img_t im1, img_t im2);
img_t img_andoffs(img_t im1, img_t im2, int off_x, int off_y);
img_t img_and__spec(img_t im1, img_t im2, int off_x, int off_y, int ignore_alpha);

img_t img_xor(img_t im1, img_t im2);
img_t img_xoroffs(img_t im1, img_t im2, int off_x, int off_y);
img_t img_xor__spec(img_t im1, img_t im2, int off_x, int off_y, int ignore_alpha);

img_t img_mask(img_t src, img_t mask_src);
img_t img_maskoffs(img_t src, img_t mask_src, int off_x, int off_y);
img_t img_mask__spec(img_t src, img_t mask_src, int off_x, int off_y, int ignore_alpha);


/**
 * Convulation & Filters methods declaration
 */
typedef enum smooth_type {SMH_GAUSSIAN, SMH_MEAN, SMH_MEDIAN} smooth_t;
img_t img_conv(img_t image, mat_t kernel);
img_t img_sobel(img_t image);
img_t img_laplacian(img_t image);
img_t img_laplac_mapped(img_t image);
img_t img_zr_crossing(img_t laplacian_img, double threshold);
img_t img_mean(img_t source, int ksize);
img_t img_median(img_t image, int ksize);
img_t img_gaussian(img_t source, int ksize, double sigma);
img_t img_sobel_x5(img_t image);
img_t img_hboost(img_t input, double intensity);
img_t img_hboost__spec(img_t input, double intensity, smooth_t smooth_type, int ksize, double g_sigma);


/**
 * Threshold methods declaration
 */

typedef enum thresh_types {THRESH_OTSU, THRESH_ITER} thresh_t;
int thresh_otsu(int * histogram);
int thresh_iterative(int * histogram);
int_arr multitresh_otsu(int *histogram);

img_t img_canny(img_t source, double low_thresh, double high_thresh);
img_t img_threshold(img_t image, thresh_t mode);
img_t img_threshold_by(img_t image, double value);
img_t img_threshold_prct(img_t gradient_image, double percentile);
img_t img_threshold_adp(img_t image, int ksize, double a, double b);
img_t img_threshold_adp_fn(img_t image, int ksize, double a, double b, int (*fn)(double, double, double));
img_t img_glb_stddev(img_t image, int ksize);
img_t img_lcl_stddev(img_t image, int ksize);


/**
 * Histogram methods declaration
 */

int * histdata(img_t image);
int * histdata__ignore_img_zeros(img_t image);
void histshow(int * histogram);

/**
 * Morphological methods declaration
 */

img_t img_erode(img_t source, mat_t structure);
img_t img_dilate(img_t source, mat_t structure);
img_t img_open(img_t source, mat_t structure);
img_t img_close(img_t source, mat_t structure);
img_t extract_frontier(img_t source);
img_t img_fillholes(img_t source);
extract_cc_t img_extract_cc(img_t source);
void img_free_extraction(extract_cc_t * extraction);
img_t img_hit_or_miss(img_t source, mat_t structure);

#endif // __IMGC_H__