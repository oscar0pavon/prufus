#include "user_interface.h"

#include "types.h"

Color background_color;

CpuImage directory_icon;

void init_user_interface_data(){

  background_color.r = rgb(246.f) ;
  background_color.g = rgb(245.f);
  background_color.b = rgb(244.f);

  cpu_image_load(&directory_icon, "images/directory.png");

}
