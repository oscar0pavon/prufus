#include "select_window.h"

#include "window.h"

#include <stdio.h>

#include "user_interface.h"
#include "button.h"
#include <string.h>
#include <unistd.h>

#include "opengl.h"

#define SELECT_WINDOW_WIDTH 855
#define SELECT_WINDOW_HEIGHT 780

Window select_file_window;

bool can_draw_select_window = false;

GLXContext select_window_context;

Button open_select_window;
Button cancel_select_window;

void close_select_window(){

  hande_close_window(select_file_window);
}


void init_select_window(){
    
    strcpy(cancel_select_window.text,"Cancel");
    cancel_select_window.execute = &close_select_window;

    button_new(&cancel_select_window, vec2(SELECT_WINDOW_WIDTH-100,SELECT_WINDOW_HEIGHT-90), vec2(80,30) );
}


void draw_select_window(){

  glXMakeCurrent(display, select_file_window, select_window_context);
  
  
  glViewport(0, 0, SELECT_WINDOW_WIDTH, SELECT_WINDOW_HEIGHT);

  set_ortho_projection(SELECT_WINDOW_WIDTH,SELECT_WINDOW_HEIGHT);

  glClearColor(background_color.r, background_color.g, background_color.b, 1);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


  Button buttons[] = {
      cancel_select_window,
  };

  int buttons_count = sizeof(buttons) / sizeof(Button);

  for (int i = 0; i < buttons_count; i++) {
    if (check_button_clicked(&buttons[i])) {
      if (buttons[i].execute != NULL) {
        buttons[i].execute();
      }
    }
    draw_button(&buttons[i]);
  }

  glXSwapBuffers(display, select_file_window);
}



void create_select_file_window(){

    select_file_window = XCreateWindow(display, RootWindow(display, window_visual->screen),
            0, 0, SELECT_WINDOW_WIDTH, SELECT_WINDOW_HEIGHT, 0, 
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


    XSelectInput(display, select_file_window, ButtonPressMask | ButtonReleaseMask | FocusChangeMask);
    
    //XSetInputFocus(display, select_file_window, RevertToParent, CurrentTime);

    select_window_context = glXCreateContext(display, window_visual, prufus_main_window_context, GL_TRUE); // GL_TRUE for direct rendering
    if (prufus_main_window_context == NULL) {
        printf("Can't create OpenGL context\n");
    }

    glXMakeCurrent(display, prufus_window, select_window_context);
    
    XMapWindow(display, select_file_window);

    can_draw_select_window = true;

    init_select_window();
    
}
