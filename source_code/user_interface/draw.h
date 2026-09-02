#ifndef PRUFUS_DRAW_H
#define PRUFUS_DRAW_H

#include <stdint.h>

void draw_button_outline(float x, float y, float width, float height);

void draw_button_plane(float x, float y, float width, float height);

void draw_text(const char* text, float x, float y);

void draw_text_muted(const char* text, float x, float y);

void draw_text_accent(const char* text, float x, float y);

void draw_section_header(const char* text, float x, float y, float width);

float measure_text_width(const char* text);

/* Draws text scaled up by `scale` (pfonts has only one point size - see
 * draw_text_scaled()'s comment in draw.c for how this is faked) and returns
 * the width it drew at. */
float draw_text_scaled(const char* text, float x, float y, float scale);

float measure_text_scaled_width(const char* text, float scale);

typedef enum StatusBarState{
    STATUS_BAR_IDLE,
    STATUS_BAR_WORKING,
    STATUS_BAR_SUCCESS,
    STATUS_BAR_ERROR,
} StatusBarState;

void draw_status_bar(StatusBarState state, float x, float y, float width, float height, const char* text);

/* Must be called once per frame, alongside pfonts_cpu_set_target() /
 * cpu_image_set_target(), so draw_section_header() can composite scaled-up
 * text onto the real target buffer. */
void draw_set_target(uint32_t* pixels, int width, int height, int stride);

#endif
