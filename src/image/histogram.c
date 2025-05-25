#include "imagec.h"

/**
 * Get histogram data from grayscale image
 * Returns an array with 256 entries
 */
int * histdata(image_t image){
    assert(image.channels == 1);
    int * histogram = ALLOC(256 * sizeof(*histogram));
    for(int i = 0; i < 256; ++i) histogram[i] = 0;
    int val = 0;
    for (int y = 0; y < image.h; ++y) {
        for (int x = 0; x < image.w; ++x) {
            for (int c = 0; c < image.channels; ++c) {
                if (c == 3) continue;
                val = (int)PXL_AT(image, x, y, c);
                if (val < 0) val = 0;
                if (val > 255) val = 255;
                histogram[val]++;
            }
        }
    }
    return histogram;
}

/**
 * Get histogram data from grayscale image 
 * ignoring zero values
 * Returns an array with 256 entries
 */
int * histdata__ignore_zeros(image_t image){
    assert(image.channels == 1);
    int * histogram = ALLOC(256 * sizeof(*histogram));
    for(int i = 0; i < 256; ++i) histogram[i] = 0.0;
    int val = 0;
    for (int y = 0; y < image.h; ++y) {
        for (int x = 0; x < image.w; ++x) {
            for (int c = 0; c < image.channels; ++c) {
                if (c == 3) continue;
                val = (int) (PXL_AT(image, x, y, c));
                if (val == 0) continue;
                if (val < 0) val = 0;
                if (val > 255) val = 255;
                histogram[val]++;
            }
        }
    }
    return histogram;
}

static void show_using_GnuPlot(FILE *gnuplot, int * histogram);
static void show_using_terminal(int * histogram);

/**
 * Show the histogram using GnuPlot
 */
void histshow(int * histogram) {
    FILE *gnuplot = popen("gnuplot -persistent", "w");
    if (gnuplot != NULL) {
        show_using_GnuPlot(gnuplot, histogram);
        return;
    }
    show_using_terminal(histogram);
}

static void show_using_terminal(int * histogram){
    printf("+-- Histogram Data ----------------------------------------------+\n");

    int min = __INT_MAX__, max = -__INT_MAX__;
    for(int i = 0; i < 256; ++i){
        if(histogram[i] > max) max = histogram[i];
        if(histogram[i] < min) min = histogram[i];
    }

    int max_chars = 64;

    if (max == min) {
        for(int i = 0; i < 256; ++i){
            printf("+ %*d |\n", 3, i);
        }
        return;
    }

    for(int i = 0; i < 256; ++i){
        int normalized = ((histogram[i] - min) * max_chars) / (max - min);
        printf("+ %*d |", 3, i);
        for(int j = 0; j < normalized; ++j)
            printf("■");
        printf(" %d\n", histogram[i]);
    }
}

static void show_using_GnuPlot(FILE *gnuplot, int * histogram){
    if (gnuplot == NULL) {
        perror("Failed to open gnuplot");
        return;
    }
    fprintf(gnuplot, "set terminal qt size 800,500 enhanced font 'Verdana,10'\n");
    fprintf(gnuplot, "set title 'Image Histogram' font ',14' textcolor '#333333'\n");
    fprintf(gnuplot, "set xlabel 'Pixel Intensity (0-255)' font ',12' textcolor '#333333'\n");
    fprintf(gnuplot, "set ylabel 'Frequency' font ',12' textcolor '#333333'\n");
    fprintf(gnuplot, "set xrange [0:255]\n");
    
    fprintf(gnuplot, "set style fill solid 0.8 border rgb '#333333'\n");
    fprintf(gnuplot, "set boxwidth 0.9 relative\n");
    fprintf(gnuplot, "set grid xtics ytics lc rgb '#dddddd' lw 1\n");
    fprintf(gnuplot, "set tics font ',10'\n");
    fprintf(gnuplot, "set border 3 back lc rgb '#666666' lw 2\n");
    fprintf(gnuplot, "set key off\n");
    fprintf(gnuplot, "set object 1 rectangle from screen 0,0 to screen 1,1 behind fillcolor rgb '#f8f8f8' fillstyle solid\n");
    
    fprintf(gnuplot, "set palette defined (0 '#5e81ac', 1 '#81a1c1', 2 '#88c0d0', 3 '#8fbcbb')\n");
    
    fprintf(gnuplot, "plot '-' using 1:($2):1 with boxes lc palette\n");

    for (int i = 0; i < 256; i++) {
        fprintf(gnuplot, "%d %d\n", i, histogram[i]);
    }
    fprintf(gnuplot, "e\n");
    fflush(gnuplot);
    pclose(gnuplot);
}

void histdump__from_image(image_t image) {
    double_arr frequencies = DOUBLE_ARR(256);
    for(int i = 0; i < frequencies.capacity; ++i) 
        frequencies.items[i] = 0.0;

    double value = 0.0;
    int idx = 0;
    for(int i = 0; i < image.h; ++i) {
        for(int j = 0; j < image.w; ++j) {
            value = PXL_AT(image, j, i, 0);
            idx = (int)value;
            idx = (idx > 255) ? 255 : ((idx < 0) ? 0 : idx);
            frequencies.items[idx] += 1.0;
        }
    }

    double max_freq = 0.0;
    for(int i = 0; i < 256; ++i) {
        if(frequencies.items[i] > max_freq)
            max_freq = frequencies.items[i];
    }

    FILE *gnuplot = popen("gnuplot -persistent", "w");
    if (gnuplot == NULL) {
        perror("Failed to open gnuplot");
        exit(EXIT_FAILURE);
    }

    fprintf(gnuplot, "set terminal qt size 800,500 enhanced font 'Verdana,10'\n");
    fprintf(gnuplot, "set title 'Image Histogram' font ',14' textcolor '#333333'\n");
    fprintf(gnuplot, "set xlabel 'Pixel Intensity (0-255)' font ',12' textcolor '#333333'\n");
    fprintf(gnuplot, "set ylabel 'Frequency' font ',12' textcolor '#333333'\n");
    fprintf(gnuplot, "set xrange [0:255]\n");
    // fprintf(gnuplot, "set yrange [0:90]\n");
    // fprintf(gnuplot, "set yrange [0:%.0f]\n", max_freq+10);
    
    fprintf(gnuplot, "set style fill solid 0.8 border rgb '#333333'\n");
    fprintf(gnuplot, "set boxwidth 0.9 relative\n");
    fprintf(gnuplot, "set grid xtics ytics lc rgb '#dddddd' lw 1\n");
    fprintf(gnuplot, "set tics font ',10'\n");
    fprintf(gnuplot, "set border 3 back lc rgb '#666666' lw 2\n");
    fprintf(gnuplot, "set key off\n");
    fprintf(gnuplot, "set object 1 rectangle from screen 0,0 to screen 1,1 behind fillcolor rgb '#f8f8f8' fillstyle solid\n");
    
    fprintf(gnuplot, "set palette defined (0 '#5e81ac', 1 '#81a1c1', 2 '#88c0d0', 3 '#8fbcbb')\n");
    
    fprintf(gnuplot, "plot '-' using 1:($2):1 with boxes lc palette\n");

    for (int i = 0; i < 256; i++) {
        fprintf(gnuplot, "%d %.2f\n", i, frequencies.items[i]);
    }
    fprintf(gnuplot, "e\n");
    fflush(gnuplot);
    pclose(gnuplot);
}