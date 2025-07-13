#include "imgc.h"

int main(){

    img_t img = img_load("samples/lenna.sample.png");

    img_t add = img_sum(img, img);

    img_show(add, "Lenna");

    wait_until();

    img_free(&add);
    img_free(&img);
    shutdown_display();

    return 0;
}