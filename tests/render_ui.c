#include "app.h"

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PARTIAL_ROWS 80

static lv_color32_t *framebuffer;
static int display_width;

static void flush_display(lv_display_t *display, const lv_area_t *area,
                          uint8_t *pixels)
{
    int width = area->x2 - area->x1 + 1;
    const lv_color32_t *source = (const lv_color32_t *)pixels;

    for (int y = area->y1; y <= area->y2; ++y) {
        memcpy(framebuffer + (size_t)y * (size_t)display_width + area->x1,
               source + (size_t)(y - area->y1) * (size_t)width,
               (size_t)width * sizeof(*source));
    }
    lv_display_flush_ready(display);
}

static int write_ppm(const char *path, const lv_color32_t *pixels,
                     int width, int height)
{
    FILE *file = fopen(path, "wb");
    if (!file)
        return -1;
    fprintf(file, "P6\n%d %d\n255\n", width, height);
    for (int i = 0; i < width * height; ++i) {
        fputc(pixels[i].red, file);
        fputc(pixels[i].green, file);
        fputc(pixels[i].blue, file);
    }
    return fclose(file);
}

int main(int argc, char **argv)
{
    lv_display_t *display;
    lv_color32_t *draw_buffer;
    cm0_app_context_t context = {0};
    const cm0_app_descriptor_t *descriptor = cm0_app_get_descriptor();
    int width;
    int height;
    bool has_contrast = false;

    assert(argc == 4);
    width = atoi(argv[1]);
    height = atoi(argv[2]);
    assert(width >= 320 && height >= 240);

    display_width = width;
    framebuffer = calloc((size_t)width * (size_t)height,
                         sizeof(*framebuffer));
    draw_buffer = calloc((size_t)width * PARTIAL_ROWS,
                         sizeof(*draw_buffer));
    assert(framebuffer && draw_buffer);
    lv_init();
    display = lv_display_create(width, height);
    assert(display);
    lv_display_set_buffers(display, draw_buffer, NULL,
                           (uint32_t)((size_t)width * PARTIAL_ROWS *
                                      sizeof(*draw_buffer)),
                           LV_DISPLAY_RENDER_MODE_PARTIAL);
    lv_display_set_flush_cb(display, flush_display);
    lv_display_set_default(display);

    context.root = lv_screen_active();
    lilygo_ui_template_set_mode(width >= 800 && height >= 600
                                    ? APP_MODE_DESKTOP
                                    : APP_MODE_PHONE);
    descriptor->open(&context);
    lv_obj_update_layout(context.root);
    lv_tick_inc(20);
    lv_refr_now(display);

    for (int i = 1; i < width * height; ++i) {
        if (memcmp(&framebuffer[i], &framebuffer[0],
                   sizeof(framebuffer[i])) != 0) {
            has_contrast = true;
            break;
        }
    }
    assert(has_contrast);
    assert(write_ppm(argv[3], framebuffer, width, height) == 0);

    descriptor->close();
    lv_display_delete(display);
    lv_deinit();
    free(draw_buffer);
    free(framebuffer);
    return 0;
}
