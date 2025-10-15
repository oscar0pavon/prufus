#ifndef SELECT_WINDOW_H
#define SELECT_WINDOW_H


#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdbool.h>


#include <GL/glx.h>

extern bool can_draw_select_window;
extern GLXContext select_window_context;
extern Window select_file_window;

void create_select_file_window();

void draw_select_window();

void free_select_window();

#endif
