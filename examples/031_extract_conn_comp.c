#include "imagec.h"

int main(){

    img_t fig0916A = img_load("samples/Fig0916A.sample.png");

    extract_cc_t extraction = img_extract_cc(fig0916A);

    printf("Component Count: %d\n", extraction.count);

    conn_component_t component;
    for(int i = 0; i < extraction.count; ++i){
        component = extraction.items[i];
        printf("+-- Component %d\n", component.label);
        printf("* Area: %d\n", component.area);
        printf("* Centroid: (%.2f, %.2f)\n\n", component.cx, component.cy);
    }

    img_show(fig0916A, "Original");

    component = extraction.items[3];

    for(int i = 0; i < component.pixels.count; ++i){
        for(int ch = 0; ch < fig0916A.channels; ++ch)
            PXL_AT(fig0916A, component.pixels.items[i].x, component.pixels.items[i].y, ch) = 0;
    }

    img_show(fig0916A, "Removed 3rd CComponent");

    wait_until();

    img_free(&fig0916A);
    img_free_extraction(&extraction);

    shutdown_display();

    return 0;
}