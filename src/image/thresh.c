#include "imagec.h"

image_t threshold(image_t image, thresh_t mode){
    image_t result = clone(image);
    int t = 0;
    switch(mode){
        case THRESH_OTSU:
            t = thresh_otsu(histdata(image));
            break;
        case THRESH_ITER:
            t = thresh_iterative(histdata(image));
            break;
    }
    for (int y = 0; y < result.h; ++y) {
        for (int x = 0; x < result.w; ++x) {
            for (int c = 0; c < result.channels; ++c) {
                if (c == 3 || PXL_AT(image, x, y, c) == VOID_PIXEL) {
                    PXL_AT(result, x, y, c) = PXL_AT(image, x, y, c);
                    continue;
                }
                int val = (int)PXL_AT(image, x, y, c);
                PXL_AT(result, x, y, c) = (val > t) ? 255 : 0;
            }
        }
    }
    return result;
}

int compare_doubles(const void *a, const void *b) {
    double diff = (*(double *)a) - (*(double *)b);
    return (diff > 0) - (diff < 0);
}

image_t threshold_by(image_t image, double value) {
    /**
     * Threshold based on start index percentile
     *  */ 
    image_t result = clone(image);
    for (size_t y = 0; y < image.h; ++y) {
        for (size_t x = 0; x < image.w; ++x) {
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

image_t threshold_percentile(image_t gradient_image, double percentile) {
    size_t total_pixels = gradient_image.w * gradient_image.h * gradient_image.channels;
    double * magnitudes = ALLOC(sizeof(double) * total_pixels);
    size_t count = 0;

    /**
     * Create an array with all image data
     *  */ 
    for (size_t y = 0; y < gradient_image.h; ++y) {
        for (size_t x = 0; x < gradient_image.w; ++x) {
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
    image_t result = clone(gradient_image);

    /**
     * Threshold based on start index percentile
     *  */
    for (size_t y = 0; y < result.h; ++y) {
        for (size_t x = 0; x < result.w; ++x) {
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
image_t threshold_adaptive_stddev_fn(image_t image, int ksize, double a, double b, int (*fn)(double, double, double)){
    assert(ksize % 2);

    image_t output = clone(image);

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
image_t threshold_adaptive_stddev(image_t image, int ksize, double a, double b){
    assert(ksize % 2);

    image_t output = clone(image);

    int size = image.w * image.h;

    double global_mean = 0.0;
    for(int i = 0; i < size; ++i)
        global_mean += image.data[i];
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

            double thresh = a * sigma + b * global_mean;

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
image_t local_stddev(image_t image, int ksize) {
    assert(ksize % 2);

    image_t output = clone(image);

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
image_t global_stddev(image_t image, int ksize) {
    assert(ksize % 2);

    image_t output = clone(image);

    int size = image.w * image.h;

    double global_mean = 0.0;
    for(int i = 0; i < size; ++i)
        global_mean += image.data[i];
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
