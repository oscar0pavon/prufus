#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H
#include <GL/gl.h>

typedef struct Color{
   float r;
   float g;
   float b;
   float a;
}Color;

extern Color background_color;

void draw_text(const char* text, float x, float y, int size);

void init_user_interface_data();

void load_texture(GLuint* texture_pointer, const char* path);
void load_textures();

#endif
