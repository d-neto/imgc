#include "imagec.h"
#include "parallel.h"

int main(){

    img_t lenna = img_load("samples/lenna.sample.png");

    img_show(lenna, "Lenna");

    img_t blur = PARALLEL(lenna, 6, {
        *SLICE = img_median(*SLICE, 13);
    });

    img_show(blur, "Lenna - Blur");

    wait_until();

    return 0;
}