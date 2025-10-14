#include "opengl.h"
#include "types.h"
#include "math.h"
#include "window.h"
#include <GL/gl.h>

GLuint font_texture_id;

void set_ortho_projection(float width, float height){


    glLoadIdentity();

    //2D projection where 0,0 is top-left and width,height is bottom-right
    float left = 0.0f;
    float right = (float)width; //window's width
    float bottom = (float)height; //window's height
    float top = 0.0f;
    float near = -1.0f; // Near clipping plane
    float far = 1.0f;   // Far clipping plane

    glOrtho(left, right, bottom, top, near, far);

}

void init_opengl(){
    set_ortho_projection(WINDOW_WIDTH,WINDOW_HEIGHT);
}

void draw_border(float x, float y, float width, float height, float radius, int segments) {

    glBegin(GL_POLYGON);

    // Top-right corner
    for (int i = 0; i <= segments; ++i) {
        float angle = M_PI_2 * i / segments; // 0 to 90 degrees
        glVertex2f(x + width - radius + radius * cos(angle), y + height - radius + radius * sin(angle));
    }
    // Top-left corner
    for (int i = 0; i <= segments; ++i) {
        float angle = M_PI_2 + M_PI_2 * i / segments; // 90 to 180 degrees
        glVertex2f(x + radius + radius * cos(angle), y + height - radius + radius * sin(angle));
    }
    // Bottom-left corner
    for (int i = 0; i <= segments; ++i) {
        float angle = M_PI + M_PI_2 * i / segments; // 180 to 270 degrees
        glVertex2f(x + radius + radius * cos(angle), y + radius + radius * sin(angle));
    }
    // Bottom-right corner
    for (int i = 0; i <= segments; ++i) {
        float angle = 1.5f * M_PI + M_PI_2 * i / segments; // 270 to 360 degrees
        glVertex2f(x + width - radius + radius * cos(angle), y + radius + radius * sin(angle));
    }
    glEnd();


}


void gl_draw_button(float x, float y, float width, float height, float radius, int segments) {
    

    glColor3f(211.f/255.f, 211.f/255.f, 211.f/255.f); 
    draw_border(x-1,y-1,width+2,height+2,radius,segments);

    glColor3f(1.f, 1.0f, 1.0f); // Black border

    draw_border(x,y,width,height,radius,segments);


}

void draw_image(float x, float y, float width, float height, 
        float r, float g, float b) {

    glBindTexture(GL_TEXTURE_2D, font_texture_id);
    
    glEnable(GL_TEXTURE_2D);

    //glColor3f(r, g, b); // Set button color
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f);
        glVertex2f(x, y);
    glTexCoord2f(1.0f, 0.0f);
        glVertex2f(x + width, y);
    glTexCoord2f(1.0f, 1.0f);
        glVertex2f(x + width, y + height);
    glTexCoord2f(0.0f, 1.0f);
        glVertex2f(x, y + height);
    glEnd();

    // // Draw border (optional)
    // glColor3f(0.0f, 0.0f, 0.0f); // Black border
    // glLineWidth(2.0f); // Border thickness
    // glBegin(GL_LINE_LOOP);
    //     glVertex2f(x, y);
    //     glVertex2f(x + width, y);
    //     glVertex2f(x + width, y + height);
    //     glVertex2f(x, y + height);
    // glEnd();

    glDisable(GL_TEXTURE_2D);

}
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
