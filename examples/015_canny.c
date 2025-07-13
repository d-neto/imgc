#include "imagec.h"

int main(){

    img_t lenna = img_load("samples/lenna.sample.png");

    img_t result = img_canny(lenna, 0.1, 0.5);

    img_show(lenna, "Original");
    img_show(result, "img_canny Edge Detection");

    wait_until();

    img_free(&lenna);
    img_free(&result);

    shutdown_display();

    return 0;
}