#include "draw.h"

#include <pfonts/pfonts.h>
#include <pfonts/pfonts_cpu.h>

static const PColor button_border_color = {211.f/255.f, 211.f/255.f, 211.f/255.f};
static const PColor button_fill_color = {1.f, 1.f, 1.f};

static const PColor plane_fill_color = {53.f/255.f, 132.f/255.f, 228.f/255.f};
static const PColor plane_border_color = {0.f, 0.f, 0.f};

static const PColor text_color = {0.25f, 0.25f, 0.25f};
static const PColor muted_text_color = {0.6f, 0.6f, 0.6f};
static const PColor accent_text_color = {0.13f, 0.55f, 0.13f};
static const PColor white_text_color = {1.f, 1.f, 1.f};

static const PColor status_idle_color = {0.85f, 0.85f, 0.85f};
static const PColor status_working_color = {0.2f, 0.5f, 0.85f};
static const PColor status_success_color = {0.18f, 0.6f, 0.2f};
static const PColor status_error_color = {0.8f, 0.2f, 0.2f};

/* Rounded corners had no CPU equivalent worth building - pfonts_cpu_draw_rect
 * is axis-aligned only - so the button look is now a 1px light-gray border
 * around a white fill instead of the old rounded outline. */
void draw_button_outline(float x, float y, float width, float height){
    pfonts_cpu_draw_rect(button_border_color, x - 1, y - 1, width + 2, height + 2);
    pfonts_cpu_draw_rect(button_fill_color, x, y, width, height);
}

void draw_button_plane(float x, float y, float width, float height){
    pfonts_cpu_draw_rect(plane_fill_color, x, y, width, height);

    pfonts_cpu_draw_rect(plane_border_color, x, y, width, 1);
    pfonts_cpu_draw_rect(plane_border_color, x, y + height - 1, width, 1);
    pfonts_cpu_draw_rect(plane_border_color, x, y, 1, height);
    pfonts_cpu_draw_rect(plane_border_color, x + width - 1, y, 1, height);
}

static void draw_text_with_color(const char* text, float x, float y, PColor color){
    float pen_x = x;
    float baseline_y = y + pfonts_get_ascent();

    for(int i = 0; text[i] != '\0'; ++i){
        uint32_t codepoint = (unsigned char)text[i];
        pfonts_cpu_draw_glyph(codepoint, color, pen_x, baseline_y);
        pen_x += pfonts_get_glyph_advance(codepoint);
    }
}

void draw_text(const char* text, float x, float y){
    draw_text_with_color(text, x, y, text_color);
}

void draw_text_muted(const char* text, float x, float y){
    draw_text_with_color(text, x, y, muted_text_color);
}

void draw_text_accent(const char* text, float x, float y){
    draw_text_with_color(text, x, y, accent_text_color);
}

float measure_text_width(const char* text){
    float width = 0;
    for(int i = 0; text[i] != '\0'; ++i){
        width += pfonts_get_glyph_advance((unsigned char)text[i]);
    }
    return width;
}

/* Fake-bold: the CPU backend has no bold face, so a section title is drawn
 * twice with a 1px horizontal offset, then underlined like Rufus's section
 * headers ("Drive Properties", "Status"). */
void draw_section_header(const char* text, float x, float y, float width){
    draw_text_with_color(text, x, y, text_color);
    draw_text_with_color(text, x + 1, y, text_color);
    pfonts_cpu_draw_rect(text_color, x, y + 24, width, 2);
}

void draw_status_bar(StatusBarState state, float x, float y, float width, float height, const char* text){
    PColor fill_color;
    switch(state){
        case STATUS_BAR_WORKING: fill_color = status_working_color; break;
        case STATUS_BAR_SUCCESS: fill_color = status_success_color; break;
        case STATUS_BAR_ERROR: fill_color = status_error_color; break;
        default: fill_color = status_idle_color; break;
    }

    pfonts_cpu_draw_rect(fill_color, x, y, width, height);

    PColor label_color = state == STATUS_BAR_IDLE ? text_color : white_text_color;

    float text_x = x + (width - measure_text_width(text)) / 2;
    float text_y = y + (height - pfonts_get_cell_height()) / 2;
    draw_text_with_color(text, text_x, text_y, label_color);
}
