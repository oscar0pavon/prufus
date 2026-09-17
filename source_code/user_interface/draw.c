#include "draw.h"
#include "font.h"

#include <pfonts/pfonts.h>
#include <pfonts/pfonts_cpu.h>
#include <stdlib.h>
#include <math.h>

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

#define HEADER_TEXT_SCALE 1.7f

/* A glyph is rasterized at the size it is drawn at, so a title is a font
 * opened at the bigger size rather than the body font blown up - pfonts used
 * to offer only one size, and scaling the rendered text with pixman is what
 * made headings blurry. Sizes are few and fixed, so each one is opened once
 * and kept. */
#define MAX_SCALED_FONTS 4

static struct {
    float scale;
    PFontsFont* font;
} scaled_fonts[MAX_SCALED_FONTS];

static PFontsFont* scaled_font(float scale){
    for(int i = 0; i < MAX_SCALED_FONTS; ++i){
        if(scaled_fonts[i].font && scaled_fonts[i].scale == scale)
            return scaled_fonts[i].font;
    }

    for(int i = 0; i < MAX_SCALED_FONTS; ++i){
        if(scaled_fonts[i].font)
            continue;

        scaled_fonts[i].font = pfonts_open_font(FONT_PATH, FONT_PIXEL_HEIGHT * scale);
        scaled_fonts[i].scale = scale;
        return scaled_fonts[i].font;
    }

    return NULL;
}

void draw_close_fonts(){
    for(int i = 0; i < MAX_SCALED_FONTS; ++i){
        pfonts_close_font(scaled_fonts[i].font);
        scaled_fonts[i].font = NULL;
    }
}

float measure_text_scaled_width(const char* text, float scale){
    PFontsFont* font = scaled_font(scale);

    if(!font)
        return measure_text_width(text) * scale;

    PFontsFont* previous = pfonts_use_font(font);
    float width = measure_text_width(text);
    pfonts_use_font(previous);

    return width;
}

float draw_text_scaled(const char* text, float x, float y, float scale){
    PFontsFont* font = scaled_font(scale);

    if(!font)
        return 0;

    PFontsFont* previous = pfonts_use_font(font);

    draw_text_with_color(text, x, y, text_color);
    float width = measure_text_width(text);

    pfonts_use_font(previous);

    return width;
}

/* Rule runs beside the title - filling the rest of the row - like Rufus's
 * section headers ("Drive Properties ───", "Status ───") instead of
 * underlining it. */
void draw_section_header(const char* text, float x, float y, float width){
    float text_end_x = x + draw_text_scaled(text, x, y, HEADER_TEXT_SCALE);

    float line_x = text_end_x + 10;
    float line_end_x = x + width;
    if(line_x < line_end_x){
        float line_y = y + pfonts_get_cell_height() * HEADER_TEXT_SCALE / 2;
        pfonts_cpu_draw_rect(text_color, line_x, line_y, line_end_x - line_x, 2);
    }
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
