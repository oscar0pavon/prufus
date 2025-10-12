#ifndef BUTTON_H
#define BUTTON_H

#include "types.h"

bool check_button_clicked(Button *button);

void button_new(Button* out, Vec2 position, Vec2 dimension);

void draw_button(Button* button);


#endif
