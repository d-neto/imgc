#include "imgc.h"
#include "parallel.h"

int main(){

    img_t example_img = img_load("samples/Fig0940.sample.png");

    img_show(example_img, "Original");

    img_t otsu_original = img_threshold(example_img, THRESH_OTSU);
    img_show(otsu_original, "Original - Otsu");

    mat_t disc40 = disc(40);

    img_t opened_img = img_open(example_img, disc40);

    img_show(opened_img, "Original - Morph. img_open");

    img_t top_hat = img_sub(example_img, opened_img);
    img_show(top_hat, "Original - Top Hat");

    img_t otsu_th = img_threshold(top_hat, THRESH_OTSU);
    img_show(otsu_th, "Top Hat - Otsu");

    wait_until();
    shutdown_display();

    return 0;
}