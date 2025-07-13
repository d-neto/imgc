#include "imgc.h"

double img_min(img_t image){
    double min = __LONG_MAX__;
    for(int i = 0; i < image.h*image.w*image.channels; ++i)
        if(image.pixels.data[i] < min) min = image.pixels.data[i];
    return min;
}

double img_max(img_t image){
    double max = 0;
    for(int i = 0; i < image.h*image.w*image.channels; ++i)
        if(image.pixels.data[i] > max) max = image.pixels.data[i];
    return max;
}

img_t img_negative(img_t image){
    return TRANSFORM(img_clone(image), 255 - pixel);
}

img_t img_log(img_t image, double c){
    c = (c) * (255.0 / log(1.0 + 255.0));
    return TRANSFORM(image, c * log(1.0 + pixel));
}

img_t img_pow(img_t image, double gamma, double c){
    return TRANSFORM(image, c * pow(pixel/255.0, gamma) * 255.0);
}

img_t img_contrast_stretch(img_t image){
    double min = img_min(image);
    double max = img_max(image);
    double diff = max - min;
    return TRANSFORM(image, ((pixel - min)*255) / diff);
}


img_t img_bright(img_t image, double b){
    img_t new_image = img_clone(image);
    FOREACH_PXL(new_image, {
        if(new_image.channels == 4 && c == 3) continue;
        if(PXL_AT(new_image, x, y, c) == VOID_PIXEL) continue;

        PXL_AT(new_image, x, y, c) = b + PXL_AT(new_image, x, y, c);
    });
    return new_image;
}

img_t img_bright_channel(img_t image, int channel, double b){
    if(image.channels <= channel) return image;
    for(int y = 0; y < image.h; ++y){
        for(int x = 0; x < image.w; ++x){
            if(PXL_AT(image, x, y, channel) == VOID_PIXEL) continue;
            PXL_AT(image, x, y, channel) = b + PXL_AT(image, x, y, channel);
        }
    }
    return image;
}

img_t img_bright_ch(img_t image, int channels, ...){
    if(image.channels < channels) return image;

    va_list args;
    va_start(args, channels);

    double_arr ch_brightness = DOUBLE_ARR(channels);
    for(int c = 0; c < channels; ++c){
        double value = va_arg(args, double);
        APPEND(ch_brightness, value);
    }
    
    for(int y = 0; y < image.h; ++y){
        for(int x = 0; x < image.w; ++x){
            for(int c = 0; c < channels; ++c){
                if(PXL_AT(image, x, y, c) == VOID_PIXEL) continue;
                PXL_AT(image, x, y, c) = ch_brightness.items[c] + PXL_AT(image, x, y, c);
            }
        }
    }
    va_end(args);
    return image;
}
