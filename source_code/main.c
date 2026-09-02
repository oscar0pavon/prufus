#include <pway/pway.h>
#include <pfonts/pfonts.h>
#include <pfonts/pfonts_cpu.h>
#include <string.h>
#include <unistd.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>


#include "user_interface/types.h"
#include "user_interface/window.h"
#include "user_interface/draw.h"
#include "user_interface/cpu_image.h"

#include "user_interface/select_window.h"

#include "user_interface/button.h"
#include "user_interface/user_interface.h"

#include "device.h"


Button start_button;
Button close_button;




int main() {
    printf("Welcome to prufus\n");

    if(prufus_create_window() != 0){
        return 1;
    }

    read_sys_block();

    strcpy(start_button.text,"Start");
    strcpy(close_button.text,"Close");
    close_button.execute = &close_prufus_window;

    button_new(&close_button, vec2(WINDOW_WIDTH-100,WINDOW_HEIGHT-80), vec2(80,30) );
    button_new(&start_button, vec2(WINDOW_WIDTH-200,WINDOW_HEIGHT-80), vec2(80,30) );

    Button select_button;

    strcpy(select_button.text,"Select");

    select_button.execute = &create_select_file_window;

    button_new(&select_button, vec2(WINDOW_WIDTH-100,160), vec2(80,30) );

    Button buttons[] = {
        start_button,
        close_button,
        select_button
    };

    init_user_interface_data();

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

        pfonts_cpu_draw_rect((PColor){background_color.r, background_color.g,
                                       background_color.b},
                              0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

        if(can_draw_select_window){

          draw_select_window();

        }else{

          int buttons_count = sizeof(buttons)/sizeof(Button);

          for(int i = 0; i < buttons_count; i++){
              if(check_button_clicked(&buttons[i])){
                  if(buttons[i].execute != NULL){
                      buttons[i].execute();
                  }
              }
              draw_button(&buttons[i]);
          }

          draw_text("prufus",370,15);
          draw_text("Device",0,80);
          draw_text("Boot selection",0,140);

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
