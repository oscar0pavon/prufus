#include <GL/gl.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <GL/glx.h>
#include <stdint.h>
#include <unistd.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <pthread.h>


bool prufus_window_running = true;
XEvent window_event;
    
int mouse_click_x = 0;
int mouse_click_y = 0;

Display* display;

typedef struct Vec2{
    uint8_t x;
    uint8_t y;
}Vec2;

typedef struct AABB{
   Vec2 min;
   Vec2 max;
} AABB;

typedef struct Button{
    AABB aabb;
    Vec2 position;
    Vec2 dimention;

}Button;

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

void button_new(Button* out, Vec2 position, Vec2 dimension){
    
    out->position = position; 
    out->dimention = dimension;

    out->aabb.min.x = position.x;
    out->aabb.min.y = position.y;
    out->aabb.max.x = position.x + dimension.x;
    out->aabb.max.y = position.y + dimension.y;
}

void gl_draw_button(float x, float y, float width, float height, 
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

void draw_button(Button* button){
    gl_draw_button(button->position.x, button->position.y, 
            button->dimention.x, button->dimention.y, 1 ,1 ,1);
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
        }
    }    
}

bool check_button_clicked(Button* button){
   if(mouse_click_x >= button->aabb.min.x && mouse_click_x <= button->aabb.max.x 
           && mouse_click_y >= button->aabb.min.y && mouse_click_y <= button->aabb.max.y) {
       return true;
   }else{
       return false;
   }
}


int main() {

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


    init_opengl();
    
    pthread_t input_thread;

    pthread_create(&input_thread,NULL,handle_input,NULL);

    Button create_button;
    button_new(&create_button, (Vec2){10,10}, (Vec2){100,50} );


    while (prufus_window_running) {


        glClearColor(246.0f/255.0f, 245.0f/255.0f, 244.0f/255.0f, 1.0f);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      
       
        if(check_button_clicked(&create_button)){
            printf("Button clicked\n");
        }


        draw_button(&create_button);





        glFlush();
        
        glXSwapBuffers(display, prufus_window);
    }

    XDestroyWindow(display, prufus_window);

    XCloseDisplay(display);

    return 0;
}
