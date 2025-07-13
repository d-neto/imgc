#include "imagec.h"

void create_example_shape(img_t image) {
    for(int i = 0; i < 10; ++i)
        for(int j = 0; j < 15; ++j)
            PXL_AT(image, 118 + i, 113 + j, 0) = 255;

    for(int i = 0; i < 4; ++i)
        for(int j = 0; j < 10; ++j)
            PXL_AT(image, 118 + i, 128 + j, 0) = 255;
}

int main(){

    img_t image = img_create(256, 256, 1);
    create_example_shape(image);

    mat_t kernel = MAT(3, 5,
        0, 0, 0, 0, 0,
        1, 1, 1, 1, 1,
        0, 0, 0, 0, 0,
    );
    
    img_t erosion = img_dilate(image, kernel);
    
    img_show(image, "Original");
    img_show(erosion, "img_dilate");

    wait_until();

    return 0;
}