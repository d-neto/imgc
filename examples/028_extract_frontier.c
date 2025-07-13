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
    
    img_t frontier = extract_frontier(image);
    
    img_show(image, "Original");
    img_show(frontier, "Frontier");

    wait_until();

    return 0;
}