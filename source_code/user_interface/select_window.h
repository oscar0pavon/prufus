#ifndef SELECT_WINDOW_H
#define SELECT_WINDOW_H

#include <stdbool.h>

#define MAX_ISO_PATH_LEN 512

extern bool can_draw_select_window;

extern bool has_selected_iso;
extern char selected_iso_path[MAX_ISO_PATH_LEN];

void create_select_file_window();

void draw_select_window();

void free_select_window();

void close_select_window();

#endif
