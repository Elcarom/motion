# Motion

<p align="center">
  <img src="assets/motion.svg" width="144" height="144" alt="Motion logo">
</p>

**Motion** is a native C++23 Wayland desktop shell derived from **Noctalia 5.0.0-beta.3**. It combines bars, launcher,
control center, notifications, dock, desktop widgets, lock screen, wallpaper management, OSDs, clipboard history,
settings, IPC, and compositor integration in one process.

Motion preserves Noctalia's MIT-licensed architecture while introducing an original product identity, a centralized
Material 3 Expressive-inspired desktop design system, expressive wallpaper color schemes, safer runtime paths, and a
non-destructive migration path for existing Noctalia profiles. Motion is an independent fork and is not affiliated with
or endorsed by Google, the Material Design team, Android, or the Noctalia maintainers.

## Highlights

- **Cohesive shell surfaces:** bars and widgets, launcher and search, control center and quick settings, notifications,
  dock, tray, clipboard, lock screen, session actions, OSDs, wallpaper picker, settings, and desktop widgets.
- **Material 3 Expressive principles:** centralized spacing, shape, typography, size, elevation, state-layer, breakpoint,
  and motion tokens; light/dark dynamic palettes; and a default `m3-expressive` wallpaper scheme.
- **Desktop-first interaction:** pointer and keyboard density, focus rings, reduced motion, fractional scaling, and
  multi-monitor-aware shell surfaces rather than mobile-sized components copied unchanged.
- **Local-first privacy:** inherited telemetry configuration remains parseable for compatibility but is ignored. Motion
  sends no telemetry, analytics, advertising identifiers, or remote logs.
- **Compatibility:** the installed `noctalia` command is an alias to `motion`; legacy `NOCTALIA_*` path and hook
  variables are accepted; Noctalia configuration, state, and data are copied safely on first launch when Motion has no
  profile of its own.
- **Deployment paths:** Meson/Ninja source builds and a Guix package definition.

## Supported environment

Motion requires Linux, Wayland, OpenGL ES/EGL, and a compositor that supports the protocols used by its enabled
surfaces. The supplied source contains integrations for Niri, Hyprland, Sway, Scroll, Labwc, Mango, Triad, dwl, KDE,
and generic `ext-workspace-v1` paths. Capability varies with compositor protocol and IPC support.

Motion is a shell, not a compositor or file manager. Window placement, tiling, display configuration, removable-media
mounting, and screen casting remain the responsibility of the compositor or dedicated applications.

## Build requirements

The build uses Meson, Ninja, `pkg-config`, and a C++23 compiler (GCC 13+ or Clang 16+). Major native dependencies are:

`wayland`, `wayland-protocols`, EGL/GLES, FreeType, Fontconfig, Cairo, Pango, HarfBuzz, libxkbcommon, GLib/GIO,
sdbus-c++, PipeWire, WirePlumber, PAM, polkit, libcurl, libwebp, librsvg, libqalculate, libxml2, md4c, toml++,
nlohmann-json, stb, and optionally jemalloc.

Representative Arch Linux setup:

```sh
sudo pacman -S --needed base-devel git meson ninja just pkgconf \
  wayland wayland-protocols libglvnd freetype2 fontconfig cairo pango harfbuzz \
  libxkbcommon glib2 sdbus-cpp libpipewire wireplumber polkit pam curl libwebp \
  librsvg libqalculate libxml2 md4c tomlplusplus nlohmann-json stb jemalloc
```

Equivalent Debian/Ubuntu, Fedora, openSUSE, and Void package names are documented in
[`docs/CONFIGURATION.md`](docs/CONFIGURATION.md#build-dependencies).

## Build and test

Using the repository recipes:

```sh
just configure debug
just build debug
just test debug --print-errorlogs
just lint debug
```

Or directly with Meson:

```sh
meson setup build-debug --buildtype=debug -Dtests=enabled
meson compile -C build-debug
meson test -C build-debug --print-errorlogs
```

Release build and staged installation:

```sh
just configure release /usr/local
just build release
DESTDIR="$PWD/stage" meson install --no-rebuild -C build-release
```

Run from the build tree with `./build-debug/motion`, or use `just run debug`.

## Install and uninstall

After a successful release build:

```sh
sudo just install release
```

The default prefix installs the binary and assets to:

```text
/usr/local/bin/motion
/usr/local/bin/noctalia -> motion
/usr/local/share/motion/assets/...
/usr/local/share/applications/dev.motion.Motion.desktop
/usr/local/share/icons/hicolor/scalable/apps/motion.svg
```

Uninstall with the same build directory:

```sh
sudo just uninstall release
```

Copying only the executable is insufficient; Motion requires its `assets/` tree. Portable layouts supported by the
resource resolver include `bundle/motion` beside `bundle/assets/`, and `bundle/bin/motion` beside
`bundle/share/motion/assets/`.

## Configuration

Copy the annotated defaults:

```sh
mkdir -p ~/.config/motion
cp example.toml ~/.config/motion/config.toml
```

Primary paths:

| Purpose | Default |
|---|---|
| Configuration | `$XDG_CONFIG_HOME/motion` or `~/.config/motion` |
| State | `$XDG_STATE_HOME/motion` or `~/.local/state/motion` |
| Data | `$XDG_DATA_HOME/motion` or `~/.local/share/motion` |
| Cache | `$XDG_CACHE_HOME/motion` or `~/.cache/motion` |
| IPC socket | trusted `$XDG_RUNTIME_DIR`, otherwise uid-scoped `/tmp` |

Current overrides are `MOTION_CONFIG_HOME`, `MOTION_STATE_HOME`, `MOTION_DATA_HOME`, `MOTION_CACHE_HOME`, and
`MOTION_ASSETS_DIR`. Legacy `NOCTALIA_*` forms remain accepted as fallback aliases.

See [`docs/CONFIGURATION.md`](docs/CONFIGURATION.md) for configuration, IPC, offline mode, dynamic color, and
troubleshooting guidance.

## Migration from Noctalia

At startup, Motion checks for legacy `noctalia` configuration, state, and data directories. A legacy directory is copied
only when the corresponding Motion destination does not exist. Copying is staged in a sibling temporary directory and
committed with an atomic rename; symlinks are copied as symlinks and the source is never modified. A marker is written to
`$XDG_STATE_HOME/motion/.migrated-from-noctalia-v1`.

The old profile is therefore the rollback copy. Full behavior and recovery steps are in
[`docs/MIGRATION.md`](docs/MIGRATION.md).

## Privacy and networking

Motion does not transmit telemetry. Optional user-facing features can still make network requests: weather and
geolocation, public-IP display, community palettes/templates, remote notification or media artwork, calendar providers,
plugin Git sources, and explicitly configured integrations. Set `shell.offline_mode = true` to block outgoing HTTP
through Motion's HTTP client. Existing inherited community/catalog and OAuth compatibility endpoints remain operated by
the upstream Noctalia service and are labeled as such in code and documentation.

## Documentation

- [`docs/ARCHITECTURE.md`](docs/ARCHITECTURE.md) — modules, runtime flow, state, IPC, plugins, and invariants
- [`docs/DESIGN_SYSTEM.md`](docs/DESIGN_SYSTEM.md) — Material 3 Expressive interpretation and contributor rules
- [`docs/MIGRATION.md`](docs/MIGRATION.md) — safe profile migration and compatibility aliases
- [`docs/ACCESSIBILITY.md`](docs/ACCESSIBILITY.md) — keyboard, focus, contrast, text scaling, and reduced motion
- [`docs/CONFIGURATION.md`](docs/CONFIGURATION.md) — build dependencies and configuration reference
- [`docs/TROUBLESHOOTING.md`](docs/TROUBLESHOOTING.md) — common startup, rendering, IPC, and service failures
- [`docs/VALIDATION.md`](docs/VALIDATION.md) — automated and graphical verification checklist
- [`docs/AUDIT.md`](docs/AUDIT.md) — transformation audit, fixes, evidence, and remaining limitations
- [`UPSTREAM.md`](UPSTREAM.md) and [`NOTICE`](NOTICE) — provenance, attribution, and licensing

## Current validation status

The repository includes dependency-complete GitHub Actions for formatting, debug and release builds, tests, linting,
source packaging, and checksums. In the transformation environment, the full native shell could not
be built or launched because Meson and most graphical development dependencies were unavailable. Pure C++ regression
tests, format-independent source checks, configuration parsing, asset validation, branding audits, and packaging checks
were run locally. Exact evidence is recorded in [`artifacts/VALIDATION_REPORT.md`](artifacts/VALIDATION_REPORT.md) and
[`docs/AUDIT.md`](docs/AUDIT.md).

## Contributing and security

Read [`CONTRIBUTING.md`](CONTRIBUTING.md), [`CODE_OF_CONDUCT.md`](CODE_OF_CONDUCT.md), and
[`SECURITY.md`](SECURITY.md). Bug reports should include the Motion version, compositor, distribution, GPU, logs, and
minimal reproduction steps. Do not include secrets, clipboard content, calendar tokens, or private notification text.

Suggested repository description:

> Native Material 3 Expressive-inspired Wayland desktop shell derived from Noctalia.

Suggested GitHub topics: `wayland`, `desktop-shell`, `linux-desktop`, `material-design`, `cpp23`, `meson`,
`launcher`, `notification-daemon`, `wayland-shell`.

## License

Motion is distributed under the MIT License inherited from Noctalia. The upstream copyright notice is preserved in
[`LICENSE`](LICENSE). Motion-specific changes do not remove or supersede upstream rights. Third-party components retain
their own licenses; see [`CREDITS.md`](CREDITS.md), [`NOTICE`](NOTICE), and the notices in `third_party/`.
