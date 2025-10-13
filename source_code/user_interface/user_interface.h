#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H


typedef struct Color{
   float r;
   float g;
   float b;
   float a;
}Color;

extern Color background_color;

void draw_text(const char* text, float x, float y, int size);

void init_user_interface_data();

void load_texture();

#endif
