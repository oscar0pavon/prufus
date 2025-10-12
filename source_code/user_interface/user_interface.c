#include "user_interface.h"
#include "opengl.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


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
