#include "imagec.h"

void create_example_shape(image_t image) {
    for(int i = 0; i < 10; ++i)
        for(int j = 0; j < 15; ++j)
            PXL_AT(image, 118 + i, 113 + j, 0) = 255;

    for(int i = 0; i < 4; ++i)
        for(int j = 0; j < 10; ++j)
            PXL_AT(image, 118 + i, 128 + j, 0) = 255;
}

int main(){

    image_t image = create_image(256, 256, 1);
    create_example_shape(image);

    matrix_t kernel = mat(3, 5, (double []) {
        0, 0, 0, 0, 0,
        1, 1, 1, 1, 1,
        0, 0, 0, 0, 0,
    });
    
    image_t erosion = dilate(image, kernel);
    
    img_show(image, "Original");
    img_show(erosion, "Dilate");

    wait_until();

    return 0;
}