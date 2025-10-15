#ifndef PRUFUS_OPENGL_H
#define PRUFUS_OPENGL_H

#include <GL/gl.h>
#include <GL/glx.h>

extern GLuint font_texture_id;

void gl_draw_char(char character, float x, float y, float width, float height);

void init_opengl();

void draw_border(float x, float y, float width, float height, float radius, int segments);

void gl_draw_button(float x, float y, float width, float height, float radius, int segments);

void gl_draw_button_select_window(float x, float y, float width, float height, float radius, int segments);

void set_ortho_projection(float width, float height);

void gl_draw_button_plane(float x, float y, float width, float height);

#endif
