#include "imagec.h"

int main(){

    img_t fig0916A = img_load("samples/Fig0916A.sample.png");

    mat_t kernel = MAT(3, 3,
         -1,  1, -1,
          1,  1,  1,
         -1,  1, -1,
    );

    img_t result = img_hit_or_miss(fig0916A, kernel);

    result = img_dilate(result, kernel);

    img_show(fig0916A, "Original");
    img_show(result, "Hit Or Miss Transform");

    wait_until();

    return 0;
}