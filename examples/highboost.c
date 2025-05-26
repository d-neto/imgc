#include "imagec.h"

int main(){
    image_t lenna = load_image("./samples/lenna.sample.png");
    image_t result1 = highboost(lenna, 1.5);
    image_t result2 = highboost__spec(lenna, 2.0, SMH_MEAN, 3, 0);

    write_image("./samples/out/lenna-highboost1.png", result1);
    write_image("./samples/out/lenna-highboost2.png", result2);
    return 0;
}