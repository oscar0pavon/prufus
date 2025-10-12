#include "button.h"
#include "opengl.h"
#include <stdbool.h>

#include "window.h"

#include "user_interface.h"

bool check_button_clicked(Button *button) {

    if(!check_buttons_collision){
        return false;
    }

  if (mouse_click_x >= button->aabb.min.x &&
      mouse_click_x <= button->aabb.max.x &&
      mouse_click_y >= button->aabb.min.y &&
      mouse_click_y <= button->aabb.max.y) {

      check_buttons_collision = false;
      mouse_click_x = 0;
      mouse_click_y = 0;
        return true;
  } else {
    return false;
  }
}

void button_new(Button* out, Vec2 position, Vec2 dimension){
    
    out->position = position; 
    out->dimention = dimension;

    out->aabb.min.x = position.x;
    out->aabb.min.y = position.y;
    out->aabb.max.x = position.x + dimension.x;
    out->aabb.max.y = position.y + dimension.y;
}

void draw_button(Button* button){

    gl_draw_button(button->position.x, button->position.y,
            button->dimention.x, button->dimention.y,
            3, 2);

    Vec2 center_x;
    center_x.x = button->position.x+5;//offset TODO
    center_x.y = button->position.y + button->dimention.y/2;
    center_x.y = center_x.y - 10;//offset for center in Y


    draw_text(button->text,center_x.x, center_x.y,24);

    
}
