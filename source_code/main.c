
#include <string.h>
#include <unistd.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <pthread.h>


#include "user_interface/types.h"
#include "user_interface/window.h"
#include "user_interface/opengl.h"

#include "user_interface/button.h"
#include "user_interface/user_interface.h"

#include "device.h"


Button start_button;
Button close_button;




int main() {
    printf("Welcome to prufus\n");

    prufus_create_window();

    read_sys_block();    

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
