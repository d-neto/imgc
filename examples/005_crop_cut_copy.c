#include "imagec.h"

int main(){

    image_t lenna = img_load("samples/lenna.sample.png");
    image_t cropped = crop(lenna, rect(110, 100, 380, 380));

    draw_txt(cropped, 10, 20, 8, 0xF3F300, "Lenna Face");

    img_show(lenna, "Lenna");
    img_show(cropped, "Lenna Face");

    wait_until();

    return 0;
}