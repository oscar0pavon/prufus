#include "select_window.h"

#include "window.h"

#include <stdio.h>

#include "user_interface.h"
#include "button.h"
#include "draw.h"
#include "cpu_image.h"
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <limits.h>

#include <dirent.h>
#include <sys/types.h>

#include <stdlib.h>

bool can_draw_select_window = false;

bool has_selected_iso = false;
char selected_iso_path[MAX_ISO_PATH_LEN] = "";

Button cancel_select_window;

int file_info_position_x = 40;

#define ENTRIES_COUNT 25
#define MAX_FILE_ENTRIES 1024

Button select_files_entries[ENTRIES_COUNT];

typedef struct FileEntry{
  char name[256];
  bool is_dir;
} FileEntry;

static FileEntry file_entries[MAX_FILE_ENTRIES];
static int file_entries_count = 0;

static char current_directory[PATH_MAX];
static bool directory_dirty = true;

int current_scroll_position = 0;

static bool has_iso_suffix(const char* name){
  size_t length = strlen(name);
  return length > 4 && strcasecmp(name + length - 4, ".iso") == 0;
}

void list_directory(const char* path){

  printf("List directory: %s\n", path);

  struct dirent** raw_entries;
  int raw_count = scandir(path, &raw_entries, 0, alphasort);

  if(raw_count < 0){
    printf("Error: can't read directory %s\n", path);
    file_entries_count = 0;
    return;
  }

  file_entries_count = 0;

  if(strcmp(path, "/") != 0 && file_entries_count < MAX_FILE_ENTRIES){
    strcpy(file_entries[file_entries_count].name, "..");
    file_entries[file_entries_count].is_dir = true;
    file_entries_count++;
  }

  for(int i = 0; i < raw_count; i++){

    const char* name = raw_entries[i]->d_name;

    if(name[0] != '.' && file_entries_count < MAX_FILE_ENTRIES){
      bool is_dir = raw_entries[i]->d_type == DT_DIR;

      if(is_dir || has_iso_suffix(name)){
        strncpy(file_entries[file_entries_count].name, name, sizeof(file_entries[0].name) - 1);
        file_entries[file_entries_count].name[sizeof(file_entries[0].name) - 1] = 0;
        file_entries[file_entries_count].is_dir = is_dir;
        file_entries_count++;
      }
    }

    free(raw_entries[i]);
  }

  free(raw_entries);

  printf("valid files %i\n", file_entries_count);
}

static void navigate_to(const char* path){
  strncpy(current_directory, path, sizeof(current_directory) - 1);
  current_directory[sizeof(current_directory) - 1] = 0;
  directory_dirty = true;
  current_scroll_position = 0;
}

static void navigate_up(){
  char* last_slash = strrchr(current_directory, '/');
  if(last_slash != NULL){
    if(last_slash == current_directory){
      last_slash[1] = 0; //keep root "/"
    }else{
      last_slash[0] = 0;
    }
  }
  navigate_to(current_directory);
}

static void navigate_into(const char* name){
  char next_directory[PATH_MAX];
  if(strcmp(current_directory, "/") == 0){
    snprintf(next_directory, sizeof(next_directory), "/%s", name);
  }else{
    snprintf(next_directory, sizeof(next_directory), "%s/%s", current_directory, name);
  }
  navigate_to(next_directory);
}

static void select_iso_file(const char* name){
  if(strcmp(current_directory, "/") == 0){
    snprintf(selected_iso_path, sizeof(selected_iso_path), "/%s", name);
  }else{
    snprintf(selected_iso_path, sizeof(selected_iso_path), "%s/%s", current_directory, name);
  }
  has_selected_iso = true;
  close_select_window();
}

static void activate_entry(int index){
  FileEntry* entry = &file_entries[index];

  if(strcmp(entry->name, "..") == 0){
    navigate_up();
  }else if(entry->is_dir){
    navigate_into(entry->name);
  }else{
    select_iso_file(entry->name);
  }
}

void draw_directory(){

  if(directory_dirty){
    list_directory(current_directory);
    directory_dirty = false;
  }

  draw_text(current_directory, file_info_position_x - 25, 15);

  if(mouse_wheel_up >= 1){
    current_scroll_position++;
  }

  if(mouse_wheel_down >= 1){
    if(current_scroll_position > 0){
      current_scroll_position--;
    }
  }

  int current_file_index = current_scroll_position;
  int current_entry_count = 0;

  for( ; current_file_index < file_entries_count; current_file_index++){

    if(current_entry_count >= ENTRIES_COUNT)
      break;

    int position_y = 50 + (current_entry_count * 25);

    FileEntry* entry = &file_entries[current_file_index];

    button_new(&select_files_entries[current_entry_count], (Vec2){file_info_position_x, position_y},
        (Vec2){WINDOW_WIDTH - file_info_position_x - 20, 25});

    if(check_button_clicked(&select_files_entries[current_entry_count])){
      activate_entry(current_file_index);
      return; //file_entries/current_directory may have just changed
    }

    if(entry->is_dir){
      cpu_image_draw(&directory_icon, file_info_position_x - 20, position_y + 4);
    }

    draw_text(entry->name, file_info_position_x, position_y);

    current_entry_count++;

  }

}

void free_select_window(){
  directory_dirty = true;
  printf("Cleaned directory list\n");
}

void close_select_window(){

  can_draw_select_window = false;

  free_select_window();

}

void init_select_window(){

  strcpy(cancel_select_window.text, "Cancel");
  cancel_select_window.execute = &close_select_window;

  button_new(&cancel_select_window, vec2(WINDOW_WIDTH-100,WINDOW_HEIGHT-90), vec2(80,30) );

  const char* home = getenv("HOME");
  navigate_to(home != NULL ? home : "/");
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
