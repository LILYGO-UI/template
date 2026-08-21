# LILYGO UI Application Template

This is a complete, independent starter project for applications built with
LILYGO UI AppKit. The checked-in example is a small responsive counter named
`Template`; it demonstrates application identity, AppKit lifecycle, a
testable state model, LVGL event binding, phone/desktop mode resolution,
Launcher discovery, desktop metadata, and Debian packaging.

The project does not use Launcher source code and does not vendor AppKit. It
consumes the installed SDK only through:

```cmake
find_package(LilyGoUI CONFIG REQUIRED)
```

## Create an application

1. Copy this repository into a repository named after the final Debian
   package, for example `lilygo-ui-file-manager`.
2. Edit `cmake/AppIdentity.cmake`. Use a lowercase kebab-case component slug
   and a PascalCase component name, such as `file-manager` and `FileManager`.
3. Replace `assets/app-icon.svg` and regenerate `assets/app-icon.png` at
   128x128 pixels.
4. Replace the example model and UI under `src/`, then update the tests.
5. Choose the real project license and update `project_license` in
   `data/application.metainfo.xml.in`.

The identity file derives all stable public names:

```text
Package/executable: lilygo-ui-<component>
Application ID:     cc.lilygo.ui.<Component>
Desktop/AppStream:  cc.lilygo.ui.<Component>
```

Do not publish with the default `template` identity. Published package names
and application IDs are stable interfaces.

## Requirements

- CMake 3.21 or newer
- A C and C++ compiler
- `pkg-config`
- SDL2 for host simulation
- Git submodules initialized
- An AArch64 cross compiler for device builds

AppKit is pinned as the `third_party/cm0-appkit` Git submodule. Clone with
submodules, or initialize it in an existing checkout:

```sh
git clone --recurse-submodules https://github.com/LILYGO-UI/template.git
git submodule update --init --recursive
```

## Host simulator

```sh
git submodule update --init --recursive
cmake --preset host-simulator
cmake --build --preset host-simulator --parallel
ctest --preset host-simulator
./build/host-simulator/lilygo-ui-template --mode=auto
```

The test suite covers the state model and mode parser, then renders compact,
phone, landscape, and desktop snapshots under `build/host-simulator/`.

## Device build and package

The cross preset downloads the pinned
[`0.1.0` CM0 BSP](https://github.com/LILYGO-UI/CM0BspBuilder/releases/download/0.1.0/cm0_sdk.tar.gz)
on its first configuration, verifies its SHA-256 checksum, and extracts it
under `.cache/cm0-bsp/0.1.0`. Later builds reuse that cache.

```sh
git submodule update --init --recursive
cmake --preset cm0-cross
cmake --build --preset cm0-cross --parallel
cpack --config build/cm0-cross/CPackConfig.cmake -B dist
```

The package installs the executable, Launcher manifest, 128x128 and scalable
icons, desktop file, AppStream metadata, system mode default, and AppKit font
license notices. New applications intentionally do not declare legacy CM0
package migration relationships.

## Presentation mode

The executable accepts `--mode=auto`, `--mode=phone`, and `--mode=desktop`.
Resolution order is command line, per-user configuration, system
configuration, then automatic detection. Configuration uses:

```ini
ui.mode=auto
```

The per-user path is
`$XDG_CONFIG_HOME/lilygo/ui/<component>.conf` (or
`$HOME/.config/lilygo/ui/<component>.conf`); the system path is
`/etc/lilygo/ui/<component>.conf`. Automatic mode detection considers display
geometry, pointer availability, and desktop-session capability, never CPU
architecture.

## Source boundaries

- `src/app.c`: LVGL view, controller events, and AppKit descriptor
- `src/app_model.c`: application state independent of LVGL
- `src/app_mode.c`: mode parsing, configuration, and automatic detection
- `src/main.c`: application-specific options followed by AppKit runtime
- `data/`: Launcher, desktop, AppStream, and system configuration metadata
- `tests/`: model checks and headless responsive render tests

Keep backend and domain logic out of the View so it can be tested without a
display. Use AppKit's `CM0_FONT_UI_*` typography tokens for all LVGL text.
