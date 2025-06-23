#include "imagec.h"

int main(){

    image_t lenna = img_load("samples/lenna.sample.png");

    image_t result = canny(lenna, 0.1, 0.5);

    img_show(lenna, "Original");
    img_show(result, "Canny Edge Detection");

    wait_until();

    free_image(&lenna);
    free_image(&result);

    shutdown_display();

    return 0;
}