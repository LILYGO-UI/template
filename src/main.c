#include "app.h"

#include "app_identity.h"

#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static bool has_desktop_session(void)
{
    return getenv("WAYLAND_DISPLAY") || getenv("DISPLAY") ||
           getenv("XDG_CURRENT_DESKTOP");
}

static bool has_pointer_device(void)
{
    return access("/dev/input/mice", R_OK) == 0 || has_desktop_session();
}

static int read_user_mode(app_mode_t *mode)
{
    char path[PATH_MAX];
    const char *config_home = getenv("XDG_CONFIG_HOME");
    const char *home = getenv("HOME");

    if (config_home && *config_home) {
        if (snprintf(path, sizeof(path), "%s/lilygo/ui/%s.conf", config_home,
                     LILYGO_UI_TEMPLATE_CONFIG_SLUG) >= (int)sizeof(path))
            return -1;
    } else if (home && *home) {
        if (snprintf(path, sizeof(path), "%s/.config/lilygo/ui/%s.conf", home,
                     LILYGO_UI_TEMPLATE_CONFIG_SLUG) >= (int)sizeof(path))
            return -1;
    } else {
        return 0;
    }
    return app_mode_read_config(path, mode);
}

static int resolve_configured_mode(app_mode_t *mode)
{
    char system_path[PATH_MAX];
    int user_result = read_user_mode(mode);
    int system_result;

    if (user_result > 0)
        return 1;
    if (snprintf(system_path, sizeof(system_path), "/etc/lilygo/ui/%s.conf",
                 LILYGO_UI_TEMPLATE_CONFIG_SLUG) >=
        (int)sizeof(system_path))
        return -1;
    system_result = app_mode_read_config(system_path, mode);
    if (system_result > 0)
        return 1;
    return user_result < 0 || system_result < 0 ? -1 : 0;
}

int main(int argc, char **argv)
{
    char **runtime_argv = calloc((size_t)argc + 1U, sizeof(*runtime_argv));
    app_mode_t requested_mode = APP_MODE_AUTO;
    bool has_cli_mode = false;
    int width = 568;
    int height = 1232;
    int runtime_argc = 1;
    int result;

    if (!runtime_argv) {
        fprintf(stderr, "%s: out of memory\n", LILYGO_UI_TEMPLATE_APP_ID);
        return 1;
    }
    runtime_argv[0] = argv[0];

    for (int i = 1; i < argc; ++i) {
        const char *mode_value = NULL;

        if (strncmp(argv[i], "--mode=", 7) == 0)
            mode_value = argv[i] + 7;
        else if (strcmp(argv[i], "--mode") == 0) {
            if (++i >= argc) {
                fprintf(stderr, "%s: --mode requires a value\n",
                        LILYGO_UI_TEMPLATE_APP_ID);
                free(runtime_argv);
                return 2;
            }
            mode_value = argv[i];
        } else {
            runtime_argv[runtime_argc++] = argv[i];
            if (strcmp(argv[i], "--width") == 0 && i + 1 < argc)
                width = atoi(argv[i + 1]);
            else if (strcmp(argv[i], "--height") == 0 && i + 1 < argc)
                height = atoi(argv[i + 1]);
            continue;
        }

        if (!app_mode_parse(mode_value, &requested_mode)) {
            fprintf(stderr, "%s: invalid mode '%s'\n",
                    LILYGO_UI_TEMPLATE_APP_ID, mode_value);
            free(runtime_argv);
            return 2;
        }
        has_cli_mode = true;
    }

    if (!has_cli_mode) {
        result = resolve_configured_mode(&requested_mode);
        if (result < 0)
            fprintf(stderr,
                    "%s: ignoring invalid ui.mode configuration\n",
                    LILYGO_UI_TEMPLATE_APP_ID);
    }
    if (requested_mode == APP_MODE_AUTO)
        requested_mode = app_mode_detect(width, height, has_desktop_session(),
                                         has_pointer_device());
    lilygo_ui_template_set_mode(requested_mode);
    fprintf(stderr, "[%s] mode=%s\n", LILYGO_UI_TEMPLATE_APP_ID,
            app_mode_name(requested_mode));

    result = cm0_app_run(runtime_argc, runtime_argv,
                         cm0_app_get_descriptor());
    free(runtime_argv);
    return result;
}
