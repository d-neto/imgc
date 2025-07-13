#include "imagec.h"

void draw_circle_with_hole(img_t image, int cx, int cy, int r, int hole_size) {
    for (int y = -r; y <= r; ++y) {
        for (int x = -r; x <= r; ++x) {
            if (x*x + y*y <= r*r) {
                PXL_AT(image, cy + y, cx + x, 0) = 255;
            }
        }
    }
    for (int y = -hole_size; y <= hole_size; ++y) {
        for (int x = -hole_size; x <= hole_size; ++x) {
            PXL_AT(image, cy + y, cx + x, 0) = 0;
        }
    }
}

void create_example_shape(img_t image) {
    draw_circle_with_hole(image, 50, 50, 15, 2);
    draw_circle_with_hole(image, 100, 50, 12, 2);
    draw_circle_with_hole(image, 150, 50, 10, 1);
    draw_circle_with_hole(image, 50, 100, 18, 3);
    draw_circle_with_hole(image, 100, 100, 20, 4);
    draw_circle_with_hole(image, 150, 100, 15, 2);
}

int main(){

    img_t image = img_create(256, 256, 1);
    create_example_shape(image);
    
    img_t filled = img_fillholes(image);
    
    img_show(image, "Original");
    img_show(filled, "Filled Holes");

    img_t fig916a = img_load("samples/Fig0916A.sample.png");
    img_show(fig916a, "FIG916A");
    img_show(img_fillholes(fig916a), "FIG916A Filled");

    wait_until();

    return 0;
}