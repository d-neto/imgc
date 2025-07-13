#include "imagec.h"

#define MAX_WIN_NAME 512

extern void imgc_x11_img_show(img_t source, char * window_name);
extern void imgc_x11_wait_until();
extern void imgc_x11_shutdown();

void img_show(img_t source, char * _win_name, ...){
    char window_name[MAX_WIN_NAME] = {0};
    va_list args;
    va_start(args, _win_name);
    vsnprintf(window_name, sizeof(window_name), _win_name, args);
    va_end(args);  

    #ifdef __linux__
        imgc_x11_img_show(source, window_name);
        return;
    #endif
}

void wait_until(){
    #ifdef __linux__
        imgc_x11_wait_until();
        return;
    #endif
}

void shutdown_display(){
    #ifdef __linux__
        imgc_x11_shutdown();
        return;
    #endif
}
