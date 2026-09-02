#include "draw.h"

#include <pfonts/pfonts.h>
#include <pfonts/pfonts_cpu.h>

static const PColor button_border_color = {211.f/255.f, 211.f/255.f, 211.f/255.f};
static const PColor button_fill_color = {1.f, 1.f, 1.f};

static const PColor plane_fill_color = {53.f/255.f, 132.f/255.f, 228.f/255.f};
static const PColor plane_border_color = {0.f, 0.f, 0.f};

static const PColor text_color = {0.25f, 0.25f, 0.25f};

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

void draw_text(const char* text, float x, float y){
    float pen_x = x;
    float baseline_y = y + pfonts_get_ascent();

    for(int i = 0; text[i] != '\0'; ++i){
        uint32_t codepoint = (unsigned char)text[i];
        pfonts_cpu_draw_glyph(codepoint, text_color, pen_x, baseline_y);
        pen_x += pfonts_get_glyph_advance(codepoint);
    }
}
