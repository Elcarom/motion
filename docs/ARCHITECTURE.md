# Motion architecture

## Overview

Motion is a single native C++23 process built with Meson. It connects directly to Wayland, renders with OpenGL ES,
uses Cairo/Pango/HarfBuzz for text and vector content, integrates with D-Bus and PipeWire, and exposes a local Unix
socket IPC interface. The source archive contains approximately 266,000 C/C++ lines across the application, rendering,
services, compositor adapters, shell surfaces, reusable controls, plugins, configuration, and tests.

## Major modules

| Module | Responsibility |
|---|---|
| `src/main.cpp` | CLI parsing, migration, process mode, application startup |
| `src/app` | Bootstrap, dependency ownership, event loop, startup phases, IPC command dispatch |
| `src/wayland` | Connection, outputs, seats, surfaces, clipboard, toplevels, protocol wrappers |
| `src/render` | GLES backend, scene graph, text, textures, images, animation, frame lifecycle |
| `src/shell` | Bars, panels, launcher, control center, dock, lock screen, notifications, OSDs, settings, widgets |
| `src/ui` | Reusable controls, dialogs, palette roles, style and design tokens |
| `src/config` | Typed schema, TOML parsing, validation, migrations, hot reload, overrides, persistent state |
| `src/theme` | Wallpaper-derived Material 3 Expressive palettes, templates, color generation |
| `src/ipc` | Unix socket service/client, command parser, single-instance messaging |
| `src/dbus` | Network, Bluetooth, power, tray, notifications, MPRIS, polkit, logind, accounts, UPower |
| `src/pipewire` | Audio devices, playback, capture, volume, spectrum and privacy state |
| `src/compositors` | Detection and compositor-specific workspace/window/output/action adapters |
| `src/scripting` | Luau plugin runtime, manifests, source management, bindings, extension lifecycle |
| `src/system` | Desktop entries, location, weather, brightness, hardware and process services |
| `src/calendar` | CalDAV, Google OAuth compatibility client, iCalendar parsing and event cache |
| `src/core` / `src/util` | Logging, timers, filesystem, processes, runtime paths, shared algorithms |

## Runtime flow

1. `main` parses flags and handles CLI-only operations.
2. `legacy::migrateNoctaliaProfile()` copies a legacy profile only when Motion destinations are absent.
3. Single-instance state and IPC paths are resolved through `core/runtime_path.h`.
4. `Application` owns long-lived services and executes ordered startup phases.
5. Configuration and state are loaded before user-visible shell surfaces are created.
6. Wayland globals, outputs, seats, compositor capabilities, EGL/GLES, text, D-Bus, PipeWire, and optional services are
   initialized.
7. Shell managers create output-specific layer/session-lock surfaces and reusable scene nodes.
8. The event loop multiplexes Wayland, D-Bus, PipeWire, timers, inotify, IPC, and service file descriptors.
9. Configuration, palette, output, and service signals update models and invalidate only affected scene content.
10. Shutdown tears down surfaces and services in ownership order.

The application should degrade when optional integrations are absent. Features that are startup-critical, such as the
required Wayland display and core render stack, may fail startup with actionable logs.

## Rendering flow

The scene graph under `src/render/scene` owns layout and draw nodes. Surface code builds reusable UI controls and model
bindings; renderer code performs frame scheduling, clipping, shaders, textures, text, and image operations. Wayland
surface scale and output geometry are translated into logical layout and buffer dimensions.

Theme changes update the global semantic palette and emit `paletteChanged()`. Controls subscribe and reapply role-based
colors. Palette interpolation supports smooth theme transitions. Reduced-motion state can collapse transitions to the
same deterministic end state.

Important rendering invariants:

- no blocking network or filesystem work in a frame callback;
- no feature-specific GL ownership outside the render abstraction;
- output-local scale and geometry are authoritative;
- asynchronous image results must respect object lifetime before applying changes;
- scene nodes must remain valid through deferred callbacks and event dispatch.

## Configuration flow

The typed schema in `src/config/schema` maps TOML keys into `config_types.h`. Parsing and validation normalize values,
preserve compatible keys, and reject malformed input without crashing the shell. Configuration can be hot-reloaded;
startup-only settings are documented.

Persistent user-editable configuration belongs in the configuration directory. Runtime state and GUI overrides belong
in the state directory. Re-fetchable/generated data belongs in the cache directory. Plugin sources and durable plugin
data use state/data paths as documented by their helpers.

Profile precedence:

1. current `MOTION_*_HOME` override;
2. legacy `NOCTALIA_*_HOME` override for compatibility;
3. matching XDG base directory;
4. home-directory fallback;
5. uid-scoped cache/runtime fallback only when no home is available.

## Theme flow

The active wallpaper is the sole theme-color source. Wallpaper extraction selects a source color, generates Material 3
Expressive light and dark token maps, caches the result, and resolves the runtime palette plus richer template tokens.
The bundled Motion wallpaper is the deterministic fallback when the configured wallpaper cannot be loaded.

Built-in/user/community templates can render generated roles to external application configuration. Hooks are executed
through the process helper; Motion and legacy hook environment aliases are injected temporarily and prior environment
values are restored afterward.

## IPC

The local IPC service uses a Unix domain socket in a validated `XDG_RUNTIME_DIR`; otherwise it uses a uid-scoped name
under `/tmp`. The Wayland display component is sanitized and length-limited. Before binding, the service uses `lstat`
and refuses to replace a path that is not an owned Unix socket. The socket is created under umask `0077` and forced to
mode `0600`. The single-instance lock uses `O_NOFOLLOW`.

IPC is local-user control, not a remote authorization boundary. Commands must validate arguments and avoid passing
untrusted strings through a shell unless the feature explicitly represents a user-configured command.

## Plugins

Plugins are Luau programs loaded from local paths or configured Git sources. The plugin system validates manifests,
materializes source trees, exposes controlled bindings, and integrates widgets/providers/services with the host.

Plugin code executes with the user's desktop-session authority. Source trust is therefore part of the security model.
Do not silently enable remote plugins, and do not treat manifest validation as a sandbox.

## Service lifecycle and ownership

`Application` is the composition root. Long-lived services should have one clear owner. Callbacks into UI or services
must be disconnected during teardown or guarded by lifetime tokens/weak ownership. Deferred work should not capture raw
pointers unless the owner guarantees the callback cannot outlive the target.

Avoid hidden global mutable state. Existing global palette/style signals are narrow UI infrastructure exceptions.

## Extension points

- New shell surfaces under `src/shell` using existing surface and panel managers.
- Reusable controls under `src/ui/controls`.
- Compositor adapters under `src/compositors` behind capability interfaces.
- Launcher providers and desktop/bar widgets through factories and plugin bindings.
- Theme template tokens under `src/theme`.
- D-Bus/system integrations as optional services with explicit lifecycle and fallback behavior.
- IPC commands with typed validation and tests.

## Build and packaging

Meson is authoritative. The Guix definition wraps the Meson build. Runtime assets are installed under
`share/motion/assets`, with portable resolver fallbacks for development and bundles. Release archives must be created
from a clean source tree and exclude build directories, caches, editor state, and secrets.
