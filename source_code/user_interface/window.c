#include "window.h"
#include "select_window.h"

#include <stdio.h>

bool prufus_window_running = true;

bool check_buttons_collision = false;

Display* display;
XEvent window_event;

int mouse_click_x = 0;
int mouse_click_y = 0;

Window prufus_window; 

XSetWindowAttributes window_attributes;
XVisualInfo* window_visual;

Colormap color_map;

GLXContext prufus_main_window_context;

Atom atom_close_window; 

int gl_attributes[4] = {GLX_DEPTH_SIZE, 16, GLX_DOUBLEBUFFER, None};


int handle_x_error(Display *display, XErrorEvent *error) {
    char buffer[256];
    XGetErrorText(display, error->error_code, buffer, sizeof(buffer));
    fprintf(stderr, "X Error: %s\n", buffer);
    // Breakpoint here
    return 0;
}


void hande_close_window(Window window){

    XEvent event;
    Atom wm_delete_window;

    wm_delete_window = XInternAtom(display, "WM_DELETE_WINDOW", False);

    event.xclient.type = ClientMessage;
    event.xclient.window = window;
    event.xclient.message_type = XInternAtom(display, "WM_PROTOCOLS", False);
    event.xclient.format = 32;
    event.xclient.data.l[0] = wm_delete_window;
    event.xclient.data.l[1] = CurrentTime;

    XSendEvent(display, window, False, NoEventMask, &event);
    XFlush(display); // Ensure the event is sent immediately
}

void close_prufus_window(){
    prufus_window_running = false;

    //hande_close_window(prufus_window);
}

int prufus_create_window(){

    display = XOpenDisplay(NULL); // NULL for default display
    if (display == NULL) {
        // Handle error
        return 1;
    }
    

    window_visual = glXChooseVisual(display, DefaultScreen(display), gl_attributes);
    if (window_visual== NULL) {
        // Handle error
    }

    color_map = XCreateColormap(display, 
            RootWindow(display, window_visual->screen), window_visual->visual, AllocNone);


    window_attributes.colormap = color_map;
    window_attributes.border_pixel = 0;
    window_attributes.event_mask =
        ExposureMask | KeyPressMask | StructureNotifyMask;



    prufus_window = XCreateWindow(display, RootWindow(display, window_visual->screen),
            0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, 
            window_visual->depth, InputOutput, window_visual->visual, 
            CWBorderPixel | CWColormap | CWEventMask, &window_attributes);

    //set window name 
    XClassHint class_hint;
    class_hint.res_name = "prufus";
    class_hint.res_class = "prufus"; 
    XSetClassHint(display, prufus_window, &class_hint);

    XSetStandardProperties(display, prufus_window, 
            "prufus", "prufus", None, NULL, 0, NULL);

    //prevent resizing
    XSizeHints hints;
    hints.flags = PMinSize | PMaxSize;
    hints.min_width = WINDOW_WIDTH;
    hints.min_height = WINDOW_HEIGHT;
    hints.max_width = WINDOW_WIDTH;
    hints.max_height = WINDOW_HEIGHT;

    XSetWMNormalHints(display,prufus_window,&hints);

   
    //handle close the window
    atom_close_window = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, prufus_window, &atom_close_window, 1);


    XSelectInput(display, prufus_window, ButtonPressMask | ButtonReleaseMask | FocusChangeMask);


    //OpenGL initialization
    prufus_main_window_context = glXCreateContext(display, window_visual, None, GL_TRUE); // GL_TRUE for direct rendering
    if (prufus_main_window_context == NULL) {
        printf("Can't create OpenGL context\n");
        return -1;
    }
    
    glXMakeCurrent(display, prufus_window, prufus_main_window_context);
    
    //show the window
    XMapWindow(display, prufus_window);

}

