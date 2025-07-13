#include "imagec.h"

img_t img_threshold(img_t image, thresh_t mode){
    img_t result = from_bounds(image);
    int t = 0;
    switch(mode){
        case THRESH_OTSU:
            t = thresh_otsu(histdata(image));
            break;
        case THRESH_ITER:
            t = thresh_iterative(histdata(image));
            break;
    }
    FOREACH_PXL(result, {
        if (c == 3 || PXL_AT(image, x, y, c) == VOID_PIXEL) {
            PXL_AT(result, x, y, c) = PXL_AT(image, x, y, c);
            continue;
        }
        int val = (int)PXL_AT(image, x, y, c);
        PXL_AT(result, x, y, c) = (val > t) ? 255 : 0;
    });
    return result;
}

int compare_doubles(const void *a, const void *b) {
    double diff = (*(double *)a) - (*(double *)b);
    return (diff > 0) - (diff < 0);
}

img_t img_threshold_by(img_t image, double value) {
    /**
     * Threshold based on start index percentile
     *  */ 
    img_t result = img_clone(image);
    for (int y = 0; y < image.h; ++y) {
        for (int x = 0; x < image.w; ++x) {
            for (int c = 0; c < image.channels; ++c) {
                if (c == 3) {
                    PXL_AT(result, x, y, c) = PXL_AT(image, x, y, c);
                    continue;
                }
                double val = PXL_AT(image, x, y, c);
                PXL_AT(result, x, y, c) = (val >= value) ? val : 0.0;
            }
        }
    }
    return result;
}

img_t img_threshold_prct(img_t gradient_image, double percentile) {
    size_t total_pixels = gradient_image.w * gradient_image.h * gradient_image.channels;
    double * magnitudes = ALLOC(sizeof(double) * total_pixels);
    size_t count = 0;

    /**
     * Create an array with all image data
     *  */ 
    for (int y = 0; y < gradient_image.h; ++y) {
        for (int x = 0; x < gradient_image.w; ++x) {
            for (int c = 0; c < gradient_image.channels; ++c) {
                // Ignore Alpha Channel
                if (c == 3) continue;

                double val = PXL_AT(gradient_image, x, y, c);

                // Ignore VOID_PIXELS
                if(val == VOID_PIXEL) continue;

                magnitudes[count++] = val;
            }
        }
    }

    /**
     * Sort the array to be able to get the percentile
     *  */ 
    qsort(magnitudes, count, sizeof(double), compare_doubles);

    size_t index = (size_t)(percentile * count);
    if (index >= count) index = count - 1;
    double threshold = magnitudes[index];
    FREE(magnitudes);
    img_t result = img_clone(gradient_image);

    /**
     * Threshold based on start index percentile
     *  */
    for (int y = 0; y < result.h; ++y) {
        for (int x = 0; x < result.w; ++x) {
            for (int c = 0; c < result.channels; ++c) {
                if (c == 3) {
                    PXL_AT(result, x, y, c) = PXL_AT(gradient_image, x, y, c);
                    continue;
                }
                double val = PXL_AT(gradient_image, x, y, c);
                PXL_AT(result, x, y, c) = (val >= threshold) ? 255.0 : 0.0;
            }
        }
    }

    return result;
}

int thresh_otsu(int * histogram){
    
    int total_pixels = 0;
    for(int i = 0; i < 256; ++i) 
        total_pixels += histogram[i];

    double probabilities[256] = {0};
    double sum_all = 0.0;
    for(int i = 0; i < 256; ++i){
        probabilities[i] = (double) (histogram[i]) / (double) total_pixels;
        sum_all += i * probabilities[i];
    }

    double w0 = 0.0;   
    double mu0 = 0.0;  
    double max_sigma = 0.0;
    int best_thresh = 0;
    int consecutive_count = 0;

    for (int t = 0; t < 256; ++t) {
        w0 += probabilities[t];
        mu0 += t * probabilities[t];

        if (w0 == 0.0 || w0 == 1.0)
            continue;

        double w1 = 1.0 - w0;
        double mu1 = (sum_all - mu0) / w1;
        double mu0_norm = mu0 / w0;

        double sigma_b = w0 * w1 * (mu0_norm - mu1) * (mu0_norm - mu1);

        if (sigma_b > max_sigma) {
            consecutive_count = 0;
            max_sigma = sigma_b;
            best_thresh = t;
        }

        if (sigma_b == max_sigma) {
            consecutive_count += 1;
            max_sigma = sigma_b;
            best_thresh = t - consecutive_count/2;
        }
    }

    return best_thresh;
}

int thresh_iterative(int * histogram) {
    long total = 0;
    long sum = 0;
    for (int i = 0; i < 256; ++i) {
        total += histogram[i];
        sum += i * histogram[i];
    }

    if (total == 0) return 0;

    int threshold = sum / total;

    int prev_thresh = -1;

    while (threshold != prev_thresh) {
        prev_thresh = threshold;

        long sum1 = 0, count1 = 0;
        for (int i = 0; i <= threshold; ++i) {
            sum1 += i * histogram[i];
            count1 += histogram[i];
        }

        long sum2 = 0, count2 = 0;
        for (int i = threshold + 1; i < 256; ++i) {
            sum2 += i * histogram[i];
            count2 += histogram[i];
        }

        double mean1 = count1 ? (double)sum1 / count1 : 0;
        double mean2 = count2 ? (double)sum2 / count2 : 0;

        threshold = (int)((mean1 + mean2) / 2.0);
    }

    return threshold;
}

int_arr multitresh_otsu(int *histogram) {
    int total_pixels = 0;
    for(int i = 0; i < 256; ++i)
        total_pixels += histogram[i];

    double probabilities[256];
    double sum_total = 0.0;
    for (int i = 0; i < 256; ++i) {
        probabilities[i] = (double) histogram[i] / total_pixels;
        sum_total += i * probabilities[i];
    }

    double max_sigma = -1.0;
    int best_t1 = 0, best_t2 = 0;

    for (int t1 = 1; t1 < 254; ++t1) {
        for (int t2 = t1 + 1; t2 < 255; ++t2) {
            // Classe 1: [0, t1)
            double w0 = 0.0, mu0 = 0.0;
            for (int i = 0; i < t1; ++i) {
                w0 += probabilities[i];
                mu0 += i * probabilities[i];
            }

            // Classe 2: [t1, t2)
            double w1 = 0.0, mu1 = 0.0;
            for (int i = t1; i < t2; ++i) {
                w1 += probabilities[i];
                mu1 += i * probabilities[i];
            }

            // Classe 3: [t2, 255]
            double w2 = 0.0, mu2 = 0.0;
            for (int i = t2; i < 256; ++i) {
                w2 += probabilities[i];
                mu2 += i * probabilities[i];
            }

            if (w0 == 0 || w1 == 0 || w2 == 0)
                continue;

            mu0 /= w0;
            mu1 /= w1;
            mu2 /= w2;

            double sigma_b = w0 * (mu0 - sum_total)*(mu0 - sum_total)
                           + w1 * (mu1 - sum_total)*(mu1 - sum_total)
                           + w2 * (mu2 - sum_total)*(mu2 - sum_total);

            if (sigma_b > max_sigma) {
                max_sigma = sigma_b;
                best_t1 = t1;
                best_t2 = t2;
            }
        }
    }

    int_arr result = INT_ARR(2);
    result.items[0] = best_t1;
    result.items[1] = best_t2;
    result.count = 2;
    return result;
}

/**
 * Applies adaptive thresholding based on local standard deviation and global mean.
 * The threshold at each pixel is
 * computed as: `threshold = a * sigma + b * global_mean`.
 * Useful for images with uneven lighting or varying local contrast.
 */
img_t img_threshold_adp_fn(img_t image, int ksize, double a, double b, int (*fn)(double, double, double)){
    assert(ksize % 2);

    img_t output = img_clone(image);

    int kstart = -ksize/2;
    int kend = ksize/2;

    double_arr values = DOUBLE_ARR(ksize*ksize);

    for(int y = 0; y < image.h; ++y){
        for(int x = 0; x < image.w; ++x){

            values.count = 0;

            for(int ky = kstart; ky <= kend; ++ky){
                for(int kx = kstart; kx <= kend; ++kx){
                    int nx = x + kx;
                    int ny = y + ky;
                    if(nx >= 0 && nx < image.w && ny >= 0 && ny < image.h){
                        APPEND(values, PXL_AT(image, nx, ny, 0));
                    }
                }
            }

            double local_mean = 0.0;
            for(int i = 0; i < values.count; ++i){
                local_mean += values.items[i];
            }
            local_mean /= values.count;

            double var = 0.0;
            for(int i = 0; i < values.count; ++i){
                var += (values.items[i] - local_mean) * (values.items[i] - local_mean);
            }
            var /= values.count;
            double sigma = sqrt(var);

            double val = PXL_AT(image, x, y, 0);
            if(fn(val, a*sigma, b*local_mean))
                PXL_AT(output, x, y, 0) = 255.0;
            else PXL_AT(output, x, y, 0) = 0.0;
        }
    }

    FREE(values.items);

    return output;
}

/**
 * Applies adaptive thresholding based on local standard deviation and global mean.
 * The threshold at each pixel is
 * computed as: `threshold = a * sigma + b * global_mean`.
 * Useful for images with uneven lighting or varying local contrast.
 */
img_t img_threshold_adp(img_t image, int ksize, double a, double b){
    assert(ksize % 2);

    img_t output = img_clone(image);

    int size = image.w * image.h;

    double global_mean = 0.0;
    for(int i = 0; i < size; ++i)
        global_mean += image.pixels.data[i];
    global_mean /= size;

    int kstart = -ksize/2;
    int kend = ksize/2;

    double_arr values = DOUBLE_ARR(ksize*ksize);

    for(int y = 0; y < image.h; ++y){
        for(int x = 0; x < image.w; ++x){

            values.count = 0;

            for(int ky = kstart; ky <= kend; ++ky){
                for(int kx = kstart; kx <= kend; ++kx){
                    int nx = x + kx;
                    int ny = y + ky;
                    if(nx >= 0 && nx < image.w && ny >= 0 && ny < image.h){
                        APPEND(values, PXL_AT(image, nx, ny, 0));
                    }
                }
            }

            double local_mean = 0.0;
            for(int i = 0; i < values.count; ++i){
                local_mean += values.items[i];
            }
            local_mean /= values.count;

            double var = 0.0;
            for(int i = 0; i < values.count; ++i){
                var += (values.items[i] - local_mean) * (values.items[i] - local_mean);
            }
            var /= values.count;
            double sigma = sqrt(var);

            double thresh = (a * sigma) + (b * global_mean);

            double val = PXL_AT(image, x, y, 0);
            PXL_AT(output, x, y, 0) = val > thresh ? 255.0 : 0.0;
        }
    }

    FREE(values.items);

    return output;
}

/**
 * Computes the local standard deviation for each pixel in the image,
 * using a square window of size `ksize`. 
 * The output image highlights regions with high local variance 
 * (e.g., texture or edges).
 */
img_t img_lcl_stddev(img_t image, int ksize) {
    assert(ksize % 2);

    img_t output = img_clone(image);

    int kstart = -ksize/2;
    int kend = ksize/2;

    double_arr values = DOUBLE_ARR(ksize*ksize);

    for(int y = 0; y < image.h; ++y){
        for(int x = 0; x < image.w; ++x){

            values.count = 0;

            for(int ky = kstart; ky <= kend; ++ky){
                for(int kx = kstart; kx <= kend; ++kx){
                    int nx = x + kx;
                    int ny = y + ky;
                    if(nx >= 0 && nx < image.w && ny >= 0 && ny < image.h){
                        APPEND(values, PXL_AT(image, nx, ny, 0));
                    }
                }
            }

            double local_mean = 0.0;
            for(int i = 0; i < values.count; ++i){
                local_mean += values.items[i];
            }
            local_mean /= values.count;

            double var = 0.0;
            for(int i = 0; i < values.count; ++i){
                var += (values.items[i] - local_mean) * (values.items[i] - local_mean);
            }

            var /= values.count;
            double sigma = sqrt(var);

            PXL_AT(output, x, y, 0) = sigma;
        }
    }

    FREE(values.items);
    return output;
}

/**
 * Computes the standard deviation within a local window around each pixel,
 * using the global image mean. This highlights variations relative to the global mean,
 * useful for detecting regions that deviate from overall brightness.
 */
img_t img_glb_stddev(img_t image, int ksize) {
    assert(ksize % 2);

    img_t output = img_clone(image);

    int size = image.w * image.h;

    double global_mean = 0.0;
    for(int i = 0; i < size; ++i)
        global_mean += image.pixels.data[i];
    global_mean /= size;

    int kstart = -ksize/2;
    int kend = ksize/2;
    int count = 0;
    
    for(int y = 0; y < image.h; ++y){
        for(int x = 0; x < image.w; ++x){

            count = 0;
            double var = 0.0;

            for(int ky = kstart; ky <= kend; ++ky){
                for(int kx = kstart; kx <= kend; ++kx){
                    int nx = x + kx;
                    int ny = y + ky;
                    if(nx >= 0 && nx < image.w && ny >= 0 && ny < image.h){
                        var += (PXL_AT(image, nx, ny, 0) - global_mean) * (PXL_AT(image, nx, ny, 0) - global_mean);
                        count += 1;
                    }
                }
            }
            var /= count;
            double sigma = sqrt(var);
            PXL_AT(output, x, y, 0) = sigma;
        }
    }
    return output;
}

img_t non_max_supression(img_t magnitude, img_t direction) {
    img_t nms = img_create(magnitude.w, magnitude.h, 1);
    memset(nms.pixels.data, 0, nms.w * nms.h * sizeof(float)); // Inicializa a imagem com img_zeros

    // Itera de 1 a w-1 e h-1 para evitar acessos fora dos limites da imagem.
    for (int y = 1; y < magnitude.h - 1; ++y) {
        for (int x = 1; x < magnitude.w - 1; ++x) {
            float angle = PXL_AT(direction, x, y, 0);
            float mag = PXL_AT(magnitude, x, y, 0);

            // Se a magnitude for zero, não é uma borda, podemos saltar.
            if (mag == 0) continue;

            float q = 0.0f;
            float r = 0.0f;

            // Quantiza a direção do gradiente e encontra os vizinhos para comparar
            if ((0 <= angle && angle < 22.5) || (157.5 <= angle && angle <= 180)) {
                // Borda vertical -> Gradiente horizontal
                q = PXL_AT(magnitude, x + 1, y, 0);
                r = PXL_AT(magnitude, x - 1, y, 0);
            } else if (22.5 <= angle && angle < 67.5) {
                // Borda a 135 graus -> Gradiente a 45 graus
                // **ESTA É A CORREÇÃO CRÍTICA**
                q = PXL_AT(magnitude, x + 1, y + 1, 0); // VIZINHO CORRETO
                r = PXL_AT(magnitude, x - 1, y - 1, 0); // VIZINHO CORRETO
            } else if (67.5 <= angle && angle < 112.5) {
                // Borda horizontal -> Gradiente vertical
                q = PXL_AT(magnitude, x, y + 1, 0);
                r = PXL_AT(magnitude, x, y - 1, 0);
            } else if (112.5 <= angle && angle < 157.5) {
                // Borda a 45 graus -> Gradiente a 135 graus
                q = PXL_AT(magnitude, x - 1, y + 1, 0);
                r = PXL_AT(magnitude, x + 1, y - 1, 0);
            }

            // Se a magnitude do píxel for maior ou igual à dos seus vizinhos ao longo do gradiente, mantém-na.
            if (mag >= q && mag >= r) {
                PXL_AT(nms, x, y, 0) = mag;
            } else {
                PXL_AT(nms, x, y, 0) = 0;
            }
        }
    }
    return nms;
}

#define STRONG 255
#define WEAK 100

int in_bounds(int x, int y, int w, int h) {
    return (x >= 0 && x < w && y >= 0 && y < h);
}

img_t hysteresis_tracking(img_t nms, double low_thresh, double high_thresh) {
    int w = nms.w, h = nms.h;
    img_t result = img_create(w, h, 1);

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            double val = PXL_AT(nms, x, y, 0);
            if (val >= high_thresh)
                PXL_AT(result, x, y, 0) = STRONG;
            else if (val >= low_thresh)
                PXL_AT(result, x, y, 0) = WEAK;
            else
                PXL_AT(result, x, y, 0) = 0;
        }
    }

    int max_pixels = w * h;
    int *queue_x = ALLOC(sizeof(int) * max_pixels);
    int *queue_y = ALLOC(sizeof(int) * max_pixels);

    int front = 0, rear = 0;

    for (int y = 0; y < h; ++y)
        for (int x = 0; x < w; ++x)
            if (PXL_AT(result, x, y, 0) == STRONG) {
                queue_x[rear] = x;
                queue_y[rear] = y;
                rear++;
            }

    int dx[8] = {-1, 0, 1, -1, 1, -1, 0, 1};
    int dy[8] = {-1, -1, -1, 0, 0, 1, 1, 1};

    while (front < rear) {
        int cx = queue_x[front];
        int cy = queue_y[front];
        front++;

        for (int k = 0; k < 8; ++k) {
            int nx = cx + dx[k];
            int ny = cy + dy[k];

            if (in_bounds(nx, ny, w, h)) {
                if (PXL_AT(result, nx, ny, 0) == WEAK) {
                    PXL_AT(result, nx, ny, 0) = STRONG;
                    queue_x[rear] = nx;
                    queue_y[rear] = ny;
                    rear++;
                }
            }
        }
    }

    for (int y = 0; y < h; ++y)
        for (int x = 0; x < w; ++x)
            if (PXL_AT(result, x, y, 0) == WEAK)
                PXL_AT(result, x, y, 0) = 0;

    free(queue_x);
    free(queue_y);

    return result;
}

img_t img_canny_spec(
    img_t source, 
    smooth_t smooth_type, 
    int ksize, 
    double sigma, 
    double low_thresh_ratio, 
    double high_thresh_ratio
) {
    
    img_t smooth;

    switch(smooth_type){
        case SMH_MEDIAN:
            smooth = img_median(source, ksize);
            break;
        case SMH_MEAN:
            smooth = img_mean(source, ksize);
            break;
        case SMH_GAUSSIAN:
        default:
            smooth = img_gaussian(source, ksize, sigma);
            break;
    }
    
    mat_t sobel_x = sobel_x_kernel();
    mat_t sobel_y = sobel_y_kernel();
    img_t gx = img_conv(smooth, sobel_x);
    img_t gy = img_conv(smooth, sobel_y);

    mat_free(&sobel_x);
    mat_free(&sobel_y);
    img_free(&smooth);

    img_t magnitude = img_create(source.w, source.h, 1);
    img_t direction = img_create(source.w, source.h, 1);

    double max_magnitude = 0.0f;

    FOREACH_PXL(magnitude, {
        float gx_val = PXL_AT(gx, x, y, 0);
        float gy_val = PXL_AT(gy, x, y, 0);

        float mag = sqrtf(gx_val * gx_val + gy_val * gy_val);
        float angle = atan2f(gy_val, gx_val) * (180.0 / M_PI);

        if (angle < 0) angle += 180;

        PXL_AT(magnitude, x, y, 0) = mag;
        PXL_AT(direction, x, y, 0) = angle;

        if (mag > max_magnitude) {
            max_magnitude = mag;
        }
    });

    img_free(&gx);
    img_free(&gy);

    img_t nms = non_max_supression(magnitude, direction);

    img_free(&magnitude);
    img_free(&direction);

    double _high_thresh = 0.3 * max_magnitude;
    double _low_thresh = 0.4 * _high_thresh;

    if(high_thresh_ratio || low_thresh_ratio) {
        _high_thresh = max_magnitude * high_thresh_ratio;
        _low_thresh = _high_thresh * low_thresh_ratio;
    }

    img_t result = hysteresis_tracking(nms, _low_thresh, _high_thresh);

    img_free(&nms);

    return result;
}

img_t img_canny(img_t source, double low_thresh_ratio, double high_thresh_ratio) {
    return img_canny_spec(source, SMH_GAUSSIAN, 9, 3.0, low_thresh_ratio, high_thresh_ratio);
}