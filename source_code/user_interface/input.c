#include "input.h"
#include "window.h"

#include <pway/pway.h>

void prufus_mouse_click(void){
  mouse_click_x = pway->mouse.x;
  mouse_click_y = pway->mouse.y;
}

void prufus_mouse_click_release(void){
  if(pway->mouse.current_button == &pway->mouse.wheel_up){
    mouse_wheel_up = 1;
  }else if(pway->mouse.current_button == &pway->mouse.wheel_down){
    mouse_wheel_down = 1;
  }else{
    check_buttons_collision = true;
  }
}
