
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "imagec.h"

typedef unsigned char u8_t;
typedef struct pixel {
    union
    {
        u8_t lum;
        struct {
            u8_t r;
            u8_t g;
            u8_t b;
            u8_t a;
        };
    };
} pixel_t;

pixel_t ** data_to_pixel(int w, int h, void * data, int channels){
    pixel_t ** pixels = (pixel_t **) ALLOC(sizeof(*pixels)*h);
    assert(pixels);
    for(int i = 0; i < h; ++i){
        pixels[i] = (pixel_t *) ALLOC(sizeof(**pixels)*w);
        assert(pixels[i]);
    }
    size_t offset_pxl = sizeof(u8_t);
    int c = 0;
    int c_offset = 0;
    void * pixel;
    for(int i = 0; i < h; ++i){
        for(int j = 0; j < w; ++j){
            c = channels;
            while(c > 0){
                pixel = &pixels[i][j];
                c_offset = (channels-c);
                *(u8_t *)(pixel+(offset_pxl*c_offset)) = *(u8_t *)(data+((i*w + j)*channels+c_offset));
                c -= 1;
            }
        }
    }
    return pixels;
}

image_t create_image(int w, int h, int ch){
    image_t image = {
        .h = h,
        .w = w,
        .channels = ch,
        .bounds = boundaries(0, 0, w, h)
    };
    image.data = ALLOC(sizeof(*image.data)*image.w*image.h*image.channels);
    assert(image.data);

    for(int i = 0; i < image.w*image.h*image.channels; ++i)
        image.data[i] = 0;

    return image;
}

image_t img_load(char *_fmt_filename, ...){

    char filename[256] = {0};
    va_list args;
    va_start(args, _fmt_filename);
    vsnprintf(filename, sizeof(filename), _fmt_filename, args);
    va_end(args);

    image_t image = {0};
    void * data = stbi_load(filename, &image.w, &image.h, &image.channels, 0);

    if(!data){
        fprintf(stderr, "Error to load image \"%s\".\n", filename);
        exit(-1);
    }

    image.data = ALLOC(sizeof(*image.data)*image.w*image.h*image.channels);
    assert(image.data);

    int size = image.w*image.h*image.channels;
    unsigned char * value;
    for(int i = 0; i < size; ++i){
        value = data+i;
        image.data[i] = *value;
    }
    FREE(data);

    image.bounds = boundaries(0, 0, image.w, image.h);
    image._slice = 0;

    return image;
}

void img_write(image_t image, char *_fmt_filename, ...){

    char filename[256] = {0};
    va_list args;
    va_start(args, _fmt_filename);
    vsnprintf(filename, sizeof(filename), _fmt_filename, args);
    va_end(args);  

    int size = image.w*image.h*image.channels;
    unsigned char * data = ALLOC(sizeof(*data)*size);
    assert(data);

    long double value = 0;
    for(int i = 0; i < size; ++i){
        value = image.data[i];
        if(value > 255) value = 255;
        if(value < 0) value = 0;
        data[i] = value;
    }
    stbi_write_png(filename, image.w, image.h, image.channels, data, 0);
    FREE(data);
}

void free_image(image_t * image){
    image->h = 0;
    image->w = 0;
    image->channels = 0;
    if(image->data) FREE(image->data);
    image->data = NULL;
}

image_t from_bounds(image_t source){
    int w = source.bounds.x2 - source.bounds.x1;
    int h = source.bounds.y2 - source.bounds.y1;
    image_t new_image = create_image(w, h, source.channels);
    return new_image;
}

image_t clone(image_t source){
    image_t new_image = {
        .channels = source.channels,
        .h = source.h,
        .w = source.w,
        .data = ALLOC(sizeof(*source.data)*source.w*source.h*source.channels),
        .bounds = source.bounds
    };
    assert(new_image.data);
    int size = source.w*source.h*source.channels;
    for(int i = 0; i < size; ++i) new_image.data[i] = source.data[i];
    return new_image;
}

image_t rgb2gray(image_t image){
    if(image.channels < 3) return (image_t){0};
    image_t new_image = {
        .h = image.h,
        .w = image.w,
        .data = NULL,
        .channels = 1,
        .bounds = image.bounds
    };

    new_image.data = ALLOC(sizeof(*new_image.data)*new_image.h*new_image.w);
    assert(new_image.data);

    FOREACH_PXL(new_image, {
        if(PXL_AT(image, x, y, c) == VOID_PIXEL){
            PXL_AT(new_image, x, y, c) = VOID_PIXEL;
            continue;
        }
        /**
         * new_image.data[i] = (image.data[(i*image.channels)] + image.data[(i*image.channels)+1] + image.data[(i*image.channels)+2])/3;
         */
        double red = PXL_AT(image, x, y, 0);
        double green = PXL_AT(image, x, y, 1);
        double blue = PXL_AT(image, x, y, 2);
        PXL_AT(new_image, x, y, c) = 0.3*red + 0.59*green + 0.11*blue;
    });

    return new_image;
}

void upgrade(image_t * image, int channels){
    if(channels <= image->channels) return;
    image_t clonned = clone(*image);

    image->w = clonned.w;
    image->h = clonned.h;
    image->channels = channels;
    image->data = ALLOC(sizeof(*image->data)*clonned.h*clonned.w*channels);
    image->bounds = clonned.bounds;
    assert(image->data);

    FOREACH_PXL(*image, {
        if(c < clonned.channels)
            PXL_AT(*image, x, y, c) = PXL_AT(clonned, x, y, c);
        else
            PXL_AT(*image, x, y, c) = 0;
    });

    free_image(&clonned);
}

image_t paste(image_t src, image_t dest, int x_offset, int y_offset){
    if(dest.channels < src.channels) upgrade(&dest, src.channels);

    int tx = 0, ty = 0;

    FOREACH_PXL(src, {
        channels = dest.channels;
        tx = (x + x_offset);
        ty = (y + y_offset);

        if(tx >= dest.w || ty >= dest.h) continue;
        if(x >= src.w || y >= src.h) continue;

        if(src.channels-1 >= c) {
            if(PXL_AT(src, x, y, c) == VOID_PIXEL) continue; 
            PXL_AT(dest, tx, ty, c) = PXL_AT(src, x, y, c);
            continue;
        } else {
            PXL_AT(dest, tx, ty, c) = PXL_AT(src, x, y, 0);
        }
    });
    return dest;
}

image_t normalize_nonzero_pixels(image_t input) {
    image_t output = clone(input);
    double min = 1e9, max = -1e9;
    for (int y = 0; y < input.h; ++y) {
        for (int x = 0; x < input.w; ++x) {
            double val = PXL_AT(input, x, y, 0);
            if (val > 0.0) {
                if (val < min) min = val;
                if (val > max) max = val;
            }
        }
    }
    double range = max - min;
    if (range == 0.0) range = 1.0;
    for (int y = 0; y < input.h; ++y) {
        for (int x = 0; x < input.w; ++x) {
            double val = PXL_AT(input, x, y, 0);
            if (val > 0.0) {
                PXL_AT(output, x, y, 0) = 255.0 * (val - min) / range;
            } else {
                PXL_AT(output, x, y, 0) = 0.0;
            }
        }
    }
    return output;
}

image_t im2double(image_t image){
    return TRANSFORM(clone(image), { pixel = pixel/255.0 });
}

image_t with_all(double value, int w, int h, int channels){
    image_t i = create_image(w, h, channels);
    FOREACH_PXL(i, {
        PXL_AT(i, x, y, c) = value;
    });
    return i;
}

image_t zeros(int w, int h, int channels){
    return with_all(0, w, h, channels);
}

image_t fill_all(image_t src, double value){
    image_t new_image = clone(src);
    FOREACH_PXL(new_image, {
        if(PXL_AT(new_image, x, y, c) != VOID_PIXEL)
            PXL_AT(new_image, x, y, c) = value;
    });
    return new_image;
}

image_t fill(image_t src, int x, int y, double value){
    image_t new_image = clone(src);
    char * visited = ALLOC(new_image.w * new_image.h * sizeof(*visited));
    memset(visited, 0, new_image.w * new_image.h);

    vec2_arr positions = VEC2_ARR(10);
    PUSH(positions, vec2(x, y));
    
    int match = 0;
    int dx[] = {0, -1, 1, 0};
    int dy[] = {-1, 0, 0, 1};
    vec2_t curr;
    int i = 0, nx, ny;
    
    while(positions.count > 0){
        DEQUEUE(positions, curr);
        if(curr.x >= new_image.w || curr.y >= new_image.h || curr.x < 0 || curr.y < 0)
            continue;

        i = curr.y * new_image.w + curr.x;
        if(visited[i]) continue;
        visited[i] = 1;

        match = 1;
        for(int c = 0; c < new_image.channels; c++){
            if(PXL_AT(src, curr.x, curr.y, c) != PXL_AT(src, x, y, c)){
                match = 0;
                break;
            }
        }
        if(!match) continue;

        for(int c = 0; c < new_image.channels; c++)
            PXL_AT(new_image, curr.x, curr.y, c) = value;
    
        for(int i = 0; i < 4; i++) {
            nx = curr.x + dx[i];
            ny = curr.y + dy[i];
            PUSH(positions, vec2(nx, ny));
        }
    }

    FREE(visited);
    return new_image;
}


image_t flood_fill(image_t src, int x, int y, hex_t color){
    image_t new_image = clone(src);
    char * visited = ALLOC(new_image.w * new_image.h * sizeof(*visited));
    memset(visited, 0, new_image.w * new_image.h);

    vec2_arr positions = VEC2_ARR(10);
    PUSH(positions, vec2(x, y));
    
    int match = 0;
    int dx[] = {0, -1, 1, 0};
    int dy[] = {-1, 0, 0, 1};
    vec2_t curr;
    int i = 0, nx, ny;

    int a = (color >> 24) & 0xFF;
    int r = (color >> 16) & 0xFF;
    int g = (color >> 8) & 0xFF;
    int b = color & 0xFF;
    color = (a << 24) | (b << 16) | (g << 8) | (r);

    while(positions.count > 0){
        DEQUEUE(positions, curr);
        if(curr.x >= new_image.w || curr.y >= new_image.h || curr.x < 0 || curr.y < 0)
            continue;

        i = curr.y * new_image.w + curr.x;
        if(visited[i]) continue;
        visited[i] = 1;

        match = 1;
        for(int c = 0; c < new_image.channels; c++){
            if(PXL_AT(src, curr.x, curr.y, c) != PXL_AT(src, x, y, c)){
                match = 0;
                break;
            }
        }
        if(!match) continue;

        for(int c = 0; c < new_image.channels; c++){
            PXL_AT(new_image, curr.x, curr.y, c) = (0xFF & (color >> (c * 8)));
        }
    
        for(int i = 0; i < 4; i++) {
            nx = curr.x + dx[i];
            ny = curr.y + dy[i];
            PUSH(positions, vec2(nx, ny));
        }
    }

    FREE(visited);
    return new_image;
}