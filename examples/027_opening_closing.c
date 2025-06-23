#include "imagec.h"

void create_example_shape(image_t image) {
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

    image_t image = create_image(256, 256, 1);
    create_example_shape(image);

    matrix_t kernel1 = mat(3, 5, (double []) {
        0, 0, 0, 0, 0,
        1, 1, 1, 1, 1,
        0, 0, 0, 0, 0,
    });

    matrix_t kernel2 = mat(1, 3, (double []) {
        1, 1, 1
    });
    
    image_t open = opening(image, kernel1);
    image_t close = closing(image, kernel2);
    
    img_show(image, "Original");
    img_show(open, "Opening");
    img_show(close, "Closing");

    wait_until();

    return 0;
}