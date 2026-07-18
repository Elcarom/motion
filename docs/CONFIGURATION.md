# Configuration and build reference

## Configuration file

The primary user configuration is `config.toml` under the Motion configuration directory. Start from `example.toml`:

```sh
install -Dm600 example.toml ~/.config/motion/config.toml
```

Most settings hot-reload. Options explicitly documented as startup-only require a process restart. Unknown or malformed
values should produce contextual validation errors and fall back safely rather than crashing the shell.

## XDG and override paths

| Kind | Current override | Legacy fallback | Standard fallback |
|---|---|---|---|
| Config | `MOTION_CONFIG_HOME` | `NOCTALIA_CONFIG_HOME` | `$XDG_CONFIG_HOME/motion`, then `~/.config/motion` |
| State | `MOTION_STATE_HOME` | `NOCTALIA_STATE_HOME` | `$XDG_STATE_HOME/motion`, then `~/.local/state/motion` |
| Data | `MOTION_DATA_HOME` | `NOCTALIA_DATA_HOME` | `$XDG_DATA_HOME/motion`, then `~/.local/share/motion` |
| Cache | `MOTION_CACHE_HOME` | `NOCTALIA_CACHE_HOME` | `$XDG_CACHE_HOME/motion`, then `~/.cache/motion` |
| Assets | `MOTION_ASSETS_DIR` | `NOCTALIA_ASSETS_DIR` | installed/development/bundle lookup order |

Override values are base directories; Motion appends `/motion`.

## Theme

Typical dynamic wallpaper configuration:

```toml
[theme]
mode = "auto"
source = "wallpaper"
wallpaper_scheme = "m3-expressive"
pure_black_dark = false
```

Supported modes are `dark`, `light`, and `auto`. The default wallpaper scheme is `m3-expressive`. A deterministic
fallback is used for invalid scheme names or unavailable extraction results. Built-in fixed palettes and community/user
template flows remain available.

## Accessibility

```toml
[accessibility]
ui_scale = 1.25
high_contrast = false
```

Reduced-motion state is managed through the shell's animation settings/state. Validate all visible surfaces after
changing scale because output fractional scale and UI scale are independent multipliers.

## Privacy and offline mode

```toml
[shell]
offline_mode = true
```

Motion never sends telemetry. `telemetry_enabled` is a legacy compatibility key and has no effect.

Offline mode blocks requests made through Motion's HTTP client. It does not block external commands, compositor IPC,
D-Bus services, plugins that implement their own networking, or applications launched by the user.

## IPC

Examples:

```sh
motion msg settings-open
motion msg launcher-open
motion theme --help
motion --version
```

The installed `noctalia` command is a compatibility alias, but scripts should migrate to `motion`.

## Hooks

Motion exports `MOTION_*` variables for hook context and, for compatibility, matching `NOCTALIA_*` aliases. Variables
are applied only for the child operation and the parent environment is restored after the hook is scheduled/executed.
Quote values in scripts and avoid evaluating hook variables as shell source.

## Build dependencies

### Arch Linux

```sh
sudo pacman -S --needed base-devel git meson ninja just pkgconf \
  wayland wayland-protocols libglvnd freetype2 fontconfig cairo pango harfbuzz \
  libxkbcommon glib2 sdbus-cpp libpipewire wireplumber polkit pam curl libwebp \
  librsvg libqalculate libxml2 md4c tomlplusplus nlohmann-json stb jemalloc
```

### Debian / Ubuntu

```sh
sudo apt install build-essential git meson ninja-build just pkg-config \
  libwayland-dev wayland-protocols libegl-dev libgles-dev libfreetype-dev \
  libfontconfig-dev libcairo2-dev libpango1.0-dev libharfbuzz-dev libxkbcommon-dev \
  libglib2.0-dev libsdbus-c++-dev libpipewire-0.3-dev libwireplumber-0.5-dev \
  libpam0g-dev libpolkit-agent-1-dev libpolkit-gobject-1-dev libcurl4-openssl-dev \
  libwebp-dev librsvg2-dev libqalculate-dev libxml2-dev libmd4c-dev \
  libtomlplusplus-dev nlohmann-json3-dev libstb-dev libjemalloc-dev
```

Package availability and names vary by release. A C++23 compiler is required.

### Fedora

```sh
sudo dnf install @development-tools git meson ninja-build just pkgconf-pkg-config \
  wayland-devel wayland-protocols-devel libEGL-devel mesa-libGLES-devel \
  freetype-devel fontconfig-devel cairo-devel pango-devel harfbuzz-devel \
  libxkbcommon-devel glib2-devel sdbus-cpp-devel pipewire-devel wireplumber-devel \
  pam-devel polkit-devel libcurl-devel libwebp-devel librsvg2-devel \
  libqalculate-devel libxml2-devel md4c-devel tomlplusplus-devel json-devel \
  stb_image_resize2-devel stb_image_write-devel jemalloc-devel
```

### openSUSE

Use the corresponding `-devel` packages for the dependency list in the README. `Mesa-libEGL-devel`,
`Mesa-libGLESv2-devel`, `sdbus-cpp-devel`, `pipewire-devel`, `wireplumber-devel`, `polkit-devel`, `librsvg-devel`,
`libqalculate-devel`, `libxml2-devel`, `md4c-devel`, `tomlplusplus-devel`, `nlohmann_json-devel`, and `stb-devel` are
commonly required.

## Meson options

- `-Dtests=auto|enabled|disabled`
- `-Djemalloc=auto|enabled|disabled`
- `-Db_sanitize=address,undefined`
- standard Meson build type, prefix, LTO, and warning options

Release builds add native CPU tuning in Meson. Distribution packaging removes `-march=native`/`-mtune=native`
for reproducibility.

## Runtime optional tools/services

- PipeWire daemon and WirePlumber are required for audio integration.
- `upower` enables richer battery/power devices.
- `ddcutil` enables external monitor brightness.
- `wtype` can support clipboard auto-paste fallbacks.
- NetworkManager, BlueZ, power-profiles-daemon, logind, polkit, PAM, and compositor IPC enable their corresponding
  surfaces.

Unavailable optional integrations should not break unrelated shell functionality.
