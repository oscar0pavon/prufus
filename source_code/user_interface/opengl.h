#ifndef PRUFUS_OPENGL_H
#define PRUFUS_OPENGL_H

#include <GL/gl.h>
#include <GL/glx.h>

extern GLuint font_texture_id;

void gl_draw_char(char character, float x, float y, float width, float height);

#endif
