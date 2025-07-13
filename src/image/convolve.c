#include "imagec.h"

img_t img_conv(img_t image, mat_t kernel){
    
    int kxstart = -(kernel.c/2);
    int kystart = -(kernel.r/2);
    int kxend = (kernel.c/2);
    int kyend = (kernel.r/2);
    
    double value = 0;
    int channels = image.channels;
    int px = 0;
    int py = 0;
    
    int x0 = image.bounds.x1;
    int y0 = image.bounds.y1;
    int w = image.bounds.x2 - x0;
    int h = image.bounds.y2 - y0;
    img_t new_image = img_create(w, h, image.channels);

    for(int y = 0; y < new_image.h; ++y){
        for(int x = 0; x < new_image.w; ++x){

            for(int c = 0; c < channels; ++c){

                // Ignore alpha channel
                if(PXL_AT(new_image, x, y, c) == VOID_PIXEL) continue;
                if(c == 3) {
                    PXL_AT(new_image, x, y, c) = PXL_AT(image, x + x0, y + y0, c);
                    continue;
                }

                value = 0;

                for(int kx = kxstart; kx <= kxend; ++kx){
                    px = x0 + x + kx;
                    if (px < 0) px = 0;
                    if (px >= image.w) px = image.w-1;
                    for(int ky = kystart; ky <= kyend; ++ky){

                        py = y0 + y + ky;
                        if (py < 0) py = 0;
                        if (py >= image.h) py = image.h-1;   
                        
                        /**
                         * For VOID_PIXELS found, ignore; Then to
                         * avoid side effects multiply by the central
                         * pixel.
                         */
                        if(PXL_AT(image, px, py, c) == VOID_PIXEL){
                            value += PXL_AT(image, x0 + x, y0 + y, c) * MAT_AT(kernel, kx + kxend, ky + kyend);
                            continue;
                        }

                        value += PXL_AT(image, px, py, c) * MAT_AT(kernel, kx + kxend, ky + kyend);
                    }
                }
                PXL_AT(new_image, x, y, c) = value;
            }
        }
    }

    return new_image;
}

static int qs_comp_double(const void *a, const void *b){
    double diff = (*(double *)a) - (*(double *)b);
    return (diff > 0) - (diff < 0);
}

img_t img_median(img_t image, int ksize){
    assert(ksize%2 != 0);

    int kend = ksize/2;
    int kstart = -ksize/2;
    
    int middle = (ksize*ksize)/2;
    double_arr array = DOUBLE_ARR(ksize*ksize+1);
    memset(array.items, 0, array.capacity * sizeof(*array.items));
    
    int w = image.bounds.x2 - image.bounds.x1;
    int h = image.bounds.y2 - image.bounds.y1;
    img_t new_image = img_create(w, h, image.channels);

    int tx = 0, ty = 0;
    int off_x = image.bounds.x1;
    int off_y = image.bounds.y1;

    FOREACH_PXL(new_image, {
        for(int kx = kstart; kx <= kend; ++kx){
            for(int ky = kstart; ky <= kend; ++ky){
                tx = off_x + x + kx;
                ty = off_y + y + ky;
                if(ty >= image.h || ty < 0 || tx < 0 || tx >= image.w){
                    continue;
                }
                APPEND(array, PXL_AT(image, tx, ty, c));
            }
        }
        qsort(array.items, array.count, sizeof(array.items[0]), qs_comp_double);
        PXL_AT(new_image, x, y, c) = VGET(array, middle);
        array.count = 0;
    });
    return new_image;
}

img_t img_mean(img_t source, int ksize){
    return img_conv(source, mean_kernel(ksize));
}

img_t img_gaussian(img_t source, int ksize, double sigma){
    mat_t kernel = gaussian_kernel(ksize, sigma);
    img_t result = img_conv(source, kernel);
    mat_free(&kernel);
    return result;
}

img_t img_laplacian(img_t image) {
    img_t lapl_image = img_zeros(image.w, image.h, image.channels);

    mat_t kernel = laplacian_kernel();
    int kxstart = -(kernel.c / 2);
    int kystart = -(kernel.r / 2);
    int kxend = (kernel.c / 2);
    int kyend = (kernel.r / 2);

    double value = 0.0;
    int py, px;
    for (int y = 0; y < image.h; ++y) {
        for (int x = 0; x < image.w; ++x) {
            for (int c = 0; c < image.channels; ++c) {
                if (c == 3) {
                    PXL_AT(lapl_image, x, y, c) = PXL_AT(image, x, y, c);
                    continue;
                }

                value = 0.0;
                for (int ky = kystart; ky <= kyend; ++ky) {
                    py = y + ky;
                    if (py < 0) py = 0;
                    if (py >= image.h) py = image.h - 1;

                    for (int kx = kxstart; kx <= kxend; ++kx) {
                        px = x + kx;
                        if (px < 0) px = 0;
                        if (px >= image.w) px = image.w - 1;

                        value += PXL_AT(image, px, py, c) * MAT_AT(kernel, kx + kxend, ky + kyend);
                    }
                }
                PXL_AT(lapl_image, x, y, c) = value;
            }
        }
    }

    return lapl_image;
}

img_t img_zr_crossing(img_t laplacian_img, double threshold) {
    img_t output = img_zeros(laplacian_img.w, laplacian_img.h, laplacian_img.channels);

    for (int y = 1; y < laplacian_img.h - 1; ++y) {
        for (int x = 1; x < laplacian_img.w - 1; ++x) {
            for (int c = 0; c < laplacian_img.channels; ++c) {
                if (c == 3) {
                    PXL_AT(output, x, y, c) = PXL_AT(laplacian_img, x, y, c);
                    continue;
                }

                double center = PXL_AT(laplacian_img, x, y, c);
                int has_zero_crossing = 0;

                int dx[] = {1, -1, 0, 0};
                int dy[] = {0, 0, 1, -1};

                for (int k = 0; k < 4; ++k) {
                    double neighbor = PXL_AT(laplacian_img, x + dx[k], y + dy[k], c);
                    if ((center < 0 && neighbor > 0) || (center > 0 && neighbor < 0)) {
                        double magnitude = fabs(center - neighbor);
                        if (magnitude > threshold) {
                            has_zero_crossing = 1;
                            break;
                        }
                    }
                }

                PXL_AT(output, x, y, c) = has_zero_crossing ? 255.0 : 0.0;
            }
        }
    }
    return output;
}

img_t img_laplac_mapped(img_t image) {
    img_t new_image = img_laplacian(image);
    for (int y = 0; y < new_image.h; ++y) {
        for (int x = 0; x < new_image.w; ++x) {
            for (int c = 0; c < new_image.channels; ++c) {
                if (c == 3) continue;
                double value = PXL_AT(new_image, x, y, c) + 128.0;
                PXL_AT(new_image, x, y, c) = value;
            }
        }
    }
    return new_image;
}

img_t img_sobel(img_t image) {
    img_t new_image = img_clone(image);
    img_t sobel_x = img_conv(image, sobel_x_kernel());
    img_t sobel_y = img_conv(image, sobel_y_kernel());
    double gx, gy, magnitude;
    for (int y = 0; y < new_image.h; ++y) {
        for (int x = 0; x < new_image.w; ++x) {
            for (int c = 0; c < image.channels; ++c) {
                // Ignore alpha channel
                if(PXL_AT(new_image, x, y, c) == VOID_PIXEL) continue;
                if (c == 3) {  
                    PXL_AT(new_image, x, y, c) = PXL_AT(image, x, y, c);
                    continue;
                }
                gx = PXL_AT(sobel_x, x, y, c);
                gy = PXL_AT(sobel_y, x, y, c);
                magnitude = sqrt(gx * gx + gy * gy);
                PXL_AT(new_image, x, y, c) = magnitude;
            }
        }
    }
    return new_image;
}

img_t img_sobel_x5(img_t image) {
    img_t new_image = img_clone(image);
    img_t sobel_x = img_conv(image, sobel_x_kernel_x5());
    img_t sobel_y = img_conv(image, sobel_y_kernel_x5());
    for (int y = 0; y < new_image.h; ++y) {
        for (int x = 0; x < new_image.w; ++x) {
            for (int c = 0; c < image.channels; ++c) {

                if(PXL_AT(new_image, x, y, c) == VOID_PIXEL) continue;

                // Ignore alpha channel
                if (c == 3) {  
                    PXL_AT(new_image, x, y, c) = PXL_AT(image, x, y, c);
                    continue;
                }

                double gx = PXL_AT(sobel_x, x, y, c);
                double gy = PXL_AT(sobel_y, x, y, c);

                double magnitude = sqrt(gx * gx + gy * gy);

                PXL_AT(new_image, x, y, c) = magnitude;
            }
        }
    }
    return new_image;
}

img_t img_hboost(img_t input, double intensity){
    return img_hboost__spec(input, intensity, SMH_GAUSSIAN, 5, 1.0);
}

img_t img_hboost__spec(img_t input, double intensity, smooth_t smooth_type, int ksize, double g_sigma){
    img_t smooth;
    switch(smooth_type){
        default:
        case SMH_GAUSSIAN:
            smooth = img_gaussian(input, ksize, g_sigma);
            break;
        case SMH_MEAN:
            smooth = img_mean(input, ksize);
            break;
        case SMH_MEDIAN:
            smooth = img_median(input, ksize);
            break;
    }
    img_t sub_result = img_sub(input, smooth);
    img_t result = img_clone(input);
    FOREACH_PXL(result, {
        PXL_AT(result, x, y, c) += (intensity * PXL_AT(sub_result, x, y, c));
    });
    return smooth;
}