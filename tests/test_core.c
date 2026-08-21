#include "app_mode.h"
#include "app_model.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static void test_model(void)
{
    app_model_t model;
    app_model_init(&model);
    assert(model.counter == 0);
    app_model_decrement(&model);
    assert(model.counter == 0);
    app_model_increment(&model);
    assert(model.counter == 1);
    app_model_reset(&model);
    assert(model.counter == 0);
    for (int i = 0; i < 1100; ++i)
        app_model_increment(&model);
    assert(model.counter == 999);
}

static void test_modes(void)
{
    app_mode_t mode;
    char path[] = "/tmp/lilygo-ui-template-mode-XXXXXX";
    int fd = mkstemp(path);
    FILE *file;

    assert(app_mode_parse("auto", &mode) && mode == APP_MODE_AUTO);
    assert(app_mode_parse("phone", &mode) && mode == APP_MODE_PHONE);
    assert(app_mode_parse("desktop", &mode) && mode == APP_MODE_DESKTOP);
    assert(!app_mode_parse("tablet", &mode));
    assert(app_mode_detect(568, 1232, true, true) == APP_MODE_PHONE);
    assert(app_mode_detect(1024, 768, true, true) == APP_MODE_DESKTOP);
    assert(app_mode_detect(1024, 768, false, true) == APP_MODE_PHONE);

    assert(fd >= 0);
    file = fdopen(fd, "w");
    assert(file);
    assert(fputs("# user preference\nui.mode = desktop\n", file) >= 0);
    assert(fclose(file) == 0);
    assert(app_mode_read_config(path, &mode) == 1);
    assert(mode == APP_MODE_DESKTOP);
    assert(unlink(path) == 0);
    assert(app_mode_read_config(path, &mode) == 0);
}

int main(void)
{
    test_model();
    test_modes();
    return 0;
}
