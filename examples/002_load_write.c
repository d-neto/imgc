#include "imagec.h"

int main(){

    image_t image = img_load("samples/lenna.sample.png");

    FOREACH_PXL(image, {
        PXL_AT(image, x, y, c) += 50;
    });

    img_write(image, "samples/out/002_lenna+50.png");

    return 0;
}