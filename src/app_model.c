#include "app_model.h"

enum {
    APP_COUNTER_MIN = 0,
    APP_COUNTER_MAX = 999,
};

void app_model_init(app_model_t *model)
{
    if (model)
        model->counter = APP_COUNTER_MIN;
}

void app_model_increment(app_model_t *model)
{
    if (model && model->counter < APP_COUNTER_MAX)
        ++model->counter;
}

void app_model_decrement(app_model_t *model)
{
    if (model && model->counter > APP_COUNTER_MIN)
        --model->counter;
}

void app_model_reset(app_model_t *model)
{
    app_model_init(model);
}

