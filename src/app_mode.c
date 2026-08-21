#include "app_mode.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

const char *app_mode_name(app_mode_t mode)
{
    switch (mode) {
    case APP_MODE_PHONE:
        return "phone";
    case APP_MODE_DESKTOP:
        return "desktop";
    case APP_MODE_AUTO:
    default:
        return "auto";
    }
}

bool app_mode_parse(const char *value, app_mode_t *mode)
{
    if (!value || !mode)
        return false;
    if (strcmp(value, "auto") == 0)
        *mode = APP_MODE_AUTO;
    else if (strcmp(value, "phone") == 0)
        *mode = APP_MODE_PHONE;
    else if (strcmp(value, "desktop") == 0)
        *mode = APP_MODE_DESKTOP;
    else
        return false;
    return true;
}

int app_mode_read_config(const char *path, app_mode_t *mode)
{
    char line[128];
    FILE *file;

    if (!path || !mode)
        return -1;
    file = fopen(path, "r");
    if (!file)
        return 0;

    while (fgets(line, sizeof(line), file)) {
        char *key = line;
        char *value;
        char *end;

        while (isspace((unsigned char)*key))
            ++key;
        if (*key == '#' || *key == '\0')
            continue;
        value = strchr(key, '=');
        if (!value)
            continue;
        *value++ = '\0';
        end = key + strlen(key);
        while (end > key && isspace((unsigned char)end[-1]))
            *--end = '\0';
        while (isspace((unsigned char)*value))
            ++value;
        end = value + strlen(value);
        while (end > value && isspace((unsigned char)end[-1]))
            *--end = '\0';
        if (strcmp(key, "ui.mode") == 0) {
            int result = app_mode_parse(value, mode) ? 1 : -1;
            fclose(file);
            return result;
        }
    }

    fclose(file);
    return 0;
}

app_mode_t app_mode_detect(int width, int height, bool desktop_session,
                           bool pointer_device)
{
    const bool desktop_geometry = width >= 800 && height >= 600;
    return desktop_geometry && desktop_session && pointer_device
               ? APP_MODE_DESKTOP
               : APP_MODE_PHONE;
}

