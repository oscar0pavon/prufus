#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>
#include <stdbool.h>

#define vec2(p1, p2)                                                           \
  (Vec2) { p1, p2 }

#define rgb(p1)                                                           \
  (float) { p1/255.0f }


typedef struct UV{
    float x;
    float y;
}UV;

typedef struct Vec2{
    uint16_t x;
    uint16_t y;
}Vec2;


//Axis Aligned Bounding Box
typedef struct AABB{
   Vec2 min;
   Vec2 max;
} AABB;

#endif
