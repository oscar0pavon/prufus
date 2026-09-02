#ifndef CPU_IMAGE_H
#define CPU_IMAGE_H

#include <stdbool.h>
#include <stdint.h>

typedef struct CpuImage{
    uint32_t* pixels; /* premultiplied ARGB, row-major, top-left origin */
    int width;
    int height;
} CpuImage;

bool cpu_image_load(CpuImage* out, const char* path);

/* Points cpu_image_draw() at a destination buffer, same packing/stride
 * convention as pfonts_cpu_set_target() - call once per frame, right next to
 * that call, so both draw onto the same SHM buffer. */
void cpu_image_set_target(uint32_t* pixels, int width, int height, int stride);

void cpu_image_draw(const CpuImage* image, float x, float y);

#endif
