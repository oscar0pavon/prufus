#ifndef BUTTON_H
#define BUTTON_H

#include "types.h"

typedef struct Button{
    AABB aabb;
    Vec2 position;
    Vec2 dimention;
    char text[50];
    void (*execute)();

}Button;

bool check_button_clicked(Button *button);

void button_new(Button* out, Vec2 position, Vec2 dimension);

void draw_button(Button* button);



#endif
