#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H

#include "cpu_image.h"

typedef struct Color{
   float r;
   float g;
   float b;
   float a;
}Color;

extern Color background_color;

extern CpuImage directory_icon;

void init_user_interface_data();

#endif
