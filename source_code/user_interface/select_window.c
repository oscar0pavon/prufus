#include "select_window.h"

#include "window.h"

#include <stdio.h>

#include "user_interface.h"

Window select_file_window;

bool can_draw_select_window = false;

GLXContext select_window_context;

void draw_select_window(){

  glXMakeCurrent(display, select_file_window, select_window_context);

  glClearColor(background_color.r, background_color.g, background_color.b, 1);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
  glFlush();

  glXSwapBuffers(display, select_file_window);
}

void create_select_file_window(){

    select_file_window = XCreateWindow(display, RootWindow(display, window_visual->screen),
            0, 0, 800, 600, 0, 
            window_visual->depth, InputOutput, window_visual->visual, 
            CWBorderPixel | CWColormap | CWEventMask, &window_attributes);
    
    Atom close_window = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, select_file_window, &close_window, 1);
    
    //set window name 
    XClassHint class_hint;
    class_hint.res_name = "Select File - prufus";
    class_hint.res_class = "Select File - prufus"; 
    XSetClassHint(display, select_file_window, &class_hint);

    XSetStandardProperties(display, select_file_window, 
            "Select File - prufus", "Select File - prufus", None, NULL, 0, NULL);


    XSelectInput(display, prufus_window, ButtonPressMask | ButtonReleaseMask | FocusChangeMask);
    
    //XSetInputFocus(display, select_file_window, RevertToParent, CurrentTime);

    select_window_context = glXCreateContext(display, window_visual, None, GL_TRUE); // GL_TRUE for direct rendering
    if (prufus_main_window_context == NULL) {
        printf("Can't create OpenGL context\n");
    }

    glXMakeCurrent(display, prufus_window, select_window_context);
    
    XMapWindow(display, select_file_window);

    can_draw_select_window = true;
    
}
