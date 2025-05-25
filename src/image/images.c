
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
        .channels = ch
    };
    image.data = ALLOC(sizeof(*image.data)*image.w*image.h*image.channels);
    assert(image.data);
    return image;
}

image_t load_image(char * filename){
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
    return image;
}

void write_image(char * filename, image_t image){
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

image_t clone(image_t source){
    image_t new_image = {
        .channels = source.channels,
        .h = source.h,
        .w = source.w,
        .data = ALLOC(sizeof(*source.data)*source.w*source.h*source.channels)
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
        .channels = 1
    };

    new_image.data = ALLOC(sizeof(*new_image.data)*new_image.h*new_image.w);
    assert(new_image.data);

    size_t max = new_image.h*new_image.w;
    for(size_t i = 0; i < max; ++i){
        if(image.data[(i*image.channels)] == VOID_PIXEL){
            new_image.data[i] = VOID_PIXEL;
            continue;
        }
        new_image.data[i] = 0.3 * image.data[(i*image.channels)] + 0.59 * image.data[(i*image.channels)+1] + 0.11 * image.data[(i*image.channels)+2];
        // new_image.data[i] = (image.data[(i*image.channels)] + image.data[(i*image.channels)+1] + image.data[(i*image.channels)+2])/3;
    }

    return new_image;
}

void upgrade(image_t * image, int channels){
    if(channels <= image->channels) return;
    image_t clonned = clone(*image);

    image->w = clonned.w;
    image->h = clonned.h;
    image->channels = channels;
    image->data = ALLOC(sizeof(*image->data)*clonned.h*clonned.w*channels);
    assert(image->data);

    FOREACH_PXL(*image, {
        if(c < clonned.channels)
            PXL_AT(*image, x, y, c) = PXL_AT(clonned, x, y, c);
        else
            PXL_AT(*image, x, y, c) = 0;
    });

    free_image(&clonned);
}

image_t paste(image_t src, image_t dest, int x, int y){

    if(dest.channels < src.channels) upgrade(&dest, src.channels);

    size_t pos = 0;
    int tx = 0, ty = 0;

    int channels = dest.channels;
    
    for(size_t i = 0; i < src.h; ++i){
        ty = (y + i);
        for(size_t j = 0; j < src.w; ++j){
            tx = (x + j);
            for(size_t c = 0; c < channels; ++c){
                if(src.channels <= c) {
                    if(src.data[(i*src.w + j)*src.channels + 0] == VOID_PIXEL) continue; 
                    PXL_AT(dest, tx, ty, c) = src.data[(i*src.w + j)*src.channels + 0];
                    continue;
                }
                if(src.data[(i*src.w + j)*src.channels + c] == VOID_PIXEL) {
                    continue;
                }
                pos = (ty * dest.w + tx) * dest.channels + c;
                if(pos > (dest.w * dest.h * dest.channels))
                    continue;
                else
                    PXL_AT(dest, tx, ty, c) = src.data[(i*src.w + j)*src.channels + c];
            }
        }
    }
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