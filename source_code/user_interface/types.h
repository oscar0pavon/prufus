#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>

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

typedef struct Button{
    AABB aabb;
    Vec2 position;
    Vec2 dimention;
    char text[50];

}Button;
#endif
