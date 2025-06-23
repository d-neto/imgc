#include "imagec.h"

image_t erode(image_t source, matrix_t structure){
    
    int center_x = structure.c/2;
    int center_y = structure.r/2;

    image_t result = clone(source);

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

image_t dilate(image_t source, matrix_t structure){
    
    int center_x = structure.c/2;
    int center_y = structure.r/2;

    image_t result = clone(source);

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

image_t opening(image_t source, matrix_t structure){
    return dilate(erode(source, structure), structure);
}

image_t closing(image_t source, matrix_t structure){
    return erode(dilate(source, structure), structure);
}

image_t extract_frontier_spec(image_t source, matrix_t structure){
    return sub(source, erode(source, structure));
}

image_t extract_frontier(image_t source){
    matrix_t structure = MAT((3, 3), 
        1, 1, 1,
        1, 1, 1,
        1, 1, 1
    );
    return extract_frontier_spec(source, structure);
}

image_t bin_inverse(image_t source){
    image_t result = clone(source);
    FOREACH_PXL(result, {
        int pxl = PXL_AT(result, x, y, c);
        PXL_AT(result, x, y, c) = pxl > 0 ? 0 : 255;
    });
    return result;
}

int find_background_seed(image_t image, int *x_out, int *y_out) {
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

image_t fill_holes(image_t source){
    int x = 1, y = 1;
    find_background_seed(source, &x, &y);

    image_t inv = bin_inverse(source);

    image_t filled = flood_fill(inv, x, y, 0x0);

    return sum(source, filled);
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

extract_cc_t extract_cc(image_t source){
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

    FOREACH_PXL_IN_CH(source, 0, {
        PXL_AT(pxl_list, x, y, c).label = 0;
        PXL_AT(pxl_list, x, y, c).x = x;
        PXL_AT(pxl_list, x, y, c).y = y;
    });

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

            if(PXL_AT(pxl_list, x, y, c).label) continue;

            PUSH(positions, PXL_AT(pxl_list, x, y, c));
            
            while(positions.count){
                DEQUEUE(positions, current);

                if(PXL_AT(pxl_list, current.x, current.y, c).label != 0){
                    continue;
                } else {
                    PXL_AT(pxl_list, current.x, current.y, c).label = cc_count + 1;
                }

                for(int i = 0; i < connectivity; i++) {
                    nx = current.x + dx[i];
                    ny = current.y + dy[i];
                    
                    if(nx < 0 || nx >= source.w || ny < 0 || ny >= source.h) continue;
                    
                    if(PXL_AT(source, nx, ny, c) && PXL_AT(pxl_list, nx, ny, c).label == 0){
                        PUSH(positions, PXL_AT(pxl_list, nx, ny, c));
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

void free_extraction(extract_cc_t * extraction){
    for(int i = 0; i < extraction->count; ++i){
        FREE(extraction->items[i].pixels.items);
    }
    FREE(extraction->items);
    extraction->count = 0;
    extraction->items = NULL;
    extraction->capacity = 0;
}

image_t hit_or_miss(image_t source, matrix_t structure){
    
    int tx, ty, mtx_value, pxl_value;
    int hit;

    int center_x = structure.c/2;
    int center_y = structure.r/2;

    image_t result = clone(source);

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