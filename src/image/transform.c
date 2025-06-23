#include "imagec.h"

image_t crop_rect(image_t image, rect_t bounds);
image_t crop_polygon(image_t image, polygon_t bounds);
image_t cut_rect(image_t image, rect_t bounds);
image_t cut_polygon(image_t image, polygon_t bounds);

image_t crop(image_t image, geometry_t geometry_bounds){
    if(geometry_bounds.type == G_RECT){
        rect_t r = geometry_bounds.rect;
        return crop_rect(image, r);
    }
    if(geometry_bounds.type == G_POLYGON){
        polygon_t p = geometry_bounds.polygon;
        return crop_polygon(image, p);
    }
    return (image_t) {0};
}

image_t cut(image_t image, geometry_t geometry_bounds){
    if(geometry_bounds.type == G_RECT){
        rect_t r = geometry_bounds.rect;
        return cut_rect(image, r);
    }
    if(geometry_bounds.type == G_POLYGON){
        polygon_t p = geometry_bounds.polygon;
        return cut_polygon(image, p);
    }
    return (image_t) {0};
}

image_t cut_rect(image_t image, rect_t bounds){
    int swap = 0;

    int x1, x2, y1, y2;
    x1 = bounds.x1;
    x2 = bounds.x2;
    y1 = bounds.y1;
    y2 = bounds.y2;

    if(x1 > x2){
        swap = x1;
        x1 = x2;
        x2 = swap;
    }

    if(y1 > y2){
        swap = y1;
        y1 = y2;
        y2 = swap;
    }

    int h = y2-y1;
    int w = x2-x1;

    image_t cropped = {
        .h = h,
        .w = w,
        .channels = image.channels,
        .data = NULL,
        .bounds = boundaries(0, 0, w, h)
    };
    cropped.data = ALLOC(sizeof(*cropped.data)*h*w*cropped.channels);

    size_t pos = 0;
    int tx = 0, ty = 0;

    for(size_t i = 0; i < h; ++i){
        ty = (y1 + i);
        for(size_t j = 0; j < w; ++j){
            tx = (x1 + j);
            for(size_t c = 0; c < image.channels; ++c){
                pos = (ty * image.w + tx) * image.channels + c;
                if(pos > (image.w * image.h * image.channels)){
                    PXL_AT(cropped, j, i, c) = 0;
                }
                else {
                    PXL_AT(cropped, j, i, c) = image.data[pos];
                    image.data[pos] = VOID_PIXEL;
                }
            }
        }
    }
    return cropped;
}

image_t crop_rect(image_t image, rect_t bounds){
    int swap = 0;

    int x1, x2, y1, y2;
    x1 = bounds.x1;
    x2 = bounds.x2;
    y1 = bounds.y1;
    y2 = bounds.y2;

    if(x1 > x2){
        swap = x1;
        x1 = x2;
        x2 = swap;
    }

    if(y1 > y2){
        swap = y1;
        y1 = y2;
        y2 = swap;
    }

    if (x1 < 0) x1 = 0;
    if (x2 >= image.w) x2 = image.w - 1;
    if (y1 < 0) y1 = 0;
    if (y2 >= image.h) y2 = image.h - 1;

    int h = y2 - y1 + 1;
    int w = x2 - x1 + 1;

    image_t cropped = {
        .h = h,
        .w = w,
        .channels = image.channels,
        .data = NULL,
        .bounds = boundaries(0, 0, w, h)
    };
    cropped.data = ALLOC(sizeof(*cropped.data)*h*w*cropped.channels);

    size_t pos = 0;
    int tx = 0, ty = 0;

    for(size_t i = 0; i < h; ++i){
        ty = (y1 + i);
        for(size_t j = 0; j < w; ++j){
            tx = (x1 + j);
            for(size_t c = 0; c < image.channels; ++c){
                pos = (ty * image.w + tx) * image.channels + c;
                if(pos >= (image.w * image.h * image.channels))
                    PXL_AT(cropped, j, i, c) = 0;
                else
                    PXL_AT(cropped, j, i, c) = image.data[pos];
            }
        }
    }
    return cropped;
}

image_t cut_polygon(image_t image, polygon_t bounds){
    int max_w = 0;
    int min_w = __INT_MAX__;
    int max_h = 0;
    int min_h = __INT_MAX__;

    for(int i = 0; i < bounds.count; ++i){
        if(bounds.points.items[i].x > max_w) max_w = bounds.points.items[i].x;
        if(bounds.points.items[i].x < min_w) min_w = bounds.points.items[i].x;
        if(bounds.points.items[i].y > max_h) max_h = bounds.points.items[i].y;
        if(bounds.points.items[i].y < min_h) min_h = bounds.points.items[i].y;
    }

    int h = max_h - min_h;
    int w = max_w - min_w;

    image_t cropped = {
        .h = h,
        .w = w,
        .channels = image.channels,
        .data = NULL,
        .bounds = boundaries(0, 0, w, h)
    };

    cropped.data = ALLOC(sizeof(*cropped.data)*max_w*max_h*cropped.channels);
    size_t pos = 0;
    int tx = 0, ty = 0;

    for(size_t i = 0; i < h; ++i){
        ty = (min_h + i);
        for(size_t j = 0; j < w; ++j){
            tx = (min_w + j);
            if(!polygon_is_point_inside(bounds, vec2(tx, ty))){
                for(size_t c = 0; c < image.channels; ++c) PXL_AT(cropped, j, i, c) = VOID_PIXEL;
                if(image.channels == 4) PXL_AT(cropped, j, i, 3) = 0;
                continue;
            }
            for(size_t c = 0; c < image.channels; ++c){
                pos = (ty * image.w + tx) * image.channels + c;
                if(pos > (image.w * image.h * image.channels)){
                    PXL_AT(cropped, j, i, c) = 0;
                }
                else {
                    PXL_AT(cropped, j, i, c) = image.data[pos];
                    image.data[pos] = VOID_PIXEL;
                }
            }
        }
    }
    return cropped;
}

image_t crop_polygon(image_t image, polygon_t bounds){
    int max_w = 0;
    int min_w = __INT_MAX__;
    int max_h = 0;
    int min_h = __INT_MAX__;

    for(int i = 0; i < bounds.count; ++i){
        if(bounds.points.items[i].x > max_w) max_w = bounds.points.items[i].x;
        if(bounds.points.items[i].x < min_w) min_w = bounds.points.items[i].x;
        if(bounds.points.items[i].y > max_h) max_h = bounds.points.items[i].y;
        if(bounds.points.items[i].y < min_h) min_h = bounds.points.items[i].y;
    }

    int h = max_h - min_h;
    int w = max_w - min_w;

    image_t cropped = {
        .h = h,
        .w = w,
        .channels = image.channels,
        .data = NULL,
        .bounds = boundaries(0, 0, w, h)
    };

    cropped.data = ALLOC(sizeof(*cropped.data)*max_w*max_h*cropped.channels);
    size_t pos = 0;
    int tx = 0, ty = 0;

    for(size_t i = 0; i < h; ++i){
        ty = (min_h + i);
        for(size_t j = 0; j < w; ++j){
            tx = (min_w + j);
            if(!polygon_is_point_inside(bounds, vec2(tx, ty))){
                for(size_t c = 0; c < image.channels; ++c) PXL_AT(cropped, j, i, c) = VOID_PIXEL;
                if(image.channels == 4) PXL_AT(cropped, j, i, 3) = 0;
                continue;
            }
            for(size_t c = 0; c < image.channels; ++c){
                pos = (ty * image.w + tx) * image.channels + c;
                if(pos > (image.w * image.h * image.channels))
                    PXL_AT(cropped, j, i, c) = 0;
                else
                    PXL_AT(cropped, j, i, c) = image.data[pos];
            }
        }
    }
    return cropped;
}

image_t resize(image_t src, int w, int h){
    image_t image = create_image(w, h, src.channels);

    double wf = (double) src.w / (double) w;
    double hf = (double) src.h / (double) h;

    int xi, yi;

    for(int y = 0; y < h; ++y){
        for(int x = 0; x < w; ++x){
            xi = (int)(x * wf);
            yi = (int)(y * hf);
            if (xi >= src.w) xi = src.w - 1;
            if (yi >= src.h) yi = src.h - 1;
            for(int c = 0; c < image.channels; ++c){
                PXL_AT(image, x, y, c) = PXL_AT(src, xi, yi, c);
            }
        }
    }

    return image;
}