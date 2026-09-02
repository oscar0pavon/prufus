#ifndef PRUFUS_DRAW_H
#define PRUFUS_DRAW_H

void draw_button_outline(float x, float y, float width, float height);

void draw_button_plane(float x, float y, float width, float height);

void draw_text(const char* text, float x, float y);

void draw_text_muted(const char* text, float x, float y);

void draw_text_accent(const char* text, float x, float y);

void draw_section_header(const char* text, float x, float y, float width);

float measure_text_width(const char* text);

typedef enum StatusBarState{
    STATUS_BAR_IDLE,
    STATUS_BAR_WORKING,
    STATUS_BAR_SUCCESS,
    STATUS_BAR_ERROR,
} StatusBarState;

void draw_status_bar(StatusBarState state, float x, float y, float width, float height, const char* text);

#endif
