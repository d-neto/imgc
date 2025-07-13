#include "imgc.h"

static img_t create_greater_image_between(img_t im1, img_t im2);

img_t img_sum(img_t im1, img_t im2){
    return img_sumoffs(im1, im2, 0, 0);
}

img_t img_sumoffs(img_t im1, img_t im2, int off_x, int off_y){
    return img_sum__spec(im1, im2, off_x, off_y, 1);
}

img_t img_sum__spec(img_t im1, img_t im2, int off_x, int off_y, int ignore_alpha){
    img_t new_image = create_greater_image_between(im1, im2);
    double value = 0;
    int x2, y2;
    FOREACH_PXL(new_image, {
        value = 0;
        x2 = x - off_x;
        y2 = y - off_y;
        if(ignore_alpha && c == 3) PXL_AT(new_image, x, y, c) = 255;
        if((x < im1.w && y < im1.h)) {
            if(PXL_AT(im1, x, y, c) != VOID_PIXEL){
                if(c < im1.channels) value += PXL_AT(im1, x, y, c);
                else value += PXL_AT(im1, x, y, 0);
            }
        }
        if((x2 >= 0) && (y2 >= 0) && (x2 < im2.w && y2 < im2.h)){
            if(PXL_AT(im2, x2, y2, c) != VOID_PIXEL){
                if(c < im2.channels) value += PXL_AT(im2, x2, y2, c);
                else value += PXL_AT(im2, x2, y2, 0);
            }
        }
        PXL_AT(new_image, x, y, c) = value;
    });
    return new_image;
}

img_t img_sub(img_t im1, img_t im2){
    return img_suboffs(im1, im2, 0, 0);
}

img_t img_suboffs(img_t im1, img_t im2, int off_x, int off_y){
    return img_sub__spec(im1, im2, off_x, off_y, 1);
}

img_t img_sub__spec(img_t im1, img_t im2, int off_x, int off_y, int ignore_alpha){
    img_t new_image = create_greater_image_between(im1, im2);
    double value = 0;
    int x2, y2;
    FOREACH_PXL(new_image, {
        value = 0;
        x2 = x - off_x;
        y2 = y - off_y;
        if(ignore_alpha && c == 3) {
            PXL_AT(new_image, x, y, c) = 255;
            continue;
        }
        if(c < im1.channels && (x < im1.w && y < im1.h)) {
            if(PXL_AT(im1, x, y, c) != VOID_PIXEL)
                value += PXL_AT(im1, x, y, c);
        }
        if((x2 >= 0) && (y2 >= 0) && c < im2.channels && (x2 < im2.w && y2 < im2.h)){
            if(PXL_AT(im2, x2, y2, c) != VOID_PIXEL)
                value -= PXL_AT(im2, x2, y2, c);
        }
        PXL_AT(new_image, x, y, c) = value;
    });
    return new_image;
}

img_t img_multiply_self(img_t src, int times){
    if(times <= 1) return src;
    img_t result = img_multiply(src, src);
    img_t aux;
    times -= 1;
    for(int i = 1; i < times; ++i){
        aux = img_multiply(result, result);
        img_free(&result);
        result = aux;
    }
    return result;
}

img_t img_multiply(img_t im1, img_t im2){
    return img_multiplyoffs(im1, im2, 0, 0);
}

img_t img_multiplyoffs(img_t im1, img_t im2, int off_x, int off_y){
    return img_multiply__spec(im1, im2, off_x, off_y, 1);
}

img_t img_multiply__spec(img_t im1, img_t im2, int off_x, int off_y, int ignore_alpha){
    img_t new_image = create_greater_image_between(im1, im2);
    double value = 0;
    int x2, y2;
    FOREACH_PXL(new_image, {
        value = 0;
        x2 = x - off_x;
        y2 = y - off_y;
        if(ignore_alpha && c == 3) PXL_AT(new_image, x, y, c) = 255.0;
        if(c < im1.channels && (x < im1.w && y < im1.h)) {
            if(PXL_AT(im1, x, y, c) != VOID_PIXEL)
                value += PXL_AT(im1, x, y, c);
        }
        if((x2 >= 0) && (y2 >= 0) && c < im2.channels && (x2 < im2.w && y2 < im2.h)){
            if(PXL_AT(im2, x2, y2, c) != VOID_PIXEL)
                value = ((value) * (PXL_AT(im2, x2, y2, c)))/255.0;
        }
        PXL_AT(new_image, x, y, c) = value;
    });
    return new_image;
}

img_t img_blend(img_t im1, img_t im2, double b_alpha){
    return img_blendoffs(im1, im2, b_alpha, 0, 0);
}

img_t img_blendoffs(img_t im1, img_t im2, double b_alpha, int off_x, int off_y){
    return img_blend__spec(im1, im2, b_alpha, off_x, off_y, 1);
}

img_t img_blend__spec(img_t im1, img_t im2, double b_alpha, int off_x, int off_y, int ignore_alpha){
    img_t new_image = create_greater_image_between(im1, im2);
    double v1 = 0, v2 = 0;
    int x2, y2;
    double alpha = 1 - b_alpha;
    FOREACH_PXL(new_image, {
        x2 = x - off_x;
        y2 = y - off_y;
        v1 = -1;
        v2 = -1;
        if(ignore_alpha && c == 3) PXL_AT(new_image, x, y, c) = 255;
        if(c < im1.channels && (x < im1.w && y < im1.h)) {
            if(PXL_AT(im1, x, y, c) != VOID_PIXEL)
                v1 = PXL_AT(im1, x, y, c);
        }
        if((x2 >= 0) && (y2 >= 0) && c < im2.channels && (x2 < im2.w && y2 < im2.h)){
            if(PXL_AT(im2, x2, y2, c) != VOID_PIXEL)
                v2 = PXL_AT(im2, x2, y2, c);
        }
        if(v1 == -1) v1 = v2;
        if(v2 == -1) v2 = v1;
        if(v2 == -1) {
            v1 = 0;
            v2 = 0;
        }
        PXL_AT(new_image, x, y, c) = (alpha * v1) + (b_alpha * v2);
    });
    return new_image;
}

img_t img_screen(img_t im1, img_t im2){
    return img_screenoffs(im1, im2, 0, 0);
}

img_t img_screenoffs(img_t im1, img_t im2, int off_x, int off_y){
    return img_screen__spec(im1, im2, off_x, off_y, 1);
}

img_t img_screen__spec(img_t im1, img_t im2, int off_x, int off_y, int ignore_alpha){
    img_t new_image = create_greater_image_between(im1, im2);
    double v1 = 0, v2 = 0;
    int x2, y2;
    FOREACH_PXL(new_image, {
        x2 = x - off_x;
        y2 = y - off_y;
        v1 = -1;
        v2 = -1;
        if(ignore_alpha && c == 3) PXL_AT(new_image, x, y, c) = 255;
        if(c < im1.channels && (x < im1.w && y < im1.h)) {
            if(PXL_AT(im1, x, y, c) != VOID_PIXEL)
                v1 = PXL_AT(im1, x, y, c);
        }
        if((x2 >= 0) && (y2 >= 0) && c < im2.channels && (x2 < im2.w && y2 < im2.h)){
            if(PXL_AT(im2, x2, y2, c) != VOID_PIXEL)
                v2 = PXL_AT(im2, x2, y2, c);
        }
        if(v1 == -1 && v2 != -1) PXL_AT(new_image, x, y, c) = v2;
        else if(v1 == -1) PXL_AT(new_image, x, y, c) = 0;
        else PXL_AT(new_image, x, y, c) = 255.0 - ((255.0 - v1)*(255.0 - v2))/255;
    });
    return new_image;
}

img_t img_mask(img_t src, img_t mask_src){
    return img_maskoffs(src, mask_src, 0, 0);
}

img_t img_maskoffs(img_t src, img_t mask_src, int off_x, int off_y){
    return img_mask__spec(src, mask_src, off_x, off_y, 1);
}

img_t img_mask__spec(img_t src, img_t mask_src, int off_x, int off_y, int ignore_alpha){
    img_t new_image = img_clone(src);
    int inside_mask;
    int x2, y2;
    FOREACH_PXL(new_image, {
        x2 = x - off_x;
        y2 = y - off_y;
        if(ignore_alpha && c == 3) PXL_AT(new_image, x, y, c) = 255;
        inside_mask = (x2 >= 0) && (y2 >= 0) && c < mask_src.channels && (x2 < mask_src.w && y2 < mask_src.h);

        if(inside_mask && PXL_AT(mask_src, x2, y2, c) == VOID_PIXEL)
            inside_mask = 0;

        if(!inside_mask) PXL_AT(new_image, x, y, c) = 0;
    });
    return new_image;
}

img_t img_and(img_t im1, img_t im2){
    return img_andoffs(im1, im2, 0, 0);
}

img_t img_andoffs(img_t im1, img_t im2, int off_x, int off_y){
    return img_and__spec(im1, im2, off_x, off_y, 1);
}

img_t img_and__spec(img_t im1, img_t im2, int off_x, int off_y, int ignore_alpha){
    img_t new_image = create_greater_image_between(im1, im2);
    unsigned char v1 = 0, v2 = 0;
    int x2, y2;
    FOREACH_PXL(new_image, {
        x2 = x - off_x;
        y2 = y - off_y;
        if(ignore_alpha && c == 3) PXL_AT(new_image, x, y, c) = 255;

        if(c < im1.channels && (x < im1.w && y < im1.h)){
            v1 = CLAMP(PXL_AT(im1, x, y, c), 0, 255);
        }
        else v1 = 0;
        if((x2 >= 0) && (y2 >= 0) && c < im2.channels && (x2 < im2.w && y2 < im2.h)){
            v2 = CLAMP(PXL_AT(im2, x2, y2, c), 0, 255);
        }
        else v2 = 0;
        PXL_AT(new_image, x, y, c) = v1 & v2;
    });
    return new_image;
}

img_t img_xor(img_t im1, img_t im2){
    return img_xoroffs(im1, im2, 0, 0);
}

img_t img_xoroffs(img_t im1, img_t im2, int off_x, int off_y){
    return img_xor__spec(im1, im2, off_x, off_y, 1);
}

img_t img_xor__spec(img_t im1, img_t im2, int off_x, int off_y, int ignore_alpha){
    img_t new_image = create_greater_image_between(im1, im2);
    unsigned char v1 = 0, v2 = 0;
    int x2, y2;
    FOREACH_PXL(new_image, {
        x2 = x - off_x;
        y2 = y - off_y;
        if(ignore_alpha && c == 3) PXL_AT(new_image, x, y, c) = 255;

        if(c < im1.channels && (x < im1.w && y < im1.h)){
            v1 = CLAMP(PXL_AT(im1, x, y, c), 0, 255);
        }
        else v1 = 0;
        if((x2 >= 0) && (y2 >= 0) && c < im2.channels && (x2 < im2.w && y2 < im2.h)){
            v2 = CLAMP(PXL_AT(im2, x2, y2, c), 0, 255);
        }
        else v2 = 0;
        PXL_AT(new_image, x, y, c) = v1 ^ v2;
    });
    return new_image;
}

static img_t create_greater_image_between(img_t im1, img_t im2){
    int max_w, min_w, min_h, max_h, max_ch;
    max_w = im1.w;
    max_h = im1.h;
    min_w = im1.w;
    min_h = im1.h;
    max_ch = im1.channels;

    if(im2.channels > max_ch) max_ch = im2.channels;

    if(im2.w > max_w) max_w = im2.w;
    if(im2.h > max_h) max_h = im2.h;

    if(im2.w < min_w) min_w = im2.w;
    if(im2.h < min_h) min_h = im2.h;

    if(im2.h < min_h) min_h = im2.h;

    return img_create(max_w, max_h, max_ch);
}