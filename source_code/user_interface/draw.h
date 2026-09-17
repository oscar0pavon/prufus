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

/* Draws text at `scale` times the body size, from a font opened at that size
 * rather than the body font scaled up, and returns the width it drew at. */
float draw_text_scaled(const char* text, float x, float y, float scale);

float measure_text_scaled_width(const char* text, float scale);

void draw_close_fonts(void);

typedef enum StatusBarState{
    STATUS_BAR_IDLE,
    STATUS_BAR_WORKING,
    STATUS_BAR_SUCCESS,
    STATUS_BAR_ERROR,
} StatusBarState;

void draw_status_bar(StatusBarState state, float x, float y, float width, float height, const char* text);

#endif
