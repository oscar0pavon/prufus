#include <GL/gl.h>
#include <GL/glx.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <pthread.h>
#include <math.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


#include "user_interface/window.h"

#define vec2(p1, p2)                                                           \
  (Vec2) { p1, p2 }

#define rgb(p1, p2, p3)                                                           \
  (Vec3) { p1/255.0f, p2/255.0f, p3/255.0f }




GLuint font_texture_id;



typedef struct UV{
    float x;
    float y;
}UV;

typedef struct Vec2{
    uint16_t x;
    uint16_t y;
}Vec2;


//Axis Aligned Bounding Box
typedef struct AABB{
   Vec2 min;
   Vec2 max;
} AABB;

typedef struct Button{
    AABB aabb;
    Vec2 position;
    Vec2 dimention;
    char text[50];

}Button;

Button start_button;
Button close_button;

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


void init_opengl(){
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
     // Example for a 2D projection where 0,0 is top-left and width,height is bottom-right
    float left = 0.0f;
    float right = (float)500; // Replace with your window's width
    float bottom = (float)650; // Replace with your window's height
    float top = 0.0f;
    float near = -1.0f; // Near clipping plane
    float far = 1.0f;   // Far clipping plane

    glOrtho(left, right, bottom, top, near, far);

}

void draw_text(const char* text, float x, float y, int size) {
    int char_count = 0;

    for (int i = 0; text[i] != '\0'; ++i) {
        char_count++;
    }

    float current_x = x;

    // y = y-14;//offset

    for (int i = 0; text[i] != '\0'; ++i) {
        gl_draw_char(text[i], current_x, y,size,size);
        current_x += 10; // this the kerner (space betwen characters)
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
void draw_border(float x, float y, float width, float height, float radius, int segments) {

    glColor3f(211.f/255.f, 211.f/255.f, 211.f/255.f); 
    glLineWidth(2.0f);
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
    

    draw_border(x-1,y-1,width+2,height+2,radius,segments);

    glColor3f(1.f, 1.0f, 1.0f); // Black border
    glLineWidth(10.0f); // Border thickness

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


void load_texture(){

    int width, height, channels;
    unsigned char* image_data = stbi_load("images/font1.png",&width,&height,&channels,0);

    
    if(image_data){

      glGenTextures(1, &font_texture_id);

      glBindTexture(GL_TEXTURE_2D, font_texture_id);

      // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                   GL_UNSIGNED_BYTE, image_data);


      stbi_image_free(image_data);
    }else{
        printf("Can't load image texture: %s\n",stbi_failure_reason());
    }

}


int main() {

    prufus_create_window();


    init_opengl();
    
    pthread_t input_thread;

    pthread_create(&input_thread,NULL,handle_input,NULL);


    strcpy(start_button.text,"Start");
    strcpy(close_button.text,"Close");
    button_new(&close_button, vec2(500-100,620-50), vec2(80,30) );
    button_new(&start_button, vec2(400-100,620-50), vec2(80,30) );

    Button select_button;
    
    strcpy(select_button.text,"Select");


    button_new(&select_button, vec2(500-100,200-40), vec2(80,30) );

    Button buttons[] = {
        start_button, 
        close_button,
        select_button
    };


    load_texture();   
    
    while (prufus_window_running) {


        glClearColor(246.0f/255.0f, 245.0f/255.0f, 244.0f/255.0f, 1.0f); //default color
       
        //glClearColor(1,0,0,1);//test red


        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      
      


        int buttons_count = sizeof(buttons)/sizeof(Button);

        for(int i = 0; i < buttons_count; i++){
            if(check_button_clicked(&buttons[i])){
                printf("Button clicked!\n");
            }
            draw_button(&buttons[i]);
        }
        
        //draw_image(50,50,20,20,1,1,1);



        draw_text("prufus",195,15,24);
        draw_text("Device",0,80,24);
        draw_text("Boot selection",0,140,24);



        glFlush();
        
        glXSwapBuffers(display, prufus_window);

        usleep(50000.f);
    }

    XDestroyWindow(display, prufus_window);

    XCloseDisplay(display);

    return 0;
}
