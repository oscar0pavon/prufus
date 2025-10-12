#include "device.h"
#include <linux/limits.h>
#include <unistd.h>
#include <dirent.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

// In Linux, devices that can have storage properties are called -block device-
// For get blocks devices we need to navigate in /sys/block
// then we need to know if the block device is -removable-


bool is_usb_device(const char* device_path){
  char link_path[PATH_MAX];
  char resolved_path[PATH_MAX];

  snprintf(link_path,PATH_MAX, "/sys/block/%s",device_path);
  //printf("%s\n",link_path);//block path

  if(realpath(link_path,resolved_path)!= NULL){
    //printf("%s\n",resolved_path);//this is the PCI/USB path
    if (strstr(resolved_path, "/usb") != NULL) {
      return 1;
    }
  }
  return 0;
}

char* read_sys_file(const char* path){
  FILE* file = fopen(path,"r");
  if(!file){
    printf("Can't read %s\n",path);
    return NULL;
  }

  char* buffer = malloc(256);
  if(fgets(buffer,256,file)){
    buffer[strcspn(buffer,"\n")] = 0;
  }
  fclose(file);
  return buffer;
    
}

void get_model_name(const char* path){
  char* file = read_sys_file(path);
  printf("%s\n",file);
  free(file);
}

void get_size(const char* path){
  
  char* file = read_sys_file(path);
  if(file != NULL){
    unsigned long long size_in_blocks = strtoull(file, NULL, 10);
    unsigned long long size_in_bytes = size_in_blocks * 512;
    printf("Size: %llu bytes\n", size_in_bytes);
    free(file);
  }
}

void read_sys_block(){
  DIR* directory;
  struct dirent* entry;
  FILE* removable_file;
  char path[PATH_MAX];
  char removable_content[2];//we only need a char '1' or '0'

  directory = opendir("/sys/block");

  if(!directory){
      perror("open /sys/block");
      return;
  }
    
  while( (entry = readdir(directory)) != NULL ){
    // Skip . and .. directories
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
      continue;
    }
    
    snprintf(path,PATH_MAX,"/sys/block/%s/removable",entry->d_name);
    removable_file = fopen(path,"r");
    if(!removable_file)
        continue;

    if(fgets(removable_content, sizeof(removable_content), removable_file) != NULL) {
        if(removable_content[0] == '1'){//that means possibly is an USB stick or USB disk
          if(is_usb_device(entry->d_name)){
            snprintf(path,PATH_MAX,"/sys/block/%s/device/model",entry->d_name);

            printf("Found /dev/%s\n",entry->d_name);
            get_model_name(path);

            snprintf(path,PATH_MAX,"/sys/block/%s/size",entry->d_name);

            get_size(path);
            
          }
        }
    }
    
  }
}
