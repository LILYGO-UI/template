#include "app.hpp"

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <string>
#include <vector>

namespace {

constexpr int kPartialRows                        = 80;
constexpr std::uint32_t kColorPageBackground      = 0xf2f2f7;
constexpr std::uint32_t kColorContainerBackground = 0xffffff;
constexpr std::uint32_t kColorContainerTitle      = 0x1a1a1a;
constexpr std::uint32_t kColorText                = 0x000000;

std::vector<lv_color32_t> framebuffer;
int display_width = 0;

void flush_display(lv_display_t *display, const lv_area_t *area, std::uint8_t *pixels)
{
    const int width    = area->x2 - area->x1 + 1;
    const auto *source = reinterpret_cast<const lv_color32_t *>(pixels);

    for (int y = area->y1; y <= area->y2; ++y) {
        const auto source_offset = static_cast<std::size_t>(y - area->y1) * static_cast<std::size_t>(width);
        const auto destination_offset =
            static_cast<std::size_t>(y) * static_cast<std::size_t>(display_width) + static_cast<std::size_t>(area->x1);
        std::copy_n(source + source_offset, width, framebuffer.begin() + destination_offset);
    }
    lv_display_flush_ready(display);
}

bool write_ppm(const char *path, int width, int height)
{
    std::ofstream file(path, std::ios::binary);
    if (!file) return false;

    file << "P6\n" << width << ' ' << height << "\n255\n";
    for (const auto &pixel : framebuffer) {
        file.put(static_cast<char>(pixel.red));
        file.put(static_cast<char>(pixel.green));
        file.put(static_cast<char>(pixel.blue));
    }
    return file.good();
}

bool colors_differ(const lv_color32_t &left, const lv_color32_t &right)
{
    return left.red != right.red || left.green != right.green || left.blue != right.blue || left.alpha != right.alpha;
}

}  // namespace

int main(int argc, char **argv)
{
    assert(argc == 4);
    const int width  = std::atoi(argv[1]);
    const int height = std::atoi(argv[2]);
    assert(width >= 320 && height >= 240);

    display_width = width;
    framebuffer.resize(static_cast<std::size_t>(width) * static_cast<std::size_t>(height));
    std::vector<lv_color32_t> draw_buffer(static_cast<std::size_t>(width) * kPartialRows);

    lv_init();
    auto *display = lv_display_create(width, height);
    assert(display);
    lv_display_set_buffers(display, draw_buffer.data(), nullptr,
                           static_cast<std::uint32_t>(draw_buffer.size() * sizeof(lv_color32_t)),
                           LV_DISPLAY_RENDER_MODE_PARTIAL);
    lv_display_set_flush_cb(display, flush_display);
    lv_display_set_default(display);

    cm0_app_context_t context{};
    context.root           = lv_screen_active();
    const auto *descriptor = cm0_app_get_descriptor();
    descriptor->open(&context);
    lv_obj_update_layout(context.root);

    auto *surface       = lv_obj_find_by_name(context.root, "app_surface");
    auto *header        = lv_obj_find_by_name(context.root, "app_header");
    auto *title         = lv_obj_find_by_name(context.root, "app_title");
    auto *panel         = lv_obj_find_by_name(context.root, "counter_panel");
    auto *counter_title = lv_obj_find_by_name(context.root, "counter_title");
    auto *counter_value = lv_obj_find_by_name(context.root, "counter_value");
    auto *increment     = lv_obj_find_by_name(context.root, "counter_increment");
    auto *reset         = lv_obj_find_by_name(context.root, "counter_reset");
    assert(surface && header && title && panel && counter_title && counter_value && increment && reset);
    assert(lv_color_eq(lv_obj_get_style_bg_color(surface, LV_PART_MAIN), lv_color_hex(kColorPageBackground)));
    assert(lv_obj_get_style_bg_opa(surface, LV_PART_MAIN) == LV_OPA_COVER);
    assert(lv_obj_get_style_bg_grad_dir(surface, LV_PART_MAIN) == LV_GRAD_DIR_NONE);
    assert(lv_color_eq(lv_obj_get_style_bg_color(header, LV_PART_MAIN), lv_color_hex(kColorContainerBackground)));
    assert(lv_obj_get_style_bg_opa(header, LV_PART_MAIN) == LV_OPA_COVER);
    assert(lv_color_eq(lv_obj_get_style_bg_color(panel, LV_PART_MAIN), lv_color_hex(kColorContainerBackground)));
    assert(lv_obj_get_style_bg_opa(panel, LV_PART_MAIN) == LV_OPA_COVER);
    assert(lv_color_eq(lv_obj_get_style_text_color(title, LV_PART_MAIN), lv_color_hex(kColorContainerTitle)));
    assert(lv_color_eq(lv_obj_get_style_text_color(counter_title, LV_PART_MAIN), lv_color_hex(kColorContainerTitle)));
    assert(lv_color_eq(lv_obj_get_style_text_color(counter_value, LV_PART_MAIN), lv_color_hex(kColorText)));
    assert(std::string(lv_label_get_text(counter_value)) == "0");
    lv_obj_send_event(increment, LV_EVENT_CLICKED, nullptr);
    assert(std::string(lv_label_get_text(counter_value)) == "1");
    lv_obj_send_event(reset, LV_EVENT_CLICKED, nullptr);
    assert(std::string(lv_label_get_text(counter_value)) == "0");

    lv_tick_inc(20);
    lv_refr_now(display);
    const bool has_contrast = std::any_of(framebuffer.cbegin() + 1, framebuffer.cend(),
                                          [](const auto &pixel) { return colors_differ(pixel, framebuffer[0]); });
    assert(has_contrast);
    assert(write_ppm(argv[3], width, height));

    descriptor->close();
    lv_display_delete(display);
    lilygo_ui_fonts_deinit();
    lv_deinit();
    return 0;
}
