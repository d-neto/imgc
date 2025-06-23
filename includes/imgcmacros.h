#ifndef __IMGC_MACROS_H__
#define __IMGC_MACROS_H__

#define AT(image, x, y) ((image).data+(((image).w * (y) + (x)) * (image).channels))
#define PXL_AT(image, x, y, c) (image).data[((image).w * (y) + (x)) * (image).channels + (c)]
#define CLAMP(x, min, max) ((x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))

/**
 * Run over an image for each pixel. You can access local variables (x, y, c).
 * __function must be in brackets
 * Ex.: 
 * FOREACH_PXL(image, { printf("(%d, %d)\n", x, y); });
 */
#define FOREACH_PXL(__image, __function) do { \
    int channels = (__image).channels; \
    int __x0 = (__image).bounds.x1;\
    int __x1 = (__image).bounds.x2;\
    int __y0 = (__image).bounds.y1;\
    int __y1 = (__image).bounds.y2;\
    for(int y = __y0; y < __y1; ++y){ \
        for(int x = __x0; x < __x1; ++x){ \
            for(int c = 0; c < channels; ++c){ \
                __function \
            } \
        } \
    } \
} while(0)

#define FOREACH_PXL_IN_CH(__image, __channel, __function) do { \
    int c = __channel; \
    int __x0 = (__image).bounds.x1;\
    int __x1 = (__image).bounds.x2;\
    int __y0 = (__image).bounds.y1;\
    int __y1 = (__image).bounds.y2;\
    for(int y = __y0; y < __y1; ++y){ \
        for(int x = __x0; x < __x1; ++x){ \
            __function \
        } \
    } \
} while(0)

#define TRANSFORM(image, fn) ({ \
    image_t ___img = (image); \
    int channels = ___img.channels; \
    double pixel = 0; \
    for(int y = 0; y < ___img.h; ++y){ \
        for(int x = 0; x < ___img.w; ++x){ \
            for(int channel = 0; channel < channels; ++channel){ \
                if(channel == 3) continue; \
                if(PXL_AT(___img, x, y, channel) == VOID_PIXEL) continue; \
                pixel = PXL_AT(___img, x, y, channel); \
                PXL_AT(___img, x, y, channel) = (long double) fn; \
            } \
        } \
    } \
    ___img; \
})

#endif /* __IMGC_MACROS_H__ */