# Changelog

All notable Motion changes are documented here.

## [0.1.0] - 2026-07-18

### Fork and branding

- Created the independent Motion fork from the supplied Noctalia 5.0.0-beta.3 archive.
- Renamed the executable, package, desktop entry, application-facing identifiers, install paths, Nix/Guix outputs, UI
  labels, and documentation.
- Added original Motion vector icon and 3840×2160 wallpaper while preserving upstream attribution and license text.
- Added an installed `noctalia` command alias and intentional compatibility references.

### Material 3 Expressive-inspired system

- Added centralized spacing, shape, typography, target-size, state-layer, elevation, breakpoint, duration, and spring
  intent tokens in `src/ui/design_tokens.h`.
- Routed the established `Style` API through the new tokens so existing reusable controls and shell surfaces consume a
  common desktop density and motion scale.
- Added `m3-expressive` parsing, serialization, generation, CLI support, template-engine support, and default wallpaper
  scheme selection.
- Retained light, dark, high-contrast, scale, and reduced-motion paths from the shell architecture.

### Migration and compatibility

- Added safe first-launch copying of legacy Noctalia config, state, and data into Motion XDG paths.
- Added marker-based idempotency, sibling staging directories, atomic rename, source preservation, and symlink-safe copy.
- Added legacy `NOCTALIA_*` path, asset, hook, and glyph-prefix fallbacks.
- Added Nix aliases from `programs.noctalia` to `programs.motion`.

### Security and privacy

- Permanently disabled inherited telemetry while keeping the old key parseable.
- Hardened IPC socket and lock paths against untrusted runtime directories, traversal-like display names, symlink/path
  replacement, and permissive socket modes.
- Replaced shared fixed `/tmp` application caches with XDG cache paths and uid-scoped fallbacks.
- Preserved upstream network endpoints only for inherited optional services; no new telemetry or tracking was added.

### Correctness, maintainability, and tests

- Fixed expressive scheme serialization and standardized fallback behavior.
- Made hook environment injection restore previous values and emit both Motion and legacy aliases.
- Added regression tests for legacy profile migration, file/cache path resolution, runtime-path sanitization, hook
  compatibility/restoration, and IPC socket permissions.
- Added CI, release packaging, checksums, dependency-update configuration, issue forms, documentation, and audit reports.

### Known limitations

- Full native compilation and graphical runtime validation were not possible in the transformation container because
  Meson and the required Wayland/graphics/system development packages were unavailable. CI performs those checks on a
  dependency-complete Arch Linux environment.
- Visual review under real compositors, mixed-scale multi-monitor hardware, screen readers, touch hardware, and GPU
  profilers remains a release-gating manual task documented in `docs/VALIDATION.md`.
