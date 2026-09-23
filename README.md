# LILYGO UI Application Template

This is a complete, independent starter project for applications built with
LILYGO UI AppKit. The checked-in example is a small responsive counter named
`Template`; it demonstrates application identity, AppKit lifecycle, a
testable C++ MVVM structure, reactive LVGL binding, responsive layout,
Launcher discovery, desktop metadata, and Debian packaging.

The project does not use Launcher source code. It consumes the pinned
AppKit source SDK only through:

```cmake
find_package(LilyGoUI CONFIG REQUIRED)
```

## Create an application

1. Copy this repository into a repository named after the final Debian
   package, for example `lilygo-ui-file-manager`.
2. Edit `[project]` in `lpm.toml`. Set the package to a lowercase kebab-case
   `lilygo-ui-<component>` name and the application ID to
   `cc.lilygo.ui.<PascalCase>`, such as `lilygo-ui-file-manager` and
   `cc.lilygo.ui.FileManager`.
3. Replace `assets/app-icon.png` with a 128x128-pixel RGBA image.
4. Read and follow the [LILYGO UI development guidelines](docs/01-UI.md),
   then replace the example page under `src/pages/counter/` and its
   corresponding tests.
5. Set the real version, description, license, categories, compatibility,
   permissions, repository URLs, and preview assets in `lpm.toml`.

`lpm.toml` is the single developer-maintained source for application identity,
release metadata, build presets, and deployment defaults. CMake validates it
through LPM and derives all generated metadata and package settings from it:

```text
Package/executable: lilygo-ui-<component>
Application ID:     cc.lilygo.ui.<Component>
Desktop/AppStream:  cc.lilygo.ui.<Component>
```

Do not publish with the default `template` identity. Published package names
and application IDs are stable interfaces.

The Launcher, desktop, and AppStream files under `data/` are output templates
and must not contain application-specific values. `publish.json` is ignored by
Git because it is a temporary publishing artifact. Generate it when needed:

```sh
lpm metadata --format publish-json --output publish.json
```

The generated JSON contains only public publish metadata; `[build]` and
`[deploy]` are excluded. A future `lpm publish` will perform the same conversion
internally before upload.

## Design requirements

The [LILYGO UI development guidelines](docs/01-UI.md) define the architecture,
design, implementation, and verification requirements for derived applications.
Derived applications must follow them when adding or changing UI. In
particular, use AppKit's composite fonts, build responsive LVGL
Flex or Grid layouts, verify every supported viewport and orientation, and
include the required font license notices in distributions.

## Requirements

- CMake 3.21 or newer
- C and C++17 compilers (the application template is C++; LVGL includes C)
- `pkg-config`
- SDL2 for host simulation
- `clang-format` for automatic formatting before commits
- Git submodules initialized
- LPM installed and available as `lpm` (CMake uses it to read `lpm.toml`)
- An AArch64 cross compiler for device builds

AppKit is pinned as the `third_party/cm0-appkit` Git submodule. Initialize it
before configuring the application:

```sh
git submodule update --init --recursive
```

The presets select this source SDK with `LilyGoUI_DIR`. Its package config
builds AppKit and LVGL as static libraries and links them into each application.
Updating AppKit code requires updating the submodule revision and rebuilding
the application. A local SDK checkout can be selected explicitly:

```sh
cmake --preset host-simulator -DLilyGoUI_DIR=/path/to/appkit
```

Host builds use the SDK's font assets. Devices install `lilygo-ui-appkit-dev`,
version 0.1.0 or newer, which contains the source SDK and the shared Inter,
Source Han Sans CN, Font Awesome, and font licenses. Applications do not bundle
duplicate fonts. AppKit and LVGL remain statically linked into each application;
the package supplies no AppKit or LVGL shared libraries. The
`min_appkit_version` field in `lpm.toml` sets this package's minimum version.
The package also supports build environments using an installed source SDK
instead of the submodule.

Cross builds require the BSP sysroot and its system development libraries;
they do not require an AppKit SDK package in the sysroot.
When returning from a binary SDK build, use a fresh build directory so no old
imported targets or SDK search paths remain cached.

## Code formatting

C and C++ sources use the checked-in `.clang-format` configuration. After
cloning, enable the repository's pre-commit hook:

```sh
git config core.hooksPath .githooks
```

Each commit then formats and restages its staged C/C++ files while excluding
`third_party/`. If a staged file also has unstaged changes, the hook stops the
commit so those changes are not included accidentally; stage or stash the
remaining changes before committing again.

## Host simulator

```sh
cmake --preset host-simulator
cmake --build --preset host-simulator --parallel
ctest --preset host-simulator
lpm start --no-build --foreground
```

The test suite covers the model and ViewModel independently, verifies reactive
button-to-label binding, then renders portrait and landscape snapshots under
`build/host-simulator/`.

## Device build and package

Initialize the AppKit submodule, then use the cross preset.
The cross preset downloads the pinned
[`0.1.0` CM0 BSP](https://github.com/LILYGO-UI/CM0BspBuilder/releases/download/0.1.0/cm0_sdk.tar.gz)
on its first configuration, verifies its SHA-256 checksum, and extracts it
under `.cache/cm0-bsp/0.1.0`. Later builds reuse that cache.

```sh
cmake --preset cm0-cross
cmake --build --preset cm0-cross --parallel
cpack --config build/cm0-cross/CPackConfig.cmake -B dist
```

The package installs the executable, Launcher manifest, 128x128 icon, desktop
file, AppStream metadata, and AppKit font license notices. New applications
intentionally do not declare legacy CM0 package migration relationships.

The application has no presentation mode option. Its LVGL layout responds to
the actual container geometry and is reapplied when that geometry changes.

## GPU renderer builds

Device rendering is selected at application build time by the source AppKit
SDK. Use an updated SDK supporting `LILYGO_UI_RENDERER` and a target-matched
CM0 sysroot with `libgbm-dev`, `libegl-dev`, and `libgles-dev`. The original
BSP 0.1.0 lacks those GPU development dependencies. The original pinned
AppKit submodule does not support GPU renderer selection; use a newer source
SDK explicitly for this build.

Select the SDK source checkout or the directory containing the installed
source SDK's `LilyGoUIConfig.cmake`:

```sh
cmake --preset cm0-cross -B build/cm0-opengles \
  -DLilyGoUI_DIR=/path/to/appkit \
  -DCMAKE_TOOLCHAIN_FILE=/path/to/cm0-sysroot/usr/share/cm0-bsp/toolchain.cmake \
  -DLILYGO_UI_RENDERER=opengles
cmake --build build/cm0-opengles --parallel
cpack --config build/cm0-opengles/CPackConfig.cmake -B dist/opengles
```

The executable contains AppKit and LVGL code, and its Debian package declares
the selected renderer's system graphics dependencies. Keep separate build
directories for software, OpenGL ES, and experimental NanoVG. Use software
rendering for host simulator tests. Compare rendering correctness, frame time,
memory, and repeated application exit/return on the target before changing a
release renderer.

## Source boundaries

- `src/app.cpp`: AppKit lifecycle and construction of the initial page
- `src/pages/<page>/<page>_model.cpp`: page state and domain rules independent of LVGL
- `src/pages/<page>/<page>_view_model.cpp`: presentation state and commands
- `src/pages/<page>/<page>_view.cpp`: LVGL widget creation and binding only
- `src/domain/`: optional domain objects shared by multiple pages
- `src/components/`: reusable widgets and LVGL Subject RAII helpers
- `src/main.cpp`: AppKit runtime entry point
- `data/`: Launcher, desktop, and AppStream metadata
- `tests/`: model, ViewModel, binding, and headless responsive render tests

Each page lives in its own directory under `src/pages/`, with its private
Model, ViewModel, and View kept together. Keep Model code independent of LVGL,
transform it into observable presentation state in the ViewModel, and let the
View invoke ViewModel commands and bind widgets to that state. Move a domain
object to `src/domain/` only when multiple pages actually share it. The View
must not mutate Model objects directly.

For applications with multiple page levels, add `src/app_router.cpp/.hpp` for
the page enum, navigation, and back-stack rules. The lifecycle in `app.cpp`
owns the router and selects the corresponding page View; page Views must not
own global navigation state.

Use `lilygo_ui_font_get()` for all LVGL text.
