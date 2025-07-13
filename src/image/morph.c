#include "imagec.h"

img_t img_erode_bin(img_t source, mat_t structure){
    
    int center_x = structure.c/2;
    int center_y = structure.r/2;

    img_t result = img_clone(source);

    int tx, ty;
    int keep_pixel;
    int mtx_value;
    int pxl_value;

    FOREACH_PXL(source, {
        keep_pixel = 1;

        for(int i = 0; i < structure.r; ++i){
            for(int j = 0; j < structure.c; ++j){
                tx = x + j - center_x;
                ty = y + i - center_y;

                if(tx < 0 || tx >= source.w || ty < 0 || ty >= source.h) continue;

                mtx_value = MAT_AT(structure, i, j);
                pxl_value = PXL_AT(source, tx, ty, c);
                if(mtx_value && !pxl_value){
                    keep_pixel = 0;
                    break;
                }
            }
            if(!keep_pixel) break;
        }
        if(!keep_pixel) PXL_AT(result, x, y, c) = 0;
    });

    return result;
}


img_t img_dilate_bin(img_t source, mat_t structure){
    
    int center_x = structure.c/2;
    int center_y = structure.r/2;

    img_t result = img_clone(source);

    int tx, ty;
    int add_pixel;
    int mtx_value;
    int pxl_value;

    FOREACH_PXL(source, {
        add_pixel = 0;

        for(int i = 0; i < structure.r; ++i){
            for(int j = 0; j < structure.c; ++j){
                tx = x + j - center_x;
                ty = y + i - center_y;

                if(tx < 0 || tx >= source.w || ty < 0 || ty >= source.h) continue;

                mtx_value = MAT_AT(structure, i, j);
                pxl_value = PXL_AT(source, tx, ty, c);
                if(mtx_value >= 0 && pxl_value){
                    add_pixel = 1;
                    break;
                }
            }
            if(add_pixel) break;
        }
        if(add_pixel) PXL_AT(result, x, y, c) = 255;
    });

    return result;
}

img_t img_erode(img_t source, mat_t structure){
    
    int center_x = structure.c/2;
    int center_y = structure.r/2;

    int w = source.bounds.x2 - source.bounds.x1;
    int h = source.bounds.y2 - source.bounds.y1;
    img_t result = img_create(w, h, source.channels);

    int tx, ty;
    int mtx_value;

    int pxl_value;
    int min_pxl;

    int off_x = source.bounds.x1;
    int off_y = source.bounds.y1;

    FOREACH_PXL(source, {
        min_pxl = __INT_MAX__;
        if(c == 3){
            PXL_AT(result, x - off_x, y - off_y, c) = PXL_AT(source, x - off_x, y - off_y, c);
            continue;
        }
        for(int i = 0; i < structure.r; ++i){
            for(int j = 0; j < structure.c; ++j){
                tx = x + j - center_x;
                ty = y + i - center_y;
                if(tx < 0 || tx >= source.w || ty < 0 || ty >= source.h) continue;

                mtx_value = MAT_AT(structure, i, j);
                pxl_value = PXL_AT(source, tx, ty, c);
                if(mtx_value && pxl_value < min_pxl) min_pxl = pxl_value;
                if(min_pxl <= 0) break;
            }
        }
        
        PXL_AT(result, x - off_x, y - off_y, c) = min_pxl;
    });

    return result;
}

img_t img_dilate(img_t source, mat_t structure){
    
    int center_x = structure.c/2;
    int center_y = structure.r/2;

    int w = source.bounds.x2 - source.bounds.x1;
    int h = source.bounds.y2 - source.bounds.y1;
    img_t result = img_create(w, h, source.channels);

    int tx, ty;

    int max_pxl;
    int mtx_value;
    int pxl_value;

    int off_x = source.bounds.x1;
    int off_y = source.bounds.y1;

    FOREACH_PXL(source, {
        max_pxl = 0;
        if(c == 3){
            PXL_AT(result, x - off_x, y - off_y, c) = PXL_AT(source, x - off_x, y - off_y, c);
            continue;
        }
        for(int i = 0; i < structure.r; ++i){
            for(int j = 0; j < structure.c; ++j){
                tx = x + j - center_x;
                ty = y + i - center_y;

                if(tx < 0 || tx >= source.w || ty < 0 || ty >= source.h) continue;

                mtx_value = MAT_AT(structure, i, j);
                pxl_value = PXL_AT(source, tx, ty, c);
                if(mtx_value && pxl_value > max_pxl) max_pxl = pxl_value;
                if(max_pxl >= 255) break;
            }
        }
        
        PXL_AT(result, x - off_x, y - off_y, c) = max_pxl;
    });

    return result;
}

img_t img_open(img_t source, mat_t structure){
    img_t erosion = img_erode(source, structure);
    img_t result = img_dilate(erosion, structure);
    img_free(&erosion);
    return result;
}

img_t img_close(img_t source, mat_t structure){
    img_t dilatation = img_dilate(source, structure);
    img_t result = img_erode(dilatation, structure);
    img_free(&dilatation);
    return result;
}

img_t extract_frontier_spec(img_t source, mat_t structure){
    return img_sub(source, img_erode(source, structure));
}

img_t extract_frontier(img_t source){
    mat_t structure = MAT(3, 3, 
        1, 1, 1,
        1, 1, 1,
        1, 1, 1
    );
    return extract_frontier_spec(source, structure);
}

img_t bin_inverse(img_t source){
    img_t result = img_clone(source);
    FOREACH_PXL(result, {
        int pxl = PXL_AT(result, x, y, c);
        PXL_AT(result, x, y, c) = pxl > 0 ? 0 : 255;
    });
    return result;
}

int find_background_seed(img_t image, int *x_out, int *y_out) {
    int width = image.w;
    int height = image.h;

    for (int x = 0; x < width; ++x) {
        if (PXL_AT(image, x, 0, 0) == 0) {
            *x_out = x;
            *y_out = 0;
            return 1;
        }
        if (PXL_AT(image, x, height - 1, 0) == 0) {
            *x_out = x;
            *y_out = height - 1;
            return 1;
        }
    }

    for (int y = 0; y < height; ++y) {
        if (PXL_AT(image, 0, y, 0) == 0) {
            *x_out = 0;
            *y_out = y;
            return 1;
        }
        if (PXL_AT(image, width - 1, y, 0) == 0) {
            *x_out = width - 1;
            *y_out = y;
            return 1;
        }
    }

    return 0;
}

img_t img_fillholes(img_t source){
    int x = 1, y = 1;
    find_background_seed(source, &x, &y);

    img_t inv = bin_inverse(source);

    img_t filled = img_floodfl(inv, x, y, 0x0);

    return img_sum(source, filled);
}

typedef struct extracted_pxl {
    int label;
    int x;
    int y;
} extrpxl_t;

typedef struct extracted_pxl_arr {
    int count;
    int capacity;
    extrpxl_t * items;
} extrpxl_arr;

typedef struct pixel_list {
    int w;
    int h;
    int channels;
    extrpxl_t * data;
} pixel_list_t;

extract_cc_t img_extract_cc(img_t source){
    extract_cc_t extraction = {
        .count = 0,
        .capacity = 0,
        .conn_type = PXLCONN_8,
        .items = NULL
    };

    pixel_list_t pxl_list = {
        .channels = source.channels,
        .h = source.h,
        .w = source.w,
        .data = ALLOC(sizeof(*pxl_list.data) * source.h * source.w)
    };

    int __x0 = source.bounds.x1;
    int __x1 = source.bounds.x2;
    int __y0 = source.bounds.y1;
    int __y1 = source.bounds.y2;
    for(int y = __y0; y < __y1; ++y){ 
        for(int x = __x0; x < __x1; ++x){ 
            LIST_AT(pxl_list, x, y).label = 0;
            LIST_AT(pxl_list, x, y).x = x;
            LIST_AT(pxl_list, x, y).y = y;
        }
    }

    int cc_count = 0;

    extrpxl_arr positions = {.count = 0, .capacity = (10), .items=ALLOC(sizeof(extrpxl_t)*(10)) };
    extrpxl_t current;


    int dx4[4] = {0, -1, 1, 0};
    int dy4[4] = {-1, 0, 0, 1};
    int dx8[8] = {-1, 0, 1, -1, 1, -1, 0, 1};
    int dy8[8] = {-1, -1, -1, 0, 0, 1, 1, 1};

    int * dx = dx4;
    int * dy = dy4;
    int connectivity = 4;

    if(extraction.conn_type == PXLCONN_8){
        connectivity = 8;
        dx = dx8;
        dy = dy8;
    }

    int nx, ny;
    FOREACH_PXL_IN_CH(source, 0, {
        if(PXL_AT(source, x, y, c)){

            if(LIST_AT(pxl_list, x, y).label) continue;

            PUSH(positions, LIST_AT(pxl_list, x, y));
            
            while(positions.count){
                DEQUEUE(positions, current);

                if(LIST_AT(pxl_list, current.x, current.y).label != 0){
                    continue;
                } else {
                    LIST_AT(pxl_list, current.x, current.y).label = cc_count + 1;
                }

                for(int i = 0; i < connectivity; i++) {
                    nx = current.x + dx[i];
                    ny = current.y + dy[i];
                    
                    if(nx < 0 || nx >= source.w || ny < 0 || ny >= source.h) continue;
                    
                    if(PXL_AT(source, nx, ny, c) && LIST_AT(pxl_list, nx, ny).label == 0){
                        PUSH(positions, LIST_AT(pxl_list, nx, ny));
                    }
                }
            }

            cc_count += 1;
        }
    });

    int size = pxl_list.h*pxl_list.w;
    int min_x, min_y, max_x, max_y;

    for(int i = 0; i < cc_count; ++i){
        conn_component_t component = {
            .label = i,
            .pixels = VEC2_ARR(0),
            .cx = 0,
            .cy = 0
        };

        min_x = source.w;
        min_y = source.h;
        max_x = 0;
        max_y = 0;

        for(int k = 0; k < size; ++k){
            if(pxl_list.data[k].label == i+1){
                component.area += 1;
                component.cx += pxl_list.data[k].x;
                component.cy += pxl_list.data[k].y;

                APPEND(component.pixels, vec2(pxl_list.data[k].x, pxl_list.data[k].y));

                if(pxl_list.data[k].x < min_x) min_x = pxl_list.data[k].x;
                if(pxl_list.data[k].x > max_x) max_x = pxl_list.data[k].x;

                if(pxl_list.data[k].y < min_y) min_y = pxl_list.data[k].y;
                if(pxl_list.data[k].y > max_y) max_y = pxl_list.data[k].y;
            }
        }

        component.bounding_box = boundaries(min_x, min_y, max_x, max_y);
        component.cx /= component.pixels.count;
        component.cy /= component.pixels.count;

        APPEND(extraction, component);
    }

    FREE(pxl_list.data);
    FREE(positions.items);

    return extraction;
}

void img_free_extraction(extract_cc_t * extraction){
    for(int i = 0; i < extraction->count; ++i){
        FREE(extraction->items[i].pixels.items);
    }
    FREE(extraction->items);
    extraction->count = 0;
    extraction->items = NULL;
    extraction->capacity = 0;
}

img_t img_hit_or_miss(img_t source, mat_t structure){
    
    int tx, ty, mtx_value, pxl_value;
    int hit;

    int center_x = structure.c/2;
    int center_y = structure.r/2;

    img_t result = img_clone(source);

    FOREACH_PXL(result, {

        hit = 1;

        for(int i = 0; i < structure.r; ++i){
            for(int j = 0; j < structure.c; ++j){
                tx = x + j - center_x;
                ty = y + i - center_y;

                if(tx < 0 || tx >= source.w || ty < 0 || ty >= source.h) continue;

                mtx_value = MAT_AT(structure, i, j);
                pxl_value = PXL_AT(source, tx, ty, c);

                if(mtx_value == 0) continue;

                if((mtx_value == -1 && pxl_value) || (mtx_value == 1 && !pxl_value)){
                    hit = 0;
                    break;
                }
            }

            if(!hit) break;
        }

        if(!hit) 
            PXL_AT(result, x, y, c) = 0;
    });

    return result;
}