
#include <string.h>
#include <unistd.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <pthread.h>
#include <math.h>


#include "user_interface/types.h"
#include "user_interface/window.h"
#include "user_interface/opengl.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"



#define vec2(p1, p2)                                                           \
  (Vec2) { p1, p2 }

#define rgb(p1, p2, p3)                                                           \
  (Vec3) { p1/255.0f, p2/255.0f, p3/255.0f }



Button start_button;
Button close_button;


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

    //This is the main rendering loop
    //All the things happends here
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
