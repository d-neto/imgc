#include "imagec.h"

int main(){

    img_t example_img = img_load("samples/morph_test.sample.png");

    img_show(example_img, "Original");

    img_t closed_img = img_close(example_img, disc(4));
    img_show(closed_img, "Original - Morph. img_close");

    img_t bottom_hat = img_sub(closed_img, example_img);
    img_show(bottom_hat, "Original - Bottom Hat");

    img_t otsu_original = img_threshold(example_img, THRESH_OTSU);
    img_show(otsu_original, "Original - Otsu");

    img_t otsu_bh = img_threshold(bottom_hat, THRESH_OTSU);
    img_t otsu_bh_inv = img_negative(otsu_bh);
    img_show(otsu_bh_inv, "Bottom Hat - Otsu");
    
    wait_until();

    img_free(&example_img);
    img_free(&closed_img);
    img_free(&bottom_hat);
    img_free(&otsu_original);
    img_free(&otsu_bh);
    img_free(&otsu_bh_inv);

    shutdown_display();

    return 0;
}