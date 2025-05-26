#include "imagec.h"

image_t convolve(image_t image, matrix_t kernel){
    image_t new_image = clone(image);

    int kxstart = -(kernel.c/2);
    int kystart = -(kernel.r/2);
    int kxend = (kernel.c/2);
    int kyend = (kernel.r/2);

    double value = 0;
    int channels = image.channels;
    int px = 0;
    int py = 0;

    for(size_t y = 0; y < new_image.h; ++y){
        for(size_t x = 0; x < new_image.w; ++x){

            for(size_t c = 0; c < channels; ++c){

                // Ignore alpha channel
                if(PXL_AT(new_image, x, y, c) == VOID_PIXEL) continue;
                if(c == 3) {
                    PXL_AT(new_image, x, y, c) = PXL_AT(image, x, y, c);
                    continue;
                }

                value = 0;

                for(int kx = kxstart; kx <= kxend; ++kx){
                    px = x + kx;
                    if (px < 0) px = 0;
                    if (px >= image.w) px = image.w-1;
                    for(int ky = kystart; ky <= kyend; ++ky){

                        py = y + ky;
                        if (py < 0) py = 0;
                        if (py >= image.h) py = image.h-1;   
                        
                        /**
                         * For VOID_PIXELS found, ignore; Then to
                         * avoid side effects multiply by the central
                         * pixel.
                         */
                        if(PXL_AT(image, px, py, c) == VOID_PIXEL){
                            value += PXL_AT(image, x, y, c) * MAT_AT(kernel, kx + kxend, ky + kyend);
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

image_t median(image_t image, int ksize){
    assert(ksize%2 != 0);

    int kend = ksize/2;
    int kstart = -ksize/2;
    
    int middle = (ksize*ksize)/2;
    double_arr array = DOUBLE_ARR(ksize*ksize+1);
    memset(array.items, 0, array.capacity * sizeof(*array.items));
    double aux = 0;
    
    image_t new_image = create_image(image.w, image.h, image.channels);

    int tx = 0, ty = 0;
    FOREACH_PXL(image, {
        for(int kx = kstart; kx <= kend; ++kx){
            for(int ky = kstart; ky <= kend; ++ky){
                tx = x + kx;
                ty = y + ky;
                if(ty >= image.h || ty < 0 || tx < 0 || tx >= image.w){
                    continue;
                }
                APPEND(array, PXL_AT(image, tx, ty, c));
            }
        }
        for(int k = 0; k < array.count; ++k){
            for(int l = 0; l < array.count-1-k; ++l){
                if(VGET(array, l) > VGET(array, l+1)){
                    aux = VGET(array, l);
                    VGET(array, l) = VGET(array, l+1);
                    VGET(array, l+1) = aux;
                }
            }
        }
        PXL_AT(new_image, x, y, c) = VGET(array, middle);
        array.count = 0;
    });
    return new_image;
}

image_t mean(image_t source, int ksize){
    return convolve(source, mean_kernel(ksize));
}

image_t gaussian(image_t source, int ksize, double sigma){
    return convolve(source, gaussian_kernel(ksize, sigma));
}

image_t laplacian(image_t image) {
    image_t lapl_image = zeros(image.w, image.h, image.channels);

    matrix_t kernel = laplacian_kernel();
    int kxstart = -(kernel.c / 2);
    int kystart = -(kernel.r / 2);
    int kxend = (kernel.c / 2);
    int kyend = (kernel.r / 2);

    double value = 0.0;
    int py, px;
    for (size_t y = 0; y < image.h; ++y) {
        for (size_t x = 0; x < image.w; ++x) {
            for (size_t c = 0; c < image.channels; ++c) {
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

image_t zero_crossing(image_t laplacian_img, double threshold) {
    image_t output = zeros(laplacian_img.w, laplacian_img.h, laplacian_img.channels);

    for (size_t y = 1; y < laplacian_img.h - 1; ++y) {
        for (size_t x = 1; x < laplacian_img.w - 1; ++x) {
            for (size_t c = 0; c < laplacian_img.channels; ++c) {
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

image_t laplacian_mapped(image_t image) {
    image_t new_image = laplacian(image);
    for (size_t y = 0; y < new_image.h; ++y) {
        for (size_t x = 0; x < new_image.w; ++x) {
            for (size_t c = 0; c < new_image.channels; ++c) {
                if (c == 3) continue;
                double value = PXL_AT(new_image, x, y, c) + 128.0;
                PXL_AT(new_image, x, y, c) = value;
            }
        }
    }
    return new_image;
}

image_t sobel(image_t image) {
    image_t new_image = clone(image);
    image_t sobel_x = convolve(image, sobel_x_kernel());
    image_t sobel_y = convolve(image, sobel_y_kernel());
    double gx, gy, magnitude;
    for (size_t y = 0; y < new_image.h; ++y) {
        for (size_t x = 0; x < new_image.w; ++x) {
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

image_t sobel_x5(image_t image) {
    image_t new_image = clone(image);
    image_t sobel_x = convolve(image, sobel_x_kernel_x5());
    image_t sobel_y = convolve(image, sobel_y_kernel_x5());
    for (size_t y = 0; y < new_image.h; ++y) {
        for (size_t x = 0; x < new_image.w; ++x) {
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

image_t highboost(image_t input, double intensity){
    return highboost__spec(input, intensity, SMH_GAUSSIAN, 5, 1.0);
}

image_t highboost__spec(image_t input, double intensity, smooth_t smooth_type, int ksize, double g_sigma){
    image_t smooth;
    switch(smooth_type){
        default:
        case SMH_GAUSSIAN:
            smooth = gaussian(input, ksize, g_sigma);
            break;
        case SMH_MEAN:
            smooth = mean(input, ksize);
        case SMH_MEDIAN:
            smooth = median(input, ksize);
            break;
    }
    image_t sub_result = sub(input, smooth);
    image_t result = clone(input);
    FOREACH_PXL(result, {
        PXL_AT(result, x, y, c) += (intensity * PXL_AT(sub_result, x, y, c));
    });
    return result;
}