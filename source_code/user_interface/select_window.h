#ifndef SELECT_WINDOW_H
#define SELECT_WINDOW_H

#include <stdbool.h>

extern bool can_draw_select_window;

void create_select_file_window();

void draw_select_window();

void free_select_window();

void close_select_window();

#endif
