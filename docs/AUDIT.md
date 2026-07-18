# Motion transformation audit

## Scope

This audit covers the supplied Noctalia 5.0.0-beta.3 archive and the transformation into Motion 0.1.0. The
review included repository structure, product naming, build metadata, configuration and migration, UI/theme architecture,
filesystem and IPC paths, privacy/network behavior, dependencies, tests, packaging, CI, documentation, and assets.

The archive contained 1,421 regular files, approximately 266,000 C/C++ lines, and no `.git` directory. The archive
commit marker was `6b3632096231ecde16780937ef7ec26e8a1de388`.

## Baseline condition

The source is a native C++23 Wayland/OpenGL ES shell using Meson/Ninja, Nix, and Guix. It contains 577 C++ source files,
664 headers, a large unit/integration test suite, vendored protocol/dependency sources, and compositor-specific adapters.

A native baseline configure/build could not be started in the transformation container because Meson, Just, Nix,
clang-format, clang-tidy, and most required Wayland/graphics/system development packages were absent. GCC/G++ 14.2,
Ninja 1.12, CMake 3.31, Python 3.13, and Git 2.47 were available. See `artifacts/BASELINE.md`.

## Architecture findings

- The composition root and event loop are centralized under `src/app`; service ownership is generally explicit.
- Rendering is a custom GLES scene graph with shared controls and palette signals rather than toolkit widgets.
- Shell surfaces are split by feature under `src/shell`; shared controls live under `src/ui`.
- Configuration uses a typed TOML schema, migration/validation layers, hot reload, and a separate persistent state store.
- Compositor behavior is isolated behind adapters and capability paths.
- Plugins run through a Luau host and are extensions with user authority, not a security sandbox.
- The theme pipeline already generated broad Material token maps, but runtime layout/style constants were less
  centralized. Motion adds a canonical token layer and routes the established `Style` facade through it.

## Correctness findings and fixes

- Added parsing, serialization, CLI, template, generation, default, and regression-test coverage for `m3-expressive`.
- Corrected an accidental scheme serialization mismatch and standardized unknown-scheme fallback.
- Added idempotent profile migration with staged copies and atomic destination rename.
- Hook environment injection now restores previous values rather than unconditionally deleting them.
- Legacy and current hook variable names are emitted together for compatibility.
- Cache paths for weather, location, calendar, thumbnails, notification icons, media art, community data, screenshots,
  and clipboard fallback now share one XDG/uid-scoped path resolver.
- Temporary virtual-keyboard keymaps use a trusted runtime directory and uid-scoped template before unlinking.

## Security and privacy findings and fixes

### Fixed

- IPC and lock filenames previously depended directly on runtime environment values. Motion validates that
  `XDG_RUNTIME_DIR` is absolute, uid-owned, and not group/world writable; sanitizes and length-limits the display name;
  and uses uid-scoped `/tmp` names otherwise.
- IPC startup now checks an existing path with `lstat`, refuses to replace unexpected/non-owned paths, creates the
  socket under umask `0077`, and enforces mode `0600`. Failed startup no longer unlinks an unrelated path during
  service destruction; socket cleanup occurs only after Motion successfully owns the listening socket.
- Single-instance lock creation uses `O_NOFOLLOW`.
- Shared fixed `/tmp/motion-*` cache directories were replaced with XDG cache paths and uid-scoped fallbacks.
- Legacy migration copies symlinks without following targets, stages beside the destination, removes failed staging
  trees, and never edits the source profile.
- Inherited telemetry is permanently disabled. The old key remains parseable only to avoid breaking configuration.
- No telemetry, analytics, ads, tracking, or remote logging were introduced.

### Inherited trust boundaries

- Plugins and user-configured commands execute with user privileges and require source review.
- Optional network features use libcurl and include inherited upstream Noctalia endpoints for community catalogs,
  geolocation/public IP, and Google OAuth compatibility. These are not Motion telemetry endpoints.
- Clipboard, notification, and calendar state can contain private data and must remain user-private.

## Performance findings

- The architecture is primarily event-driven through Wayland, D-Bus, PipeWire, timers, inotify, and file descriptors.
- Palette generation and image work already use asynchronous/cached paths; Motion keeps the expressive scheme in that
  pipeline rather than recomputing per frame.
- Central tokens avoid per-feature duplicated style resolution.
- Cache consolidation avoids repeated ad-hoc path discovery and prevents multiple shared `/tmp` namespaces.
- No speculative micro-optimization or risky dependency upgrade was performed without a runnable native profiler.

Graphical startup time, idle CPU/GPU, memory fragmentation, frame pacing, and multi-output duplication require a real
Wayland/GPU session and remain in the manual validation checklist.

## Accessibility findings

- The codebase has reusable focusable controls, keyboard handlers, focus-ring roles, UI scaling, high-contrast state,
  and reduced-motion infrastructure.
- Motion centralizes focus width, target sizes, type scale, state layers, and motion durations.
- Compact pointer controls use a 32-pixel minimum role; touch-capable surfaces can use the 44-pixel role.
- Documentation now requires Enter/Space activation, Escape dismissal, arrow navigation, visible focus, non-color state,
  text-scale resilience, and reduced-motion end-state correctness.

No formal accessibility compliance is claimed. Screen-reader semantics, touch hardware, actual color contrast over
translucent surfaces, and complete keyboard traversal require graphical/manual validation.

## Maintainability findings and fixes

- Product-owned namespace, executable, package, assets, desktop file, paths, and documentation were renamed to Motion.
- Compatibility references are allow-listed and verified by `tools/branding_audit.py` instead of using blind replacement.
- `src/ui/design_tokens.h` is the canonical non-color design-token source; existing `Style` names are compatibility
  aliases so the full UI can migrate without duplicate component forks.
- New filesystem helpers centralize current and legacy XDG overrides.
- Added architecture, design-system, migration, accessibility, configuration, troubleshooting, validation, security,
  contribution, code-of-conduct, upstream, notice, changelog, and release documentation.
- Removed publication workflows, donation settings, and web links tied to a nonexistent Motion organization.

## Dependency findings

- The repository uses a broad native Linux desktop stack and several vendored libraries. Existing versions and lockfiles
  were preserved because full native compatibility could not be validated in this container.
- No new runtime dependency was added for Motion's design system, migration, security hardening, or validation scripts.
- GitHub Actions dependency updates are configured through Dependabot.
- Nix/Guix packaging preserves source-build behavior and removes native CPU tuning where reproducibility requires it.

A current vulnerability database scan was not available locally. Release maintainers should run distribution package,
Nix, and GitHub dependency/security scanning in the publication environment.

## Build and release findings

- Meson remains authoritative and installs `motion`, assets, desktop metadata, icon, and a `noctalia` compatibility
  symlink.
- Nix outputs and modules use `motion`; `programs.noctalia` is an alias.
- Guix module/package names use Motion and no longer advertise an unconfigured fork homepage.
- CI covers repository checks, branding audit, shell syntax, clang-format, debug/release native builds, tests,
  clang-tidy, staged installation, ASan/UBSan, and Nix.
- Release CI creates a reproducible source archive, a staged Linux rootfs bundle, and SHA-256 files.
- `tools/package_source.py` creates a deterministic source archive with sorted entries, normalized ownership, modes, and
  timestamps, a fixed top-level directory, and exclusions for VCS/build/cache residue.

## Verification evidence

Local checks completed in the transformation environment are recorded in `artifacts/VALIDATION_REPORT.md` and the
machine-readable `artifacts/validation-report.json`. Full native/graphical limitations are explicit rather than reported
as passes.

## Commands executed

Representative commands:

```sh
python3 tools/validate_repository.py
python3 tools/branding_audit.py
python3 tools/i18n-check.py
find tools tests -type f -name '*.sh' -print0 | xargs -0 -r -n1 bash -n

g++ -std=c++23 -Wall -Wextra -Wpedantic -Werror \
  -Wno-missing-field-initializers -Isrc -pthread \
  tests/legacy_migration_test.cpp src/core/legacy_migration.cpp -o /tmp/motion-legacy-migration-test
/tmp/motion-legacy-migration-test

g++ -std=c++23 -Wall -Wextra -Wpedantic -Werror \
  -Wno-missing-field-initializers -Isrc -pthread \
  tests/hook_manager_test.cpp src/hooks/hook_manager.cpp src/core/log.cpp -o /tmp/motion-hook-manager-test
/tmp/motion-hook-manager-test

g++ -std=c++23 -Wall -Wextra -Wpedantic -Werror \
  -Wno-missing-field-initializers -Isrc -pthread \
  tests/ipc_service_test.cpp src/ipc/ipc_service.cpp src/core/log.cpp -o /tmp/motion-ipc-service-test
/tmp/motion-ipc-service-test

g++ -std=c++23 -Wall -Wextra -Wpedantic -Werror \
  -Wno-missing-field-initializers -Isrc -pthread \
  tests/file_utils_paths_test.cpp -o /tmp/motion-file-utils-paths-test
/tmp/motion-file-utils-paths-test

g++ -std=c++23 -Wall -Wextra -Wpedantic -Werror \
  -Wno-missing-field-initializers -Isrc -pthread \
  tests/runtime_path_test.cpp -o /tmp/motion-runtime-path-test
/tmp/motion-runtime-path-test
```

## Remaining limitations

1. **Native build/test/lint not run locally.** Meson and required native development dependencies are absent. Practical
   effect: complete compile/link compatibility must be confirmed by CI. Corrective command: `just test debug
   --print-errorlogs && just lint debug`. CI coverage: yes.
2. **Graphical shell not launched.** No Wayland compositor/GPU/session services are available. Practical effect: visual,
   animation, input, lock, service, and multi-monitor behavior is not claimed as runtime-verified. Corrective procedure:
   `docs/VALIDATION.md`. CI coverage: build/headless checks only; manual compositor matrix required.
3. **Sanitizers and performance profilers not run locally.** Practical effect: runtime-only lifetime, leak, and frame
   issues may remain. Corrective commands: `just test asan --print-errorlogs`, plus the profiling checklist. CI coverage:
   sanitizers yes; GPU/performance manual.
4. **Formal accessibility testing not performed.** Practical effect: screen-reader semantics and complete keyboard/touch
   coverage are not certified. Corrective procedure: `docs/ACCESSIBILITY.md` release checklist. CI coverage: logic/build
   only; manual assistive-technology testing required.
5. **Inherited service endpoints remain.** Practical effect: optional community/geolocation/calendar features depend on
   upstream availability and policy. Corrective action: configure fork-owned compatible services and replace only the
   documented constants/translations. CI coverage: no live network tests by design.
