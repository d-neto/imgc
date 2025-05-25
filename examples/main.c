#include <stdio.h>
#include "imagec.h"

int main(){
    MM_BEGIN();

    image_t lenna = load_image("./samples/lenna.sample.png");
    image_t fig0305 = load_image("./samples/Fig0305.sample.png");
    image_t fig0309 = load_image("./samples/Fig0309.sample.png");
    image_t fig0310 = load_image("./samples/Fig0310.sample.png");
    image_t lenna_salt_pepper = load_image("./samples/lenna-salt-pepper.sample.png");


    // RGB to Grayscale
    image_t grasycale = rgb2gray((lenna));
    write_image("./samples/out/lenna-grayscale.png", grasycale);

    // Average Blur Filter
    image_t average_filter = mean(lenna, 5);
    write_image("./samples/out/lenna-mean-3x3.png", average_filter);

    // Gaussian Blur Filter
    image_t gaussian_filter = gaussian(lenna, 13, 2);
    write_image("./samples/out/lenna-gaussian-7x7.png", gaussian_filter);
    
    // Clone, Crop & Paste
    image_t crop_half = crop(lenna, rect(0, 0, lenna.w/2, lenna.h));
    image_t half_gray = rgb2gray(crop_half);
    write_image("./samples/out/lenna-grayscale-rgb.png", paste(half_gray, clone(lenna), 0, 0));

    // Sobel Filter
    image_t wdg2 = load_image("./samples/wdg2.sample.gif");
    image_t test_sobel = sobel(wdg2);

    image_t goboard = load_image("./samples/goboard.sample.jpg");
    image_t test_sobel2 = sobel(goboard);

    image_t crop_rect = crop(lenna, rect(240, 0, 360, lenna.h));
    image_t lenna_sobel = paste(sobel(crop_rect), clone(lenna), 240, 0);

    write_image("./samples/out/lenna-sobel.png", lenna_sobel);
    write_image("./samples/out/wdg2-sobel.png", test_sobel);
    write_image("./samples/out/goboard-sobel.png", test_sobel2);
    
    // Scaling
    image_t lenna_64 = resize(lenna, 64, 64);
    write_image("./samples/out/lenna-64x64.png", lenna_64);
    
    // Cut and Paste
    geometry_t lenna_face_geom = polygon(7, vec2(250, 260), vec2(340, 200), vec2(350, 300), vec2(310, 385), vec2(240, 380), vec2(220, 370), vec2(220, 300));
    image_t sobel_polygon = sobel(cut(clone(lenna), lenna_face_geom));
    write_image("./samples/out/lenna_cut_polygon.png", paste(sobel_polygon, clone(lenna), 220, 200));
    write_image("./samples/out/lenna_sobel.png", sobel(rgb2gray(lenna)));

    // Median Filter
    write_image("./samples/out/lenna-median.png", median(lenna_salt_pepper, 5));
    
    // Intensity Transforms
    write_image("./samples/out/lenna-brightness-1.5.png", brightness(clone(lenna), 1.5));
    write_image("./samples/out/lenna-brightness-red-channel.png", brightness_ch(clone(lenna), 3, 1.5, 1.0, 1.0));
    write_image("./samples/out/lenna-negative.png", negative(clone(lenna)));
    write_image("./samples/out/fig0305-log.png", tlog(fig0305, 1));
    write_image("./samples/out/fig0309-pow.png", tpower(fig0309, 5.0, 1.0));
    write_image("./samples/out/fig0310-stretch.png", contrast_stretch(fig0310));

    // Blend Operations
    write_image("./samples/out/example-sum.png", sum_offset(lenna, sobel_polygon, 220, 200));
    write_image("./samples/out/example-sub.png", sub_offset(lenna, sobel_polygon, 220, 200));
    write_image("./samples/out/example-multiply.png", multiply(lenna, lenna));
    write_image("./samples/out/example-blend.png", blend_offset(lenna, sobel_polygon, 0.5, 220, 200));
    write_image("./samples/out/example-screen.png", screen(lenna, lenna));
    write_image("./samples/out/example-and.png", and_offset(lenna, sobel_polygon, 220, 200));
    write_image("./samples/out/example-xor.png", xor_offset(lenna, sobel_polygon, 220, 200));
    write_image("./samples/out/example-mask.png", mask(lenna, sobel_polygon));

    // Flood Fill
    write_image("./samples/out/example-fill.png", flood_fill(mask(lenna, sobel_polygon), 315, 302, 0xFF0000));

    MM_CLEAN();
    return 0;
}