#include "imgc.h"

int main(){

    img_t image = img_create(256, 256, 1);

    int r2 = 64*64;

    for (int i = 64; i < 192; ++i) {
        for (int j = 64; j < 192; ++j) {
            int dx = i - 128;
            int dy = j - 128;
            if (dx*dx + dy*dy <= r2) {
                PXL_AT(image, i, j, 0) = 255;
            }
        }
    }

    img_show(image, "Circle");
    wait_until();

    img_free(&image);

    shutdown_display();

    return 0;
}