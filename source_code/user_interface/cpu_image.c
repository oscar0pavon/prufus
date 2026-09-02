#include "cpu_image.h"

#include <stdlib.h>
#include <stdio.h>
#include <pixman.h>

#include "lodepng.h"

static uint32_t* target_pixels = NULL;
static int target_width = 0;
static int target_height = 0;
static int target_stride = 0; /* pixels, not bytes */

bool cpu_image_load(CpuImage* out, const char* path){
    unsigned char* rgba = NULL;
    unsigned width, height;

    unsigned error = lodepng_decode32_file(&rgba, &width, &height, path);
    if(error || !rgba){
        printf("Can't load image %s\n", path);
        return false;
    }

    uint32_t* pixels = malloc(width * height * sizeof(uint32_t));
    if(!pixels){
        free(rgba);
        return false;
    }

    for(unsigned i = 0; i < width * height; i++){
        unsigned char r = rgba[i*4 + 0];
        unsigned char g = rgba[i*4 + 1];
        unsigned char b = rgba[i*4 + 2];
        unsigned char a = rgba[i*4 + 3];

        r = (unsigned char)(r * a / 255);
        g = (unsigned char)(g * a / 255);
        b = (unsigned char)(b * a / 255);

        pixels[i] = ((uint32_t)a << 24) | ((uint32_t)r << 16) |
                    ((uint32_t)g << 8) | (uint32_t)b;
    }

    free(rgba);

    out->pixels = pixels;
    out->width = (int)width;
    out->height = (int)height;

    return true;
}

void cpu_image_set_target(uint32_t* pixels, int width, int height, int stride){
    target_pixels = pixels;
    target_width = width;
    target_height = height;
    target_stride = stride;
}

void cpu_image_draw(const CpuImage* image, float x, float y){
    if(!target_pixels || !image->pixels)
        return;

    pixman_image_t* source = pixman_image_create_bits(PIXMAN_a8r8g8b8,
        image->width, image->height, image->pixels, image->width * 4);

    pixman_image_t* target = pixman_image_create_bits(PIXMAN_x8r8g8b8,
        target_width, target_height, target_pixels, target_stride * 4);

    pixman_image_composite32(PIXMAN_OP_OVER, source, NULL, target,
        0, 0, 0, 0, (int)(x + 0.5f), (int)(y + 0.5f), image->width, image->height);

    pixman_image_unref(source);
    pixman_image_unref(target);
}
