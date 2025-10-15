#include "select_window.h"

#include "window.h"

#include <stdio.h>

#include "user_interface.h"
#include "button.h"
#include <string.h>
#include <unistd.h>

#include "opengl.h"

#include <dirent.h>
#include <sys/types.h>

#include <stdlib.h>

#define SELECT_WINDOW_WIDTH 855
#define SELECT_WINDOW_HEIGHT 780

Window select_file_window;

bool can_draw_select_window = false;

GLXContext select_window_context;

Button open_select_window;
Button cancel_select_window;


int file_info_position_x = 150;


#define ENTRIES_COUNT 25

int select_file_current_entry = 0;

Button select_files_entries[ENTRIES_COUNT];


struct dirent** files_list;
int number_of_entries = 0;

bool can_read_directory = true;

int current_scroll_position = 0;

struct dirent** valid_files_list;
struct dirent* valid_files_mem;

int valid_files_count = 0;



void list_directory(const char* path){

  printf("List directory: %s\n", path);

  number_of_entries = scandir(path,&files_list,0,alphasort);

  if( number_of_entries < 0 ){
    printf("Error: can't read directory %s\n",path);
  }else{
   printf("Directory count: %i\n",number_of_entries); 
  }

  int valid = 0;
  for (int i = 0; i < number_of_entries; i++) {

    if (files_list[i]->d_name[0] == '.')
      continue;

    valid++;
  }

  valid_files_count = valid;

    
  struct dirent* test;


    valid_files_list = malloc(valid_files_count*sizeof(struct dirent*));


   
    int local_valid_files_count = 0;
    for(int i = 0 ; i < number_of_entries; i++){

      if(files_list[i]->d_name[0] == '.')
        continue;
      
      valid_files_list[local_valid_files_count] =  files_list[i];
        local_valid_files_count++;
    }


    printf("valid files %i\n", valid_files_count);
}

void draw_directory(){
    DIR *directory;
    struct dirent *entry;

  
    const char* home = getenv("HOME");
    const char* directory_to_read = home;
    //directory = opendir(directory_to_read);

    if(can_read_directory){
      list_directory(directory_to_read);
      can_read_directory = false; //we only read the directory one time
    }

    for(int i = 0; i < ENTRIES_COUNT;i++){
      if(check_button_clicked(&select_files_entries[i])){
        select_file_current_entry = i;
      }
    }

    

    if(mouse_wheel_up >= 1){
      current_scroll_position++;
    }
    
    if(mouse_wheel_down >= 1){
      if(current_scroll_position > 0){
        current_scroll_position--;
      }
    }

    int current_file_name = current_scroll_position;


    int current_entry_count = 0;



    for( ; current_file_name < valid_files_count; current_file_name++){

      if (current_entry_count >= ENTRIES_COUNT)
        break;


      int position_y = 50+(current_entry_count*25);

      if(select_file_current_entry == current_entry_count){

        gl_draw_button_plane(file_info_position_x-25,position_y,400,25);
      }

      if(valid_files_list[current_file_name]->d_type == DT_DIR){
        draw_image(directory_icon_id, file_info_position_x-15,position_y,20,20,
            1,1,1);
      }

      button_new(&select_files_entries[current_entry_count], (Vec2){file_info_position_x, position_y}, (Vec2){400,25});

      draw_text(valid_files_list[current_file_name]->d_name, file_info_position_x, position_y, 25);

      current_entry_count++;

    }



}


void free_select_window(){

  for(int i = 0; i< number_of_entries; i++){

    free(files_list[i]);

  }

  free(files_list);

  free(valid_files_list);

  can_read_directory = true;

  printf("Cleaned directory list\n");
}

void close_select_window(){

  hande_close_window(select_file_window);
 
}

void init_select_window(){
    
    strcpy(cancel_select_window.text,"Cancel");
    cancel_select_window.execute = &close_select_window;

    button_new(&cancel_select_window, vec2(SELECT_WINDOW_WIDTH-100,SELECT_WINDOW_HEIGHT-90), vec2(80,30) );

    select_file_current_entry = 0;

    //init buttons
    for(int i = 0; i < ENTRIES_COUNT;i++){
      select_files_entries[i].selected = false;
    }

    valid_files_count = 0;
}


void draw_select_window(){

  glXMakeCurrent(display, select_file_window, select_window_context);
  
  
  glViewport(0, 0, SELECT_WINDOW_WIDTH, SELECT_WINDOW_HEIGHT);

  set_ortho_projection(SELECT_WINDOW_WIDTH,SELECT_WINDOW_HEIGHT);

  glClearColor(background_color.r, background_color.g, background_color.b, 1);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


  draw_directory();

  Button buttons[] = {
      cancel_select_window,
  };

  int buttons_count = sizeof(buttons) / sizeof(Button);

  for (int i = 0; i < buttons_count; i++) {
    if (check_button_clicked(&buttons[i])) {
      if (buttons[i].execute != NULL) {
        buttons[i].execute();
      }
    }
    draw_button(&buttons[i]);
  }

  glXSwapBuffers(display, select_file_window);
}



void create_select_file_window(){

    select_file_window = XCreateWindow(display, RootWindow(display, window_visual->screen),
            0, 0, SELECT_WINDOW_WIDTH, SELECT_WINDOW_HEIGHT, 0, 
            window_visual->depth, InputOutput, window_visual->visual, 
            CWBorderPixel | CWColormap | CWEventMask, &window_attributes);
    
    Atom close_window = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, select_file_window, &close_window, 1);
    
    //set window name 
    XClassHint class_hint;
    class_hint.res_name = "Select File - prufus";
    class_hint.res_class = "Select File - prufus"; 
    XSetClassHint(display, select_file_window, &class_hint);

    XSetStandardProperties(display, select_file_window, 
            "Select File - prufus", "Select File - prufus", None, NULL, 0, NULL);


    XSelectInput(display, select_file_window, ButtonPressMask | ButtonReleaseMask | FocusChangeMask);
    
    //XSetInputFocus(display, select_file_window, RevertToParent, CurrentTime);

    select_window_context = glXCreateContext(display, window_visual, prufus_main_window_context, GL_TRUE); // GL_TRUE for direct rendering
    if (prufus_main_window_context == NULL) {
        printf("Can't create OpenGL context\n");
    }

    glXMakeCurrent(display, prufus_window, select_window_context);
    
    XMapWindow(display, select_file_window);

    can_draw_select_window = true;

    init_select_window();
    
}
