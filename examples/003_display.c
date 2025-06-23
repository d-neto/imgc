#include "imagec.h"

int main(){

    image_t image = img_load("samples/cat.sample.png");
    
    /**
     * Display Image Implemented Methods
     * - Linux: X11
     * - Windows: Not Implemented
     * - MacOs: Not Implemented
     */
    img_show(image, "Cat.png");

    /**
     * Block listening events of the current thread
     * until the above images are closed
     */
    wait_until();

    img_show(image, "Cat.png");
    wait_until();

    img_show(image, "Cat.png");
    img_show(image, "Cat.png");
    wait_until();

    free_image(&image);

    /**
     * If you want to free all resources allocated
     * by the display implementation
     */
    shutdown_display();

        
    /**
     * LINUX X11: Be careful using wait_until(), it will
     * block while has images on the current queue for display.
     * 
     * If another thread in your program is always showing images
     * any thread with wait_until() will be blocked
     */

    return 0;
}