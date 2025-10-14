#include "input.h"
#include "window.h"
#include "select_window.h"

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

                    int revert_to;
                    Window focused_window;
                    XGetInputFocus(display,&focused_window,&revert_to);

                    if(focused_window == prufus_window){
                         prufus_window_running = false;
                    }
                    if(focused_window == select_file_window){
                        can_draw_select_window = false;
                        XDestroyWindow(display,select_file_window);
                    }

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

            case FocusIn:

               //XSetInputFocus(display, prufus_window, RevertToParent, CurrentTime);
              break;

            case FocusOut:

              break;
            }
    }    
}
