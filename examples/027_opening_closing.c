#include "imgc.h"

void create_example_shape(img_t image) {
    for(int i = 0; i < 15; ++i)
        for(int j = 0; j < 15; ++j)
            PXL_AT(image, 118 + i, 113 + j, 0) = 255;

    for(int i = 0; i < 4; ++i)
        for(int j = 0; j < 10; ++j)
            PXL_AT(image, 118 + i, 128 + j, 0) = 255;

    for(int i = 0; i < 2; ++i)
        for(int j = 0; j < 2; ++j)
            PXL_AT(image, 124 + i, 118 + j, 0) = 0;
}

int main(){

    img_t image = img_create(256, 256, 1);
    create_example_shape(image);

    mat_t kernel1 = MAT(3, 5,
        0, 0, 0, 0, 0,
        1, 1, 1, 1, 1,
        0, 0, 0, 0, 0,
    );

    mat_t kernel2 = MAT(1, 3,
        1, 1, 1
    );
    
    img_t open = img_open(image, kernel1);
    img_t close = img_close(image, kernel2);
    
    img_show(image, "Original");
    img_show(open, "img_open");
    img_show(close, "img_close");

    wait_until();

    return 0;
}