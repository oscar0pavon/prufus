#include "draw.h"

#include <pfonts/pfonts.h>
#include <pfonts/pfonts_cpu.h>
#include <pixman.h>
#include <stdlib.h>
#include <math.h>

static uint32_t* target_pixels = NULL;
static int target_width = 0;
static int target_height = 0;
static int target_stride = 0; /* pixels, not bytes */

void draw_set_target(uint32_t* pixels, int width, int height, int stride){
    target_pixels = pixels;
    target_width = width;
    target_height = height;
    target_stride = stride;
}

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

/* Matches background_color in user_interface.c - the flat color the whole
 * window is cleared to before anything else is drawn. draw_text_scaled()
 * needs it to seed the off-screen buffer it anti-aliases glyphs against. */
static const PColor page_background_color = {246.f/255.f, 245.f/255.f, 244.f/255.f};

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

static uint32_t pack_opaque_color(PColor color){
    uint32_t r = (uint32_t)(color.r * 255.f + 0.5f);
    uint32_t g = (uint32_t)(color.g * 255.f + 0.5f);
    uint32_t b = (uint32_t)(color.b * 255.f + 0.5f);
    return 0xFF000000u | (r << 16) | (g << 8) | b;
}

/* pfonts has one global glyph atlas rasterized at whatever pixel_height was
 * passed to pfonts_load_font() at startup (see pfonts' truetype.c) - there is
 * no bigger point size to draw a title in directly. Instead this renders the
 * title once at normal size into a small off-screen buffer, seeded with
 * page_background_color so the glyphs anti-alias correctly, then has pixman
 * upscale that buffer onto the real target - producing an actually bigger
 * title instead of a fake-bold trick. Returns the scaled-up width drawn. */
static float draw_text_scaled(const char* text, float x, float y, float scale){
    int source_width = (int)ceilf(measure_text_width(text)) + 2;
    int source_height = (int)ceilf(pfonts_get_cell_height()) + 2;

    if(source_width <= 0 || source_height <= 0 || !target_pixels)
        return 0;

    uint32_t* source_pixels = malloc((size_t)source_width * source_height * sizeof(uint32_t));
    if(!source_pixels)
        return 0;

    uint32_t background_pixel = pack_opaque_color(page_background_color);
    for(int i = 0; i < source_width * source_height; ++i){
        source_pixels[i] = background_pixel;
    }

    pfonts_cpu_set_target(source_pixels, source_width, source_height, source_width);
    draw_text_with_color(text, 0, 0, text_color);
    pfonts_cpu_set_target(target_pixels, target_width, target_height, target_stride);

    pixman_image_t* source = pixman_image_create_bits(PIXMAN_x8r8g8b8,
        source_width, source_height, source_pixels, source_width * 4);
    pixman_image_set_filter(source, PIXMAN_FILTER_BILINEAR, NULL, 0);

    pixman_transform_t transform;
    pixman_transform_init_scale(&transform,
        pixman_double_to_fixed(1.0 / scale), pixman_double_to_fixed(1.0 / scale));
    pixman_image_set_transform(source, &transform);

    pixman_image_t* target = pixman_image_create_bits(PIXMAN_x8r8g8b8,
        target_width, target_height, target_pixels, target_stride * 4);

    float dest_width = source_width * scale;
    float dest_height = source_height * scale;

    pixman_image_composite32(PIXMAN_OP_SRC, source, NULL, target,
        0, 0, 0, 0, (int)(x + 0.5f), (int)(y + 0.5f),
        (int)(dest_width + 0.5f), (int)(dest_height + 0.5f));

    pixman_image_unref(source);
    pixman_image_unref(target);
    free(source_pixels);

    return dest_width;
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
