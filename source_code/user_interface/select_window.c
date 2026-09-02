#include "select_window.h"

#include "window.h"

#include <stdio.h>

#include "user_interface.h"
#include "button.h"
#include "draw.h"
#include "cpu_image.h"
#include <string.h>
#include <unistd.h>

#include <dirent.h>
#include <sys/types.h>

#include <stdlib.h>

bool can_draw_select_window = false;

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

    const char* home = getenv("HOME");
    const char* directory_to_read = home;

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

        draw_button_plane(file_info_position_x-25,position_y,400,25);
      }

      if(valid_files_list[current_file_name]->d_type == DT_DIR){
        cpu_image_draw(&directory_icon, file_info_position_x-15,position_y);
      }

      button_new(&select_files_entries[current_entry_count], (Vec2){file_info_position_x, position_y}, (Vec2){400,25});

      draw_text(valid_files_list[current_file_name]->d_name, file_info_position_x, position_y);

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

  can_draw_select_window = false;

  free_select_window();

}

void init_select_window(){

    strcpy(cancel_select_window.text,"Cancel");
    cancel_select_window.execute = &close_select_window;

    button_new(&cancel_select_window, vec2(WINDOW_WIDTH-100,WINDOW_HEIGHT-90), vec2(80,30) );

    select_file_current_entry = 0;

    //init buttons
    for(int i = 0; i < ENTRIES_COUNT;i++){
      select_files_entries[i].selected = false;
    }

    valid_files_count = 0;
}


void draw_select_window(){

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
}



void create_select_file_window(){

    can_draw_select_window = true;

    init_select_window();

}
