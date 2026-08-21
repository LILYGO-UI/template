#include "app.h"

#include "app_identity.h"
#include "app_model.h"

#include <stdint.h>

typedef struct app_view_state {
    app_model_t model;
    app_mode_t mode;
    lv_obj_t *surface;
    lv_obj_t *panel;
    lv_obj_t *value_label;
    lv_obj_t *controls;
    lv_obj_t *decrement_button;
    lv_obj_t *reset_button;
    lv_obj_t *increment_button;
} app_view_state_t;

static app_view_state_t app_state = {.mode = APP_MODE_AUTO};

static void update_counter_label(void)
{
    if (app_state.value_label)
        lv_label_set_text_fmt(app_state.value_label, "%d",
                              app_state.model.counter);
}

static void counter_event(lv_event_t *event)
{
    intptr_t action = (intptr_t)lv_event_get_user_data(event);

    if (action < 0)
        app_model_decrement(&app_state.model);
    else if (action > 0)
        app_model_increment(&app_state.model);
    else
        app_model_reset(&app_state.model);
    update_counter_label();
}

static lv_obj_t *create_button(lv_obj_t *parent, const char *text,
                               intptr_t action, bool icon_button)
{
    lv_obj_t *button = lv_button_create(parent);
    lv_obj_set_size(button, icon_button ? 64 : 120, 56);
    lv_obj_set_style_radius(button, 8, 0);
    lv_obj_set_style_bg_color(button, lv_color_hex(0x20262d), 0);
    lv_obj_add_event_cb(button, counter_event, LV_EVENT_CLICKED,
                        (void *)action);

    lv_obj_t *label = lv_label_create(button);
    lv_label_set_text(label, text);
    lv_obj_set_style_text_font(label, CM0_FONT_UI_22, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0xffffff), 0);
    lv_obj_center(label);
    return button;
}

static void apply_layout(void)
{
    int32_t width;
    bool desktop;
    bool compact;

    if (!app_state.surface || !app_state.panel || !app_state.controls ||
        !app_state.decrement_button || !app_state.reset_button ||
        !app_state.increment_button)
        return;
    width = lv_obj_get_content_width(app_state.surface);
    compact = width < 400;
    desktop = width >= 640 &&
              (app_state.mode == APP_MODE_DESKTOP ||
               (app_state.mode == APP_MODE_AUTO && width >= 800));
    lv_obj_set_width(app_state.panel,
                     compact ? LV_PCT(94) : (desktop ? 560 : LV_PCT(92)));
    lv_obj_set_style_pad_all(app_state.panel,
                             desktop ? 40 : (compact ? 16 : 28), 0);
    lv_obj_set_style_pad_column(app_state.controls, compact ? 8 : 12, 0);
    lv_obj_set_size(app_state.decrement_button, compact ? 56 : 64, 56);
    lv_obj_set_size(app_state.reset_button, compact ? 96 : 120, 56);
    lv_obj_set_size(app_state.increment_button, compact ? 56 : 64, 56);
}

static void size_changed_event(lv_event_t *event)
{
    (void)event;
    apply_layout();
}

static void app_open(const cm0_app_context_t *context)
{
    lv_obj_t *header;
    lv_obj_t *title;
    lv_obj_t *body;
    lv_obj_t *counter_title;
    lv_obj_t *controls;

    app_model_init(&app_state.model);

    app_state.surface = lv_obj_create(context->root);
    lv_obj_set_size(app_state.surface, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_border_width(app_state.surface, 0, 0);
    lv_obj_set_style_radius(app_state.surface, 0, 0);
    lv_obj_set_style_bg_color(app_state.surface, lv_color_hex(0xf4f6f8), 0);
    lv_obj_set_style_pad_all(app_state.surface, 0, 0);
    lv_obj_clear_flag(app_state.surface, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_flex_flow(app_state.surface, LV_FLEX_FLOW_COLUMN);
    lv_obj_add_event_cb(app_state.surface, size_changed_event,
                        LV_EVENT_SIZE_CHANGED, NULL);

    header = lv_obj_create(app_state.surface);
    lv_obj_set_width(header, LV_PCT(100));
    lv_obj_set_height(header, 88);
    lv_obj_set_style_border_width(header, 0, 0);
    lv_obj_set_style_border_side(header, LV_BORDER_SIDE_BOTTOM, 0);
    lv_obj_set_style_border_color(header, lv_color_hex(0xd8dde3), 0);
    lv_obj_set_style_border_width(header, 1, 0);
    lv_obj_set_style_radius(header, 0, 0);
    lv_obj_set_style_bg_color(header, lv_color_hex(0xffffff), 0);
    lv_obj_set_style_pad_hor(header, 28, 0);
    lv_obj_set_style_pad_ver(header, 18, 0);
    lv_obj_clear_flag(header, LV_OBJ_FLAG_SCROLLABLE);

    title = lv_label_create(header);
    lv_label_set_text(title, LILYGO_UI_TEMPLATE_APP_NAME);
    lv_obj_set_style_text_font(title, CM0_FONT_UI_36, 0);
    lv_obj_set_style_text_color(title, lv_color_hex(0x15191e), 0);
    lv_obj_align(title, LV_ALIGN_LEFT_MID, 0, 0);

    body = lv_obj_create(app_state.surface);
    lv_obj_set_width(body, LV_PCT(100));
    lv_obj_set_flex_grow(body, 1);
    lv_obj_set_style_border_width(body, 0, 0);
    lv_obj_set_style_radius(body, 0, 0);
    lv_obj_set_style_bg_opa(body, LV_OPA_TRANSP, 0);
    lv_obj_set_style_pad_all(body, 24, 0);
    lv_obj_set_style_pad_bottom(body, 52, 0);
    lv_obj_clear_flag(body, LV_OBJ_FLAG_SCROLLABLE);

    app_state.panel = lv_obj_create(body);
    lv_obj_set_height(app_state.panel, LV_SIZE_CONTENT);
    lv_obj_set_style_radius(app_state.panel, 8, 0);
    lv_obj_set_style_border_width(app_state.panel, 1, 0);
    lv_obj_set_style_border_color(app_state.panel, lv_color_hex(0xd8dde3), 0);
    lv_obj_set_style_bg_color(app_state.panel, lv_color_hex(0xffffff), 0);
    lv_obj_set_style_pad_row(app_state.panel, 24, 0);
    lv_obj_clear_flag(app_state.panel, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_flex_flow(app_state.panel, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(app_state.panel, LV_FLEX_ALIGN_CENTER,
                          LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_center(app_state.panel);

    counter_title = lv_label_create(app_state.panel);
    lv_label_set_text(counter_title, "Counter");
    lv_obj_set_style_text_font(counter_title, CM0_FONT_UI_22, 0);
    lv_obj_set_style_text_color(counter_title, lv_color_hex(0x59636f), 0);

    app_state.value_label = lv_label_create(app_state.panel);
    lv_obj_set_style_text_font(app_state.value_label, CM0_FONT_UI_48, 0);
    lv_obj_set_style_text_color(app_state.value_label,
                                lv_color_hex(0x15191e), 0);
    update_counter_label();

    controls = lv_obj_create(app_state.panel);
    app_state.controls = controls;
    lv_obj_set_size(controls, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_style_border_width(controls, 0, 0);
    lv_obj_set_style_bg_opa(controls, LV_OPA_TRANSP, 0);
    lv_obj_set_style_pad_all(controls, 0, 0);
    lv_obj_set_style_pad_column(controls, 12, 0);
    lv_obj_clear_flag(controls, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_flex_flow(controls, LV_FLEX_FLOW_ROW);

    app_state.decrement_button = create_button(controls, "-", -1, true);
    app_state.reset_button = create_button(controls, "Reset", 0, false);
    app_state.increment_button = create_button(controls, "+", 1, true);
    apply_layout();
}

static void app_close(void)
{
    if (app_state.surface)
        lv_obj_delete(app_state.surface);
    app_state.surface = NULL;
    app_state.panel = NULL;
    app_state.value_label = NULL;
    app_state.controls = NULL;
    app_state.decrement_button = NULL;
    app_state.reset_button = NULL;
    app_state.increment_button = NULL;
}

void lilygo_ui_template_set_mode(app_mode_t mode)
{
    app_state.mode = mode;
    apply_layout();
}

static const cm0_app_descriptor_t app_descriptor = {
    CM0_APP_API_VERSION,
    sizeof(cm0_app_descriptor_t),
    LILYGO_UI_TEMPLATE_APP_ID,
    app_open,
    app_close,
};

const cm0_app_descriptor_t *cm0_app_get_descriptor(void)
{
    return &app_descriptor;
}
