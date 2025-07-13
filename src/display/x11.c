#include <pthread.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <sys/select.h>

#include "imgc.h" 

#define IMGC_W_ICON_SIZE 64

typedef struct x11_window_task {
    Window win;
    XImage * image;
    int w;
    int h;
    int win_w;
    int win_h;
    Atom del_win;
    GC gc;
    int destroyed;
} wintask_t;

typedef enum {
    CMD_SHOW_IMAGE,
    CMD_EXIT
} ui_command_type;

typedef struct {
    ui_command_type type;
    wintask_t task;
} ui_command_t;

#define MAX_COMMANDS 128
#define MAX_X11_TASKS 1024

static wintask_t x11_tasks[MAX_X11_TASKS] = {0};

static ui_command_t command_queue[MAX_COMMANDS];
static int queue_start = 0, queue_end = 0;
static pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;
static int command_pipe_fd[2];

static volatile int active_window_count = 0;
static pthread_mutex_t window_count_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t all_windows_closed_cond = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t initializer_mtx = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t img_show_request = PTHREAD_MUTEX_INITIALIZER;

static int initialized = 0;
static int running = 0;
static volatile int show_count = 0;

static Display * main_dpy;

pthread_t ui_tid;

void imgc_x11_img_show(img_t source, char * window_name);
void imgc_x11_wait_until();
void imgc_x11_shutdown();

static void start_ui_thread();
static void * ui_thread(void *arg);
static void send_command(ui_command_t cmd);
static void imgc_x11_window_set_icon(img_t source, Display * dpy, Window win);
static XImage * imgc_x11_img_to_XImage(img_t source, Display * dpy);
static void destroy_wintask(Display * dpy, wintask_t * task);
static void default_handle_img(Display * dpy, XEvent evt, wintask_t * task);
void imgc_x11_reset_display();

static void start_ui_thread() {

    if(initialized){
        return;
    }
    
    XInitThreads();

    main_dpy = XOpenDisplay(NULL);
    if (!main_dpy) {
        fprintf(stderr, "[X11 UI] Failed to open X11 display\n");
        return;
    }
    
    if (pipe(command_pipe_fd) == -1) {
        perror("[X11 UI] Pipe failed");
        XCloseDisplay(main_dpy);
        return;
    }

    initialized = 1;
    running = 1;

    for(int i = 0; i < MAX_X11_TASKS; ++i)
        x11_tasks[i].destroyed = 1;

    pthread_create(&ui_tid, NULL, ui_thread, NULL);
}

static void * ui_thread(void *arg) {
    
    (void) arg;

    int x11_fd = ConnectionNumber(main_dpy);
    int command_fd = command_pipe_fd[0];
    fd_set fds;
    

    return_to_loop:
    int is_running = 1;

    while (is_running) {
        XFlush(main_dpy);

        FD_ZERO(&fds);
        FD_SET(x11_fd, &fds);
        FD_SET(command_fd, &fds);
        
        int max_fd = (x11_fd > command_fd) ? x11_fd : command_fd;

        if (select(max_fd + 1, &fds, NULL, NULL, NULL) == -1) {
            perror("[X11 UI] Select failed");
            break; 
        }

        if (FD_ISSET(command_fd, &fds)) {
            char dummy_buffer[32];
            read(command_fd, dummy_buffer, sizeof(dummy_buffer));

            pthread_mutex_lock(&queue_mutex);
            while (queue_start != queue_end) {
                ui_command_t cmd = command_queue[queue_start];
                queue_start = (queue_start + 1) % MAX_COMMANDS;
                
                if (cmd.type == CMD_EXIT) {
                    is_running = 0;
                    break;
                } else if (cmd.type == CMD_SHOW_IMAGE) {
                    int found_slot = 0;
                    for (int i = 0; i < MAX_X11_TASKS; i++) {
                        if (x11_tasks[i].destroyed) {
                            x11_tasks[i] = cmd.task;
                            x11_tasks[i].destroyed = 0;
                            found_slot = 1;

                            pthread_mutex_lock(&window_count_mutex);
                            active_window_count++;
                            pthread_mutex_unlock(&window_count_mutex);

                            break;
                        }
                    }
                    if(!found_slot){
                        destroy_wintask(main_dpy, &cmd.task);
                    }
                }
            }
            pthread_mutex_unlock(&queue_mutex);
        }

        if (!is_running) break;

        while (XPending(main_dpy)) {
            XEvent evt;
            XNextEvent(main_dpy, &evt);
            
            for (int i = 0; i < MAX_X11_TASKS; ++i) {
                if (!x11_tasks[i].destroyed && x11_tasks[i].win == evt.xany.window) {
                    default_handle_img(main_dpy, evt, &x11_tasks[i]);
                    break;
                }
            }
        }
    }

    pthread_mutex_lock(&img_show_request);
    if(show_count <= 0){
        for (int i = 0; i < MAX_X11_TASKS; i++) {
            destroy_wintask(main_dpy, &x11_tasks[i]);
        }
    }
    else {
        pthread_mutex_unlock(&img_show_request);
        goto return_to_loop;
    }
    pthread_mutex_unlock(&img_show_request);

    close(command_pipe_fd[0]);
    close(command_pipe_fd[1]);
    XCloseDisplay(main_dpy);

    imgc_x11_reset_display();

    return NULL;
}

static void send_command(ui_command_t cmd) {
    pthread_mutex_lock(&queue_mutex);
    command_queue[queue_end] = cmd;
    queue_end = (queue_end + 1) % MAX_COMMANDS;
    pthread_mutex_unlock(&queue_mutex);

    char dummy = 'c';
    if (write(command_pipe_fd[1], &dummy, 1) == -1) {
        perror("[X11 UI] Write to pipe failed");
    }
}

static void imgc_x11_window_set_icon(img_t source, Display * dpy, Window win){

    unsigned long * icon_data = ALLOC((2 + IMGC_W_ICON_SIZE * IMGC_W_ICON_SIZE) * sizeof(unsigned long));

    if (!icon_data) { return; }

    icon_data[0] = IMGC_W_ICON_SIZE;
    icon_data[1] = IMGC_W_ICON_SIZE;
    unsigned long * icon_pixels = &icon_data[2];

    img_t resized = img_resize(source, IMGC_W_ICON_SIZE, IMGC_W_ICON_SIZE);

    if(resized.channels == 1){
        int r = 0;
        for(int y = 0; y < resized.h; ++y){
            for(int x = 0; x < resized.w; ++x){
                r = CLAMP(PXL_AT(resized, x, y, 0), 0, 255);
                icon_pixels[y * IMGC_W_ICON_SIZE + x] = (0xFF << 24) | (r << 16) | (r << 8) | r;
            }
        }
    } else {
        int r = 0, g = 0, b = 0;
        for(int y = 0; y < resized.h; ++y){
            for(int x = 0; x < resized.w; ++x){
                r = CLAMP(PXL_AT(resized, x, y, 0), 0, 255);
                g = (resized.channels > 1) ? CLAMP(PXL_AT(resized, x, y, 1), 0, 255) : r;
                b = (resized.channels > 2) ? CLAMP(PXL_AT(resized, x, y, 2), 0, 255) : r;
                icon_pixels[y * IMGC_W_ICON_SIZE + x] = (0xFF << 24) | (r << 16) | (g << 8) | b;
            }
        }
    }

    Atom net_wm_icon = XInternAtom(dpy, "_NET_WM_ICON", False);
    Atom cardinal = XInternAtom(dpy, "CARDINAL", False);
    XChangeProperty(dpy, win, net_wm_icon, cardinal, 32, PropModeReplace, (unsigned char *) icon_data, 2 + IMGC_W_ICON_SIZE * IMGC_W_ICON_SIZE);
    
    img_free(&resized);
    FREE(icon_data);
}

static XImage * imgc_x11_img_to_XImage(img_t source, Display * dpy){
    int screen = DefaultScreen(dpy);
    Visual *vis = DefaultVisual(dpy, screen);
    int depth = DefaultDepth(dpy, screen);

    int width = source.bounds.x2 - source.bounds.x1;
    int height = source.bounds.y2 - source.bounds.y1;

    char * image_data = calloc(width * height * 4, sizeof(*image_data));
    XImage * displayed_img = XCreateImage(dpy, vis, depth, ZPixmap, 0, image_data, width, height, 32, 0);

    int x0 = source.bounds.x1;
    int y0 = source.bounds.y1;

    unsigned long pixel = 0;
    if(source.channels == 1){
        int r = 0;
        for(int y = 0; y < height; ++y){
            for(int x = 0; x < width; ++x){
                r = CLAMP(PXL_AT(source, x + x0, y + y0, 0), 0, 255);
                pixel = (r << 16) | (r << 8) | r;
                XPutPixel(displayed_img, x, y, pixel);
            }
        }
    } else {
        int r = 0, g = 0, b = 0;
        for(int y = 0; y < height; ++y){
            for(int x = 0; x < width; ++x){
                r = CLAMP(PXL_AT(source, x + x0, y + y0, 0), 0, 255);
                g = (source.channels > 1) ? CLAMP(PXL_AT(source, x + x0, y + y0, 1), 0, 255) : r;
                b = (source.channels > 2) ? CLAMP(PXL_AT(source, x + x0, y + y0, 2), 0, 255) : r;
                pixel = (r << 16) | (g << 8) | b;
                XPutPixel(displayed_img, x, y, pixel);
            }
        }
    }

    return displayed_img;
}

static void default_handle_img(Display * dpy, XEvent evt, wintask_t * task){
    if (evt.xany.window != task->win) return;
    switch(evt.type){
        case Expose:
            int dx = (task->win_w - task->w) / 2;
            int dy = (task->win_h - task->h) / 2;
            if (dx < 0) dx = 0;
            if (dy < 0) dy = 0;

            XPutImage(dpy, task->win, task->gc, task->image, 0, 0, dx, dy, task->w, task->h);
            break;
        case ClientMessage: 
            if ((Atom) evt.xclient.data.l[0] == task->del_win) {
                destroy_wintask(dpy, task);
                pthread_mutex_lock(&img_show_request);
                show_count--;
                pthread_mutex_unlock(&img_show_request);
            }
            break;
        case KeyPress:
            break;
        case ConfigureNotify:
            task->win_w = evt.xconfigure.width;
            task->win_h = evt.xconfigure.height;
    }
}

void imgc_x11_img_show(img_t source, char * window_name){

    pthread_mutex_lock(&img_show_request);
    show_count++;
    pthread_mutex_unlock(&img_show_request);
    
    pthread_mutex_lock(&initializer_mtx);
    if(!initialized) start_ui_thread();
    pthread_mutex_unlock(&initializer_mtx);
    
    int width = source.bounds.x2 - source.bounds.x1;
    int height = source.bounds.y2 - source.bounds.y1;

    XImage * displayed_img = imgc_x11_img_to_XImage(source, main_dpy);
    
    XLockDisplay(main_dpy);

    Window win = XCreateSimpleWindow(main_dpy, RootWindow(main_dpy, DefaultScreen(main_dpy)), 100, 100, width, height, 1, BlackPixel(main_dpy, DefaultScreen(main_dpy)), BlackPixel(main_dpy, DefaultScreen(main_dpy)));
    XSelectInput(main_dpy, win, ExposureMask | KeyPressMask | StructureNotifyMask | ClientMessage);
    
    if(window_name) XStoreName(main_dpy, win, window_name);
    else XStoreName(main_dpy, win, "Imagec Display");

    imgc_x11_window_set_icon(source, main_dpy, win);
    
    /**
     * Disable Resizing
     */
    // XSizeHints *hints = XAllocSizeHints();
    // hints->flags = PMinSize | PMaxSize;
    // hints->min_width = hints->max_width = width;
    // hints->min_height = hints->max_height = height;
    // XSetWMNormalHints(main_dpy, win, hints);
    // XFree(hints);

    GC gc = XCreateGC(main_dpy, win, 0, NULL);
    
    Atom del_win = XInternAtom(main_dpy, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(main_dpy, win, &del_win, 1);
    
    XMapWindow(main_dpy, win);
    XUnlockDisplay(main_dpy);

    ui_command_t command = {
        .type = CMD_SHOW_IMAGE,
        .task = {
            .win = win,
            .w = width,
            .h = height,
            .del_win = del_win,
            .image = displayed_img,
            .destroyed = 0,
            .gc = gc,
        },
    };

    send_command(command);
}

static void destroy_wintask(Display * dpy, wintask_t * task){
    if(!task->destroyed){
        if (task->image) {
            XDestroyImage(task->image);
        }
        if (task->gc) XFreeGC(dpy, task->gc);
        if (task->win) XDestroyWindow(dpy, task->win);
        task->destroyed = 1;

        pthread_mutex_lock(&window_count_mutex);
        active_window_count--;
        
        if (active_window_count == 0) {
            running = 0;
            pthread_cond_signal(&all_windows_closed_cond);
        }

        pthread_mutex_unlock(&window_count_mutex);
    }
}

void imgc_x11_wait_until() {

    pthread_mutex_lock(&initializer_mtx);
    if (!initialized) {
        pthread_mutex_unlock(&initializer_mtx);
        return;
    }
    pthread_mutex_unlock(&initializer_mtx);

    pthread_mutex_lock(&window_count_mutex);
    while (show_count > 0 || active_window_count > 0 || running) {
        pthread_cond_wait(&all_windows_closed_cond, &window_count_mutex);
    }

    pthread_mutex_unlock(&window_count_mutex);
}

void imgc_x11_reset_display() {
    pthread_mutex_lock(&initializer_mtx);

    pthread_mutex_destroy(&window_count_mutex);
    pthread_cond_destroy(&all_windows_closed_cond);
    pthread_mutex_destroy(&queue_mutex);

    pthread_mutex_init(&window_count_mutex, NULL);
    pthread_cond_init(&all_windows_closed_cond, NULL);
    pthread_mutex_init(&queue_mutex, NULL);

    initialized = 0;
    running = 0;

    pthread_mutex_unlock(&initializer_mtx);
}

void imgc_x11_shutdown() {
    if (!initialized) return;
    
    ui_command_t cmd = {.type = CMD_EXIT};
    send_command(cmd);

    pthread_join(ui_tid, NULL);
    
    pthread_mutex_destroy(&window_count_mutex);
    pthread_cond_destroy(&all_windows_closed_cond);
    pthread_mutex_destroy(&queue_mutex);

    pthread_mutex_destroy(&initializer_mtx);

    initialized = 0;
    running = 0;
}