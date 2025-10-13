#include "window.h"
#include <X11/X.h>
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
Window select_file_window;

XSetWindowAttributes window_attributes;

Colormap color_map;

GLXContext prufus_gl;

Atom atom_close_window; 

static int gl_attributes[] = {GLX_DEPTH_SIZE, 16, GLX_DOUBLEBUFFER, None};

XVisualInfo *window_visual;



void create_select_file_window(){

    select_file_window = XCreateWindow(display, RootWindow(display, window_visual->screen),
            0, 0, 800, 600, 0, 
            window_visual->depth, InputOutput, window_visual->visual, 
            CWBorderPixel | CWColormap | CWEventMask, &window_attributes);
    
    Atom close_window = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, select_file_window, &close_window, 1);
    
    //set window name 
    XClassHint class_hint;
    class_hint.res_name = "prufus";
    class_hint.res_class = "prufus"; 
    XSetClassHint(display, select_file_window, &class_hint);

    XSetStandardProperties(display, select_file_window, 
            "prufus", "prufus", None, NULL, 0, NULL);

    XMapWindow(display, select_file_window);
    
}

void close_prufus_window(){
    prufus_window_running = false;

    XEvent event;
    Atom wm_delete_window;

    wm_delete_window = XInternAtom(display, "WM_DELETE_WINDOW", False);

    event.xclient.type = ClientMessage;
    event.xclient.window = prufus_window;
    event.xclient.message_type = XInternAtom(display, "WM_PROTOCOLS", False);
    event.xclient.format = 32;
    event.xclient.data.l[0] = wm_delete_window;
    event.xclient.data.l[1] = CurrentTime;

    XSendEvent(display, prufus_window, False, NoEventMask, &event);
    XFlush(display); // Ensure the event is sent immediately
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


    XSelectInput(display, prufus_window, ButtonPressMask | ButtonReleaseMask);


    //OpenGL initialization
    prufus_gl = glXCreateContext(display, window_visual, None, GL_TRUE); // GL_TRUE for direct rendering
    if (prufus_gl == NULL) {
        // Handle error
        printf("Can't create OpenGL context\n");
        return -1;
    }
    glXMakeCurrent(display, prufus_window, prufus_gl);
    
    
    //show the window
    XMapWindow(display, prufus_window);
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
                // printf("Mouse clicked: %d , %d\n",window_event.xbutton.x,
                //         window_event.xbutton.y);

                mouse_click_x = window_event.xbutton.x;
                mouse_click_y = window_event.xbutton.y;

                break;

            case ButtonRelease:
                check_buttons_collision = true;


              break;
        }
    }    
}
