#include "window.h"
#include "input.h"

#include <stdio.h>

#include <pway/pway.h>
#include <pfonts/pfonts.h>
#include <pfonts/pfonts_cpu.h>

#define FONT_PATH "/usr/share/fonts/TTF/DejaVuSans.ttf"
#define FONT_PIXEL_HEIGHT 20.f

bool prufus_window_running = true;

bool check_buttons_collision = false;

int mouse_click_x = 0;
int mouse_click_y = 0;

int mouse_wheel_up = 0;
int mouse_wheel_down = 0;

void close_prufus_window(){
    prufus_window_running = false;
}

int prufus_create_window(){

    if(!pfonts_load_font(FONT_PATH, FONT_PIXEL_HEIGHT)){
        printf("Can't load font %s\n", FONT_PATH);
        return 1;
    }

    pway = pway_init();
    if(pway == NULL){
        printf("Can't init pway\n");
        return 1;
    }

    pway->exit = close_prufus_window;
    pway->click = prufus_mouse_click;
    pway->click_release = prufus_mouse_click_release;

    if(!pway_create_window("prufus", WINDOW_WIDTH, WINDOW_HEIGHT)){
        printf("Can't create Wayland window\n");
        return 1;
    }

    pway_init_shm();
    pfonts_cpu_init();

    return 0;
}
