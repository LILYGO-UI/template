#ifndef LILYGO_UI_TEMPLATE_APP_MODEL_H
#define LILYGO_UI_TEMPLATE_APP_MODEL_H

typedef struct app_model {
    int counter;
} app_model_t;

void app_model_init(app_model_t *model);
void app_model_increment(app_model_t *model);
void app_model_decrement(app_model_t *model);
void app_model_reset(app_model_t *model);

#endif

