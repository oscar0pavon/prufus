#ifndef WINDOW_H
#define WINDOW_H

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdbool.h>
#include <GL/gl.h>
#include <GL/glx.h>

#define WINDOW_WIDTH 500
#define WINDOW_HEIGHT 650

extern Display* display;

extern XEvent window_event;

extern bool prufus_window_running;
extern bool check_buttons_collision;

extern int mouse_click_x;
extern int mouse_click_y;

extern Window prufus_window; 

extern XSetWindowAttributes window_attributes;
extern XVisualInfo* window_visual;

extern GLXContext prufus_main_window_context;

int prufus_create_window();

void close_prufus_window();

void create_select_file_window();

#endif
