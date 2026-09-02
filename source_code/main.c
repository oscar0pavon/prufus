#include <pway/pway.h>
#include <pfonts/pfonts.h>
#include <pfonts/pfonts_cpu.h>
#include <unistd.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>


#include "user_interface/window.h"
#include "user_interface/cpu_image.h"
#include "user_interface/draw.h"

#include "user_interface/select_window.h"
#include "user_interface/home_window.h"

#include "user_interface/user_interface.h"

#include "device.h"




int main() {
    printf("Welcome to prufus\n");

    if(prufus_create_window() != 0){
        return 1;
    }

    read_sys_block();

    init_user_interface_data();
    init_home_window();

    //This is the main rendering loop
    //All the things happends here
    while (prufus_window_running) {
      pway_handle_events();

      if(!prufus_window_running)
        break;

      int stride;
      uint32_t* pixels = pway_shm_get_buffer(&stride);

      if(pixels){

        pfonts_cpu_set_target(pixels, WINDOW_WIDTH, WINDOW_HEIGHT, stride / 4);
        cpu_image_set_target(pixels, WINDOW_WIDTH, WINDOW_HEIGHT, stride / 4);
        draw_set_target(pixels, WINDOW_WIDTH, WINDOW_HEIGHT, stride / 4);

        pfonts_cpu_draw_rect((PColor){background_color.r, background_color.g,
                                       background_color.b},
                              0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

        if(can_draw_select_window){

          draw_select_window();

        }else{

          draw_home_window();

        }

        pway_shm_commit(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
      }

      mouse_wheel_up = 0;
      mouse_wheel_down = 0;

      usleep(50000.f);
    }

    pway_finish();

    return 0;
}
