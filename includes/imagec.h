#ifndef __IMAGEC_H__
#define __IMAGEC_H__

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
    double * data;

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
} image_t;


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
 * with extract_cc(image_t source)
 */
typedef struct extracted_connected_components {
    int count;
    int capacity;
    conn_component_t * items;
    pconnection_t conn_type;
} extract_cc_t;

image_t img_load(char * _fmt_filename, ...);
void img_write(image_t image, char *_fmt_filename, ...);

void upgrade(image_t * image, int channels);
image_t create_image(int w, int h, int ch);
image_t from_bounds(image_t source);
image_t clone(image_t source);
image_t rgb2gray(image_t image);
image_t im2double(image_t image);
image_t paste(image_t src, image_t dest, int x, int y);
image_t with_all(double value, int w, int h, int channels);
image_t fill_all(image_t src, double value);
image_t fill(image_t src, int x, int y, double value);
image_t zeros(int w, int h, int channels);
image_t flood_fill(image_t src, int x, int y, hex_t color);
image_t normalize_nonzero_pixels(image_t input);
void free_image(image_t * image);


/**
 * Display
 */
void img_show(image_t image, char * win_name, ...);
void wait_until();
void shutdown_display();


/**
 * Texts
 */
void draw_txt(image_t source, int x, int y, int size, hex_t color,  char * _fmt_text, ...);


/**
 * Intensity Transformation methods declaration
 */

image_t brightness(image_t image, double b);
image_t brightness_ch(image_t image, int channels, ...);
image_t brightness_channel(image_t image, int channel, double b);
image_t negative(image_t image);
image_t tlog(image_t image, double c);
image_t tpower(image_t image, double gamma, double c);
image_t contrast_stretch(image_t image);


/**
 * Transform methods declaration
 */

image_t crop(image_t image, geometry_t geometry_bounds);
image_t cut(image_t image, geometry_t geometry_bounds);
image_t resize(image_t src, int w, int h);


/**
 * Blending methods declaration
 */

image_t sum(image_t im1, image_t im2);
image_t sum_offset(image_t im1, image_t im2, int off_x, int off_y);
image_t sum_offset_a(image_t im1, image_t im2, int off_x, int off_y, int ignore_alpha);
image_t sub(image_t im1, image_t im2);
image_t sub_offset(image_t im1, image_t im2, int off_x, int off_y);
image_t sub_offset_a(image_t im1, image_t im2, int off_x, int off_y, int ignore_alpha);
image_t multiply(image_t im1, image_t im2);
image_t multiply_offset(image_t im1, image_t im2, int off_x, int off_y);
image_t multiply_offset_a(image_t im1, image_t im2, int off_x, int off_y, int ignore_alpha);
image_t multiply_self(image_t src, int times);
image_t blend(image_t im1, image_t im2, double b_alpha);
image_t blend_offset(image_t im1, image_t im2, double b_alpha, int off_x, int off_y);
image_t blend_offset_a(image_t im1, image_t im2, double b_alpha, int off_x, int off_y, int ignore_alpha);
image_t screen(image_t im1, image_t im2);
image_t screen_offset(image_t im1, image_t im2, int off_x, int off_y);
image_t screen_offset_a(image_t im1, image_t im2, int off_x, int off_y, int ignore_alpha);
image_t and(image_t im1, image_t im2);
image_t and_offset(image_t im1, image_t im2, int off_x, int off_y);
image_t and_offset_a(image_t im1, image_t im2, int off_x, int off_y, int ignore_alpha);
image_t xor(image_t im1, image_t im2);
image_t xor_offset(image_t im1, image_t im2, int off_x, int off_y);
image_t xor_offset_a(image_t im1, image_t im2, int off_x, int off_y, int ignore_alpha);
image_t mask(image_t src, image_t mask_src);
image_t mask_offset(image_t src, image_t mask_src, int off_x, int off_y);
image_t mask_offset_a(image_t src, image_t mask_src, int off_x, int off_y, int ignore_alpha);


/**
 * Convulation & Filters methods declaration
 */
typedef enum smooth_type {SMH_GAUSSIAN, SMH_MEAN, SMH_MEDIAN} smooth_t;
image_t convolve(image_t image, matrix_t kernel);
image_t sobel(image_t image);
image_t laplacian(image_t image);
image_t laplacian_mapped(image_t image);
image_t zero_crossing(image_t laplacian_img, double threshold);
image_t mean(image_t source, int ksize);
image_t median(image_t image, int ksize);
image_t gaussian(image_t source, int ksize, double sigma);
image_t sobel_x5(image_t image);
image_t highboost(image_t input, double intensity);
image_t highboost__spec(image_t input, double intensity, smooth_t smooth_type, int ksize, double g_sigma);


/**
 * Threshold methods declaration
 */

typedef enum thresh_types {THRESH_OTSU, THRESH_ITER} thresh_t;
image_t canny(image_t source, double low_thresh, double high_thresh);
image_t threshold(image_t image, thresh_t mode);
image_t threshold_by(image_t image, double value);
image_t threshold_percentile(image_t gradient_image, double percentile);
image_t threshold_adaptive_stddev(image_t image, int ksize, double a, double b);
image_t threshold_adaptive_stddev_fn(image_t image, int ksize, double a, double b, int (*fn)(double, double, double));
int thresh_otsu(int * histogram);
int thresh_iterative(int * histogram);
int_arr multitresh_otsu(int *histogram);
image_t global_stddev(image_t image, int ksize);
image_t local_stddev(image_t image, int ksize);


/**
 * Histogram methods declaration
 */

int * histdata(image_t image);
int * histdata__ignore_zeros(image_t image);
void histshow(int * histogram);

/**
 * Morphological methods declaration
 */

image_t erode(image_t source, matrix_t structure);
image_t dilate(image_t source, matrix_t structure);
image_t opening(image_t source, matrix_t structure);
image_t closing(image_t source, matrix_t structure);
image_t extract_frontier(image_t source);
image_t fill_holes(image_t source);
extract_cc_t extract_cc(image_t source);
void free_extraction(extract_cc_t * extraction);
image_t hit_or_miss(image_t source, matrix_t structure);

#endif // __IMAGEC_H__