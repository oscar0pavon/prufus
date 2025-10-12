#include "device.h"
#include <linux/limits.h>
#include <unistd.h>
#include <dirent.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>


// In Linux, devices that can have storage properties are called -block device-
// For get blocks devices we need to navigate in /sys/block
// then we need to know if the block device is -removable-


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
            
        }
    }
    
  }
}
