# LILYGO UI Application Template

This is a complete, independent starter project for applications built with
LILYGO UI AppKit. The checked-in example is a small responsive counter named
`Template`; it demonstrates application identity, AppKit lifecycle, a
testable C++ MVVM structure, reactive LVGL binding, responsive layout,
Launcher discovery, desktop metadata, and Debian packaging.

The project does not use Launcher source code and does not vendor AppKit. It
consumes the installed SDK only through:

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
- Git submodules initialized
- LPM installed and available as `lpm` (CMake uses it to read `lpm.toml`)
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
lpm start --no-build --foreground
```

The test suite covers the model and ViewModel independently, verifies reactive
button-to-label binding, then renders portrait and landscape snapshots under
`build/host-simulator/`.

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

The package installs the executable, Launcher manifest, 128x128 icon, desktop
file, AppStream metadata, and AppKit font license notices. New applications
intentionally do not declare legacy CM0 package migration relationships.

The application has no presentation mode option. Its LVGL layout responds to
the actual container geometry and is reapplied when that geometry changes.

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
