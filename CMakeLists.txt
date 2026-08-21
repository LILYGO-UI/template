cmake_minimum_required(VERSION 3.21)

project(lilygo_ui_app_template VERSION 0.1.0 LANGUAGES C CXX)

include(GNUInstallDirs)
include(CTest)
include(cmake/AppIdentity.cmake)
find_package(LilyGoUI CONFIG REQUIRED)

configure_file(
    cmake/app_identity.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/generated/app_identity.h
    @ONLY)

add_library(lilygo_ui_template_core STATIC
    src/app_model.c
    src/app_mode.c)
target_include_directories(lilygo_ui_template_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_BINARY_DIR}/generated)
target_compile_options(lilygo_ui_template_core PRIVATE
    -Wall -Wextra -Wpedantic)

add_library(lilygo_ui_template_ui STATIC src/app.c)
target_include_directories(lilygo_ui_template_ui PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_BINARY_DIR}/generated)
target_compile_options(lilygo_ui_template_ui PRIVATE
    -Wall -Wextra -Wpedantic)
target_link_libraries(lilygo_ui_template_ui PUBLIC
    lilygo_ui_template_core
    LilyGoUI::Runtime)

add_executable(lilygo_ui_template src/main.c)
target_compile_options(lilygo_ui_template PRIVATE
    -Wall -Wextra -Wpedantic)
target_link_libraries(lilygo_ui_template PRIVATE lilygo_ui_template_ui)
set_target_properties(lilygo_ui_template PROPERTIES
    OUTPUT_NAME "${LILYGO_UI_EXECUTABLE_NAME}")

configure_file(data/launcher.json.in
    ${CMAKE_CURRENT_BINARY_DIR}/${LILYGO_UI_APP_ID}.json @ONLY)
configure_file(data/application.desktop.in
    ${CMAKE_CURRENT_BINARY_DIR}/${LILYGO_UI_APP_ID}.desktop @ONLY)
configure_file(data/application.metainfo.xml.in
    ${CMAKE_CURRENT_BINARY_DIR}/${LILYGO_UI_APP_ID}.metainfo.xml @ONLY)

install(TARGETS lilygo_ui_template
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    COMPONENT app)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${LILYGO_UI_APP_ID}.json
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/launcher/apps
    COMPONENT app)
lilygo_ui_install_launcher_icon("${LILYGO_UI_APP_ID}" assets/app-icon.png)
install(FILES assets/app-icon.svg
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/scalable/apps
    RENAME ${LILYGO_UI_APP_ID}.svg
    COMPONENT app)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${LILYGO_UI_APP_ID}.desktop
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/applications
    COMPONENT app)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${LILYGO_UI_APP_ID}.metainfo.xml
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/metainfo
    COMPONENT app)
install(FILES data/application.conf
    DESTINATION ${CMAKE_INSTALL_FULL_SYSCONFDIR}/lilygo/ui
    RENAME ${LILYGO_UI_COMPONENT_SLUG}.conf
    COMPONENT app)
lilygo_ui_install_font_licenses(
    "${CMAKE_INSTALL_DATAROOTDIR}/doc/${LILYGO_UI_PACKAGE_NAME}"
    app)

if(BUILD_TESTING AND NOT CMAKE_CROSSCOMPILING)
    add_executable(template-core-tests tests/test_core.c)
    target_compile_options(template-core-tests PRIVATE
        -Wall -Wextra -Wpedantic -UNDEBUG)
    target_link_libraries(template-core-tests PRIVATE
        lilygo_ui_template_core)
    add_test(NAME template-core COMMAND template-core-tests)

    add_executable(template-render-ui tests/render_ui.c)
    target_compile_options(template-render-ui PRIVATE
        -Wall -Wextra -Wpedantic -UNDEBUG)
    target_link_libraries(template-render-ui PRIVATE
        lilygo_ui_template_ui)
    foreach(viewport IN ITEMS compact phone landscape desktop)
        if(viewport STREQUAL "compact")
            set(_width 320)
            set(_height 568)
        elseif(viewport STREQUAL "phone")
            set(_width 568)
            set(_height 1232)
        elseif(viewport STREQUAL "landscape")
            set(_width 1232)
            set(_height 568)
        else()
            set(_width 1024)
            set(_height 768)
        endif()
        add_test(NAME template-render-${viewport} COMMAND template-render-ui
            ${_width} ${_height}
            ${CMAKE_CURRENT_BINARY_DIR}/template-${viewport}.ppm)
    endforeach()
endif()

# New applications have no legacy CM0 package to replace. Add Debian migration
# fields only when this project supersedes an actually published package.
set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_NAME "${LILYGO_UI_PACKAGE_NAME}")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_CONTACT "LILYGO")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${LILYGO_UI_APP_DESCRIPTION}")
set(CPACK_DEB_COMPONENT_INSTALL ON)
set(CPACK_SET_DESTDIR ON)
set(CPACK_COMPONENTS_ALL app)
set(CPACK_DEBIAN_APP_PACKAGE_NAME "${LILYGO_UI_PACKAGE_NAME}")
set(CPACK_DEBIAN_APP_FILE_NAME DEB-DEFAULT)
set(CPACK_DEBIAN_APP_PACKAGE_DEPENDS "libc6, libstdc++6, libdrm2")
set(CPACK_DEBIAN_APP_PACKAGE_SECTION "utils")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS OFF)
if(CMAKE_CROSSCOMPILING AND CMAKE_SYSTEM_PROCESSOR MATCHES "^(aarch64|arm64)$")
    set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "arm64")
endif()
include(CPack)
