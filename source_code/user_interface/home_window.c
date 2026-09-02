#include "home_window.h"

#include "window.h"
#include "button.h"
#include "draw.h"
#include "select_window.h"
#include "usb_creation.h"
#include "../device.h"

#include <stdio.h>
#include <string.h>
#include <time.h>

#define MARGIN 20
#define RIGHT_EDGE (WINDOW_WIDTH - MARGIN)
#define CONTENT_WIDTH (WINDOW_WIDTH - MARGIN*2)

#define BOTTOM_INFO_Y (WINDOW_HEIGHT - 40)
#define BUTTONS_Y (BOTTOM_INFO_Y - 20 - 34)
#define STATUS_BAR_Y (BUTTONS_Y - 15 - 40)
#define STATUS_HEADER_Y (STATUS_BAR_Y - 40)

static Button device_dropdown_button;
static Button select_iso_button;
static Button start_usb_button;
static Button cancel_button;

static Button device_list_entries[MAX_USB_DEVICES];
static bool device_list_open = false;
static int selected_device_index = -1;

static Button confirm_yes_button;
static Button confirm_no_button;
static bool confirm_open = false;

static Button success_close_button;

void init_home_window(){

  button_new(&device_dropdown_button, vec2(MARGIN, 76), vec2(CONTENT_WIDTH, 34));

  strcpy(select_iso_button.text, "SELECT");
  select_iso_button.execute = &create_select_file_window;
  button_new(&select_iso_button, vec2(670, 190), vec2(165, 34));

  strcpy(start_usb_button.text, "START");
  button_new(&start_usb_button, vec2(560, BUTTONS_Y), vec2(160, 34));

  strcpy(cancel_button.text, "CANCEL");
  button_new(&cancel_button, vec2(735, BUTTONS_Y), vec2(100, 34));

  strcpy(confirm_yes_button.text, "I'm sure");
  strcpy(confirm_no_button.text, "Cancel");

  strcpy(success_close_button.text, "Close");

  if(usb_device_count > 0){
    selected_device_index = 0;
  }
}

static void draw_device_dropdown(){

  bool dropdown_clicked = check_button_clicked(&device_dropdown_button);
  if(dropdown_clicked && !usb_creation_running && usb_device_count > 0){
    device_list_open = !device_list_open;
  }

  draw_button_outline(device_dropdown_button.position.x, device_dropdown_button.position.y,
      device_dropdown_button.dimention.x, device_dropdown_button.dimention.y);

  const char* device_label = usb_device_count == 0 ?
    "No USB device found" : usb_devices[selected_device_index].label;

  draw_text(device_label, device_dropdown_button.position.x + 10, device_dropdown_button.position.y + 8);
  draw_text("v", device_dropdown_button.position.x + device_dropdown_button.dimention.x - 20,
      device_dropdown_button.position.y + 8);

  if(!device_list_open)
    return;

  for(int i = 0; i < usb_device_count; i++){

    strncpy(device_list_entries[i].text, usb_devices[i].label, sizeof(device_list_entries[i].text) - 1);
    device_list_entries[i].text[sizeof(device_list_entries[i].text) - 1] = 0;

    Vec2 position = vec2(MARGIN, 114 + i * 28);
    button_new(&device_list_entries[i], position, vec2(CONTENT_WIDTH, 28));

    if(check_button_clicked(&device_list_entries[i])){
      selected_device_index = i;
      device_list_open = false;
    }

    draw_button(&device_list_entries[i]);
  }
}

static const char* iso_file_name(){
  const char* last_slash = strrchr(selected_iso_path, '/');
  return last_slash != NULL ? last_slash + 1 : selected_iso_path;
}

static void draw_boot_selection(){

  draw_button_outline(20, 190, 600, 34);

  if(has_selected_iso){
    draw_text(iso_file_name(), 30, 198);
    draw_text_accent("OK", 630, 198);
  }else{
    draw_text_muted("No boot image selected", 30, 198);
  }

  if(check_button_clicked(&select_iso_button)){
    if(!usb_creation_running && select_iso_button.execute != NULL){
      select_iso_button.execute();
    }
  }
  draw_button(&select_iso_button);
}

static StatusBarState current_status_state(){
  if(usb_creation_running) return STATUS_BAR_WORKING;
  if(usb_creation_success) return STATUS_BAR_SUCCESS;
  if(usb_creation_failed) return STATUS_BAR_ERROR;
  if(has_selected_iso && usb_device_count > 0) return STATUS_BAR_SUCCESS;
  return STATUS_BAR_IDLE;
}

static const char* current_status_text(){
  if(!usb_creation_running && !usb_creation_success && !usb_creation_failed
      && has_selected_iso && usb_device_count > 0){
    return "READY";
  }
  return usb_creation_status_text;
}

static void draw_bottom_bar(){

  char device_count_text[32];
  if(usb_device_count == 0){
    strcpy(device_count_text, "No device found");
  }else{
    snprintf(device_count_text, sizeof(device_count_text), "%d device%s found",
        usb_device_count, usb_device_count == 1 ? "" : "s");
  }
  draw_text_muted(device_count_text, MARGIN, BOTTOM_INFO_Y);

  if(usb_creation_running){
    time_t elapsed = time(NULL) - usb_creation_start_time;
    char elapsed_text[16];
    snprintf(elapsed_text, sizeof(elapsed_text), "%02d:%02d",
        (int)(elapsed / 60), (int)(elapsed % 60));
    draw_text_muted(elapsed_text, RIGHT_EDGE - measure_text_width(elapsed_text), BOTTOM_INFO_Y);
  }
}

static void draw_confirm_dialog(){

  int box_x = (WINDOW_WIDTH - 500) / 2;
  int box_y = 300;

  draw_button_outline(box_x, box_y, 500, 200);

  draw_text("WARNING! All data will be lost", box_x + 20, box_y + 15);
  draw_text(selected_iso_path, box_x + 20, box_y + 45);
  draw_text(usb_devices[selected_device_index].label, box_x + 20, box_y + 70);

  button_new(&confirm_yes_button, vec2(box_x + 30, box_y + 140), vec2(200, 34));
  button_new(&confirm_no_button, vec2(box_x + 270, box_y + 140), vec2(200, 34));

  if(check_button_clicked(&confirm_yes_button)){
    usb_creation_start(selected_iso_path, usb_devices[selected_device_index].path);
    confirm_open = false;
  }
  if(check_button_clicked(&confirm_no_button)){
    confirm_open = false;
  }

  draw_button(&confirm_yes_button);
  draw_button(&confirm_no_button);
}

static void draw_success_dialog(){

  int box_x = (WINDOW_WIDTH - 400) / 2;
  int box_y = 300;

  draw_button_outline(box_x, box_y, 400, 100);

  const char* message = "Success, now you can disconnect your USB!";
  draw_text(message, box_x + (400 - measure_text_width(message)) / 2, box_y + 20);

  button_new(&success_close_button, vec2(box_x + 125, box_y + 55), vec2(150, 30));

  if(check_button_clicked(&success_close_button)){
    usb_creation_success = false;
  }

  draw_button(&success_close_button);
}

void draw_home_window(){

  usb_creation_poll();

  draw_section_header("Drive Properties", MARGIN, 20, CONTENT_WIDTH);
  draw_text_muted("Device", MARGIN, 56);
  draw_device_dropdown();

  draw_text_muted("Boot selection", MARGIN, 170);
  draw_boot_selection();

  draw_section_header("Status", MARGIN, STATUS_HEADER_Y, CONTENT_WIDTH);
  draw_status_bar(current_status_state(), MARGIN, STATUS_BAR_Y, CONTENT_WIDTH, 40, current_status_text());

  bool can_start = has_selected_iso && usb_device_count > 0 && !usb_creation_running;
  if(check_button_clicked(&start_usb_button) && can_start){
    confirm_open = true;
  }
  draw_button(&start_usb_button);

  if(check_button_clicked(&cancel_button) && usb_creation_running){
    usb_creation_cancel();
  }
  draw_button(&cancel_button);

  draw_bottom_bar();

  if(confirm_open){
    draw_confirm_dialog();
  }

  if(usb_creation_success){
    draw_success_dialog();
  }
}
