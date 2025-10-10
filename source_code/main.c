#include <GL/gl.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <GL/glx.h>
#include <unistd.h>
#include <stddef.h>
#include <stdbool.h>

#include "user_interface/cglm/cglm.h"


void init_opengl(){
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

     // Example for a 2D projection where 0,0 is top-left and width,height is bottom-right
    float left = 0.0f;
    float right = (float)500; // Replace with your window's width
    float bottom = (float)650; // Replace with your window's height
    float top = 0.0f;
    float near = -1.0f; // Near clipping plane
    float far = 1.0f;   // Far clipping plane

    glOrtho(left, right, bottom, top, near, far);

}

void draw_button(float x, float y, float width, float height, 
        float r, float g, float b) {

    glColor3f(r, g, b); // Set button color
    glBegin(GL_QUADS);
        glVertex2f(x, y);
        glVertex2f(x + width, y);
        glVertex2f(x + width, y + height);
        glVertex2f(x, y + height);
    glEnd();

    // Draw border (optional)
    glColor3f(0.0f, 0.0f, 0.0f); // Black border
    glLineWidth(2.0f); // Border thickness
    glBegin(GL_LINE_LOOP);
        glVertex2f(x, y);
        glVertex2f(x + width, y);
        glVertex2f(x + width, y + height);
        glVertex2f(x, y + height);
    glEnd();
}


int main() {

    Display* display = XOpenDisplay(NULL); // NULL for default display
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
    window_attributes.event_mask = ExposureMask | KeyPressMask | StructureNotifyMask;

    XClassHint class_hint;
    class_hint.res_name = "prufus";
    class_hint.res_class = "prufus"; 

    Window prufus_window = XCreateWindow(display, RootWindow(display, window_visual->screen),
            0, 0, 500, 650, 0, 
            window_visual->depth, InputOutput, window_visual->visual, 
            CWBorderPixel | CWColormap | CWEventMask, &window_attributes);

    XSetClassHint(display, prufus_window, &class_hint);
    XSetStandardProperties(display, prufus_window, 
            "prufus", "prufus", None, NULL, 0, NULL);

    // GL_TRUE for direct rendering
    GLXContext cx = glXCreateContext(display, window_visual, None, GL_TRUE); 
    if (cx == NULL) {
        // Handle error
    }
    
    Atom wm_delete_window = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, prufus_window, &wm_delete_window, 1);

    glXMakeCurrent(display, prufus_window, cx);

    XMapWindow(display, prufus_window);

    XSelectInput(display, prufus_window, ButtonPressMask);

    bool prufus_window_running = true;

    init_opengl();

    XEvent window_event;

    int mouse_click_x = 0;
    int mouse_click_y = 0;

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
            // Handle other event types (Expose, KeyPress, etc.)
        }


        glClearColor(246.0f/255.0f, 245.0f/255.0f, 244.0f/255.0f, 1.0f);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      

        draw_button(10,10,100,50,1,1,1);



        glFlush();
        
        glXSwapBuffers(display, prufus_window);
    }

    XDestroyWindow(display, prufus_window);

    XCloseDisplay(display);

    return 0;
}
