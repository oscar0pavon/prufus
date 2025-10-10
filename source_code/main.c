#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <GL/glx.h>
#include <unistd.h>
#include <stddef.h>
#include <stdbool.h>

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
  

    bool prufus_window_running = true;

    XEvent window_event;
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
            // Handle other event types (Expose, KeyPress, etc.)
        }


        glClearColor(246.0f/255.0f, 245.0f/255.0f, 244.0f/255.0f, 1.0f);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glXSwapBuffers(display, prufus_window);
    }

    XDestroyWindow(display, prufus_window);

    XCloseDisplay(display);

    return 0;
}
