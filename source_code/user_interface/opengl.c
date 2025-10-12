#include "opengl.h"
#include "types.h"
#include "math.h"

GLuint font_texture_id;

void gl_draw_char(char character, float x, float y, float width, float height) {

    int ascii_value = (int)character;

    int char_x = ascii_value%16;
    int char_y = floor(ascii_value/16);

    float char_size_x = 32.f/512.f;
    float char_size_y = 32.f/512.f;


    UV uv1;
    UV uv2;
    UV uv3;
    UV uv4;

    //button left
    uv1.x = (float)char_x*char_size_x;
    uv1.y = (float)char_y*char_size_y;

    //button right
    uv2.x = (char_x+1)*char_size_x;
    uv2.y = char_y*char_size_y;


    //top right
    uv3.x = (char_x+1)*char_size_x;
    uv3.y = (char_y+1)*char_size_y;


    //top left
    uv4.x = char_x*char_size_x;
    uv4.y = (char_y+1)*char_size_y;
    
    float alpha_value = 0.f;

    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
    
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindTexture(GL_TEXTURE_2D, font_texture_id);

    float gray = 0.25;
    glColor3f(gray, gray, gray); 

    glBegin(GL_QUADS);
    glTexCoord2f(uv1.x, uv1.y);
        glVertex2f(x, y);
    glTexCoord2f(uv2.x, uv2.y);
        glVertex2f(x + width, y);
    glTexCoord2f(uv3.x, uv3.y);
        glVertex2f(x + width, y + height);
    glTexCoord2f(uv4.x, uv4.y);
        glVertex2f(x, y + height);
    glEnd();

    glDisable(GL_TEXTURE_2D);
    
    glDisable(GL_BLEND);


}
