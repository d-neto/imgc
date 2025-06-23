#include "imagec.h"

int main(){

    image_t image = create_image(512, 512, 1);

    for(int i = 0; i < 10; ++i)
        draw_txt(image, 32, 32 + 32*i, 12, 0xFFFFFF, "%d: Goodbye, World!", i);

    img_show(image, "Goodbye, World!");

    image_t lenna = img_load("samples/lenna.sample.png");
    draw_txt(lenna, 16, 16, 12, 0x0000FF, "Lenna");

    img_show(lenna, "Lenna");

    wait_until();

    return 0;
}