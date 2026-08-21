#ifndef LILYGO_UI_TEMPLATE_APP_H
#define LILYGO_UI_TEMPLATE_APP_H

#include <cm0/app.h>

#include "app_mode.h"

#ifdef __cplusplus
extern "C" {
#endif

void lilygo_ui_template_set_mode(app_mode_t mode);
const cm0_app_descriptor_t *cm0_app_get_descriptor(void);

#ifdef __cplusplus
}
#endif

#endif

