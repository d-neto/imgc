#include "imagec.h"

static image_t create_greater_image_between(image_t im1, image_t im2);

image_t sum(image_t im1, image_t im2){
    return sum_offset(im1, im2, 0, 0);
}

image_t sum_offset(image_t im1, image_t im2, int off_x, int off_y){
    return sum_offset_a(im1, im2, off_x, off_y, 1);
}

image_t sum_offset_a(image_t im1, image_t im2, int off_x, int off_y, int ignore_alpha){
    image_t new_image = create_greater_image_between(im1, im2);
    double value = 0;
    int x2, y2;
    FOREACH_PXL(new_image, {
        value = 0;
        x2 = x - off_x;
        y2 = y - off_y;
        if(ignore_alpha && c == 3) PXL_AT(new_image, x, y, c) = 255;
        if(c < im1.channels && (x < im1.w && y < im1.h)) {
            if(PXL_AT(im1, x, y, c) != VOID_PIXEL)
                value += PXL_AT(im1, x, y, c);
        }
        if((x2 >= 0) && (y2 >= 0) && c < im2.channels && (x2 < im2.w && y2 < im2.h)){
            if(PXL_AT(im2, x2, y2, c) != VOID_PIXEL)
                value += PXL_AT(im2, x2, y2, c);
        }
        PXL_AT(new_image, x, y, c) = value;
    });
    return new_image;
}

image_t sub(image_t im1, image_t im2){
    return sub_offset(im1, im2, 0, 0);
}

image_t sub_offset(image_t im1, image_t im2, int off_x, int off_y){
    return sub_offset_a(im1, im2, off_x, off_y, 1);
}

image_t sub_offset_a(image_t im1, image_t im2, int off_x, int off_y, int ignore_alpha){
    image_t new_image = create_greater_image_between(im1, im2);
    double value = 0;
    int x2, y2;
    FOREACH_PXL(new_image, {
        value = 0;
        x2 = x - off_x;
        y2 = y - off_y;
        if(ignore_alpha && c == 3) PXL_AT(new_image, x, y, c) = 255;
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

image_t multiply_self(image_t src, int times){
    if(times <= 1) return src;
    image_t result = multiply(src, src);
    image_t aux;
    times -= 1;
    for(int i = 1; i < times; ++i){
        aux = multiply(result, result);
        free_image(&result);
        result = aux;
    }
    return result;
}

image_t multiply(image_t im1, image_t im2){
    return multiply_offset(im1, im2, 0, 0);
}

image_t multiply_offset(image_t im1, image_t im2, int off_x, int off_y){
    return multiply_offset_a(im1, im2, off_x, off_y, 1);
}

image_t multiply_offset_a(image_t im1, image_t im2, int off_x, int off_y, int ignore_alpha){
    image_t new_image = create_greater_image_between(im1, im2);
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

image_t blend(image_t im1, image_t im2, double b_alpha){
    return blend_offset(im1, im2, b_alpha, 0, 0);
}

image_t blend_offset(image_t im1, image_t im2, double b_alpha, int off_x, int off_y){
    return blend_offset_a(im1, im2, b_alpha, off_x, off_y, 1);
}

image_t blend_offset_a(image_t im1, image_t im2, double b_alpha, int off_x, int off_y, int ignore_alpha){
    image_t new_image = create_greater_image_between(im1, im2);
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

image_t screen(image_t im1, image_t im2){
    return screen_offset(im1, im2, 0, 0);
}

image_t screen_offset(image_t im1, image_t im2, int off_x, int off_y){
    return screen_offset_a(im1, im2, off_x, off_y, 1);
}

image_t screen_offset_a(image_t im1, image_t im2, int off_x, int off_y, int ignore_alpha){
    image_t new_image = create_greater_image_between(im1, im2);
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

image_t mask(image_t src, image_t mask_src){
    return mask_offset(src, mask_src, 0, 0);
}

image_t mask_offset(image_t src, image_t mask_src, int off_x, int off_y){
    return mask_offset_a(src, mask_src, off_x, off_y, 1);
}

image_t mask_offset_a(image_t src, image_t mask_src, int off_x, int off_y, int ignore_alpha){
    image_t new_image = clone(src);
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

image_t and(image_t im1, image_t im2){
    return and_offset(im1, im2, 0, 0);
}

image_t and_offset(image_t im1, image_t im2, int off_x, int off_y){
    return and_offset_a(im1, im2, off_x, off_y, 1);
}

image_t and_offset_a(image_t im1, image_t im2, int off_x, int off_y, int ignore_alpha){
    image_t new_image = create_greater_image_between(im1, im2);
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

image_t xor(image_t im1, image_t im2){
    return xor_offset(im1, im2, 0, 0);
}

image_t xor_offset(image_t im1, image_t im2, int off_x, int off_y){
    return xor_offset_a(im1, im2, off_x, off_y, 1);
}

image_t xor_offset_a(image_t im1, image_t im2, int off_x, int off_y, int ignore_alpha){
    image_t new_image = create_greater_image_between(im1, im2);
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

static image_t create_greater_image_between(image_t im1, image_t im2){
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

    return create_image(max_w, max_h, max_ch);
}