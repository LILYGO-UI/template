#ifndef LILYGO_UI_TEMPLATE_APP_MODE_H
#define LILYGO_UI_TEMPLATE_APP_MODE_H

#include <stdbool.h>

typedef enum app_mode {
    APP_MODE_AUTO,
    APP_MODE_PHONE,
    APP_MODE_DESKTOP,
} app_mode_t;

const char *app_mode_name(app_mode_t mode);
bool app_mode_parse(const char *value, app_mode_t *mode);
int app_mode_read_config(const char *path, app_mode_t *mode);
app_mode_t app_mode_detect(int width, int height, bool desktop_session,
                           bool pointer_device);

#endif

