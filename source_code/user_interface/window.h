#ifndef WINDOW_H
#define WINDOW_H

#include <stdbool.h>

#define WINDOW_WIDTH 480
#define WINDOW_HEIGHT 680

extern bool prufus_window_running;
extern bool check_buttons_collision;

extern int mouse_click_x;
extern int mouse_click_y;

extern int mouse_wheel_up;
extern int mouse_wheel_down;

int prufus_create_window();

void close_prufus_window();

#endif
