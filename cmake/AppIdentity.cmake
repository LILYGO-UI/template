# Change these values before publishing a project created from this template.
set(LILYGO_UI_COMPONENT_SLUG "template")
set(LILYGO_UI_COMPONENT_NAME "Template")
set(LILYGO_UI_APP_NAME "Template")
set(LILYGO_UI_APP_DESCRIPTION "Starter application for LILYGO UI")
set(LILYGO_UI_APP_ORDER 90)

if(NOT LILYGO_UI_COMPONENT_SLUG MATCHES "^[a-z0-9]+(-[a-z0-9]+)*$")
    message(FATAL_ERROR
        "LILYGO_UI_COMPONENT_SLUG must be lowercase kebab-case")
endif()
if(NOT LILYGO_UI_COMPONENT_NAME MATCHES "^[A-Z][A-Za-z0-9]*$")
    message(FATAL_ERROR
        "LILYGO_UI_COMPONENT_NAME must be a PascalCase identifier")
endif()

set(LILYGO_UI_PACKAGE_NAME "lilygo-ui-${LILYGO_UI_COMPONENT_SLUG}")
set(LILYGO_UI_EXECUTABLE_NAME "${LILYGO_UI_PACKAGE_NAME}")
set(LILYGO_UI_APP_ID "cc.lilygo.ui.${LILYGO_UI_COMPONENT_NAME}")

