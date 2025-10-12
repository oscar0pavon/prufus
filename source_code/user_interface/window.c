#include "window.h"
#include <stdio.h>
#include <stdbool.h>


#include <GL/gl.h>
#include <GL/glx.h>

bool prufus_window_running = true;

bool check_buttons_collision = false;

Display* display;
XEvent window_event;

int mouse_click_x = 0;
int mouse_click_y = 0;

Window prufus_window; 

int prufus_create_window(){

    display = XOpenDisplay(NULL); // NULL for default display
    if (display == NULL) {
        // Handle error
        return 1;
    }

    int screen = DefaultScreen(display);
    Window root_window = DefaultRootWindow(display);

    static int attributes[] = {GLX_DEPTH_SIZE, 16, GLX_DOUBLEBUFFER, None};
    XVisualInfo *window_visual = glXChooseVisual(display, DefaultScreen(display), attributes);
    if (window_visual== NULL) {
        // Handle error
    }

    Colormap cmap = XCreateColormap(display, 
            RootWindow(display, window_visual->screen), window_visual->visual, AllocNone);


    XSetWindowAttributes window_attributes;
    window_attributes.colormap = cmap;
    window_attributes.border_pixel = 0;
    window_attributes.event_mask =
        ExposureMask | KeyPressMask | StructureNotifyMask;

    XClassHint class_hint;
    class_hint.res_name = "prufus";
    class_hint.res_class = "prufus"; 


    prufus_window = XCreateWindow(display, RootWindow(display, window_visual->screen),
            0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, 
            window_visual->depth, InputOutput, window_visual->visual, 
            CWBorderPixel | CWColormap | CWEventMask, &window_attributes);

    XSetClassHint(display, prufus_window, &class_hint);
    XSetStandardProperties(display, prufus_window, 
            "prufus", "prufus", None, NULL, 0, NULL);

    XSizeHints hints;
    hints.flags = PMinSize | PMaxSize;
    hints.min_width = WINDOW_WIDTH;
    hints.min_height = WINDOW_HEIGHT;
    hints.max_width = WINDOW_WIDTH;
    hints.max_height = WINDOW_HEIGHT;

    XSetWMNormalHints(display,prufus_window,&hints);

    // GL_TRUE for direct rendering
    GLXContext cx = glXCreateContext(display, window_visual, None, GL_TRUE); 
    if (cx == NULL) {
        // Handle error
    }
    
    Atom wm_delete_window = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, prufus_window, &wm_delete_window, 1);

    glXMakeCurrent(display, prufus_window, cx);

    XMapWindow(display, prufus_window);

    XSelectInput(display, prufus_window, ButtonPressMask | ButtonReleaseMask);

}

void* handle_input(void* none){


    while (prufus_window_running) {
    
        XNextEvent(display, &window_event);

        switch (window_event.type) {
            case ClientMessage:
                if (window_event.xclient.message_type == 
                        XInternAtom(display, "WM_PROTOCOLS", False) &&
                    (Atom)window_event.xclient.data.l[0] == 
                    XInternAtom(display, "WM_DELETE_WINDOW", False)) {
                    // or prompt the user for confirmation
                    prufus_window_running = false; 
                }
                break;
            case ButtonPress:
                printf("Mouse clicked: %d , %d\n",window_event.xbutton.x,
                        window_event.xbutton.y);

                mouse_click_x = window_event.xbutton.x;
                mouse_click_y = window_event.xbutton.y;

                break;

            case ButtonRelease:
                check_buttons_collision = true;


              break;
        }
    }    
}
