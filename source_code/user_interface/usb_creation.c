#include "usb_creation.h"
#include "../scripts_names.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>

bool usb_creation_running = false;
bool usb_creation_success = false;
bool usb_creation_failed = false;
char usb_creation_status_text[64] = "Nothing to do";
time_t usb_creation_start_time = 0;

static pid_t usb_creation_pid = -1;

void usb_creation_start(const char* iso_path, const char* device_path){

  remove("/tmp/prufus/status");

  pid_t pid = fork();
  if(pid == 0){
    execl(graphics_sudo_script, graphics_sudo_script, make_usb_script,
        iso_path, device_path, (char*)NULL);
    _exit(127);
  }

  usb_creation_pid = pid;
  usb_creation_running = true;
  usb_creation_success = false;
  usb_creation_failed = false;
  usb_creation_start_time = time(NULL);
  strcpy(usb_creation_status_text, "Starting...");
}

void usb_creation_cancel(){
  if(usb_creation_pid > 0){
    kill(usb_creation_pid, SIGKILL);
    waitpid(usb_creation_pid, NULL, 0);
  }
  usb_creation_pid = -1;
  usb_creation_running = false;
  strcpy(usb_creation_status_text, "Canceled!");
}

void usb_creation_poll(){

  if(!usb_creation_running)
    return;

  if(waitpid(usb_creation_pid, NULL, WNOHANG) == usb_creation_pid){
    usb_creation_pid = -1;
    usb_creation_running = false;
    usb_creation_failed = true;
    strcpy(usb_creation_status_text, "Failed!");
    return;
  }

  int status_fd = open("/tmp/prufus/status", O_RDONLY | O_NONBLOCK);
  if(status_fd == -1)
    return;

  char stage = 0;
  ssize_t bytes_read = read(status_fd, &stage, 1);
  close(status_fd);

  if(bytes_read != 1)
    return;

  switch(stage){
    case '1': strcpy(usb_creation_status_text, "Formatting"); break;
    case '2': strcpy(usb_creation_status_text, "Creating temporal files"); break;
    case '3': strcpy(usb_creation_status_text, "Mounting"); break;
    case '4': strcpy(usb_creation_status_text, "Copying files"); break;
    case '5': strcpy(usb_creation_status_text, "Copying files"); break;
    case '6': strcpy(usb_creation_status_text, "Syncronizing disks"); break;
    case '7': strcpy(usb_creation_status_text, "Cleaning"); break;
    case '8':
      strcpy(usb_creation_status_text, "Success!, you can disconnect your USB");
      usb_creation_running = false;
      usb_creation_success = true;
      waitpid(usb_creation_pid, NULL, 0);
      usb_creation_pid = -1;
      break;
    default: break;
  }
}
