# Validation guide

## Automated checks

Dependency-complete environment:

```sh
just configure debug
just build debug
just test debug --print-errorlogs
just lint debug
just configure release
just build release
DESTDIR="$PWD/stage" meson install --no-rebuild -C build-release
```

Formatting:

```sh
mapfile -d '' files < <(find src tests -type f \( -name '*.cpp' -o -name '*.h' \) -print0)
clang-format --dry-run -Werror "${files[@]}"
```

Sanitizers:

```sh
just configure asan
just test asan --print-errorlogs
```

Repository validation:

```sh
python3 tools/validate_repository.py
python3 tools/branding_audit.py
```

The Meson test suite includes `palette_roles`, which checks role-token round trips, generated-to-runtime mapping,
semantic foreground lookup, state-layer composition, and interpolation of the extended runtime palette.

## Clean-room build

Create a source-only archive or copy, extract it outside the working directory, then configure and build without local
caches or generated files:

```sh
tar --exclude-vcs --exclude='build-*' --exclude='result*' -czf /tmp/motion-source.tar.gz .
mkdir /tmp/motion-clean && tar -xzf /tmp/motion-source.tar.gz -C /tmp/motion-clean
cd /tmp/motion-clean
meson setup build --buildtype=release -Dtests=enabled
meson compile -C build
meson test -C build --print-errorlogs
```

## Graphical smoke matrix

Perform at least one full session on a compositor from each supported integration path used by the release. Record exact
compositor and protocol versions.

### Startup/profile cases

- [ ] no config/state/data directories;
- [ ] valid Motion profile;
- [ ] Noctalia-only profile migration;
- [ ] malformed TOML and unknown settings;
- [ ] interrupted/stale migration staging directory;
- [ ] missing optional system services;
- [ ] restart after clean shutdown and forced termination.

### Themes and accessibility

- [ ] dark, light, auto;
- [ ] Material 3 Expressive wallpaper palette and bundled-wallpaper fallback;
- [ ] wallpaper change while panels are open;
- [ ] verify all five surface-container levels update together while launcher, control center, menus, and notifications
      are visible;
- [ ] high contrast in light, dark, and automatic modes;
- [ ] UI scale 1.0/1.25/1.5/2.0;
- [ ] reduced motion;
- [ ] keyboard-only operation and focus restoration;
- [ ] long translations and RTL where supported.

### Surfaces

- [ ] bars on every edge and multiple panels;
- [ ] launcher/search, large result sets, keyboard navigation;
- [ ] control center and each quick-setting expansion;
- [ ] notification arrival, stacking, actions, reply, dismissal, history;
- [ ] volume/brightness OSDs;
- [ ] tray menus and nested D-Bus menus;
- [ ] dock, taskbar, pinned apps, drag/reorder;
- [ ] clipboard text/image history, pin, clear, auto-paste;
- [ ] wallpaper picker and dynamic color update;
- [ ] settings, dialogs, pickers, errors, empty and loading states;
- [ ] button variants, sliders/range sliders, switches, checkboxes, radios, inputs, selects, segmented controls,
      scrollbars, progress indicators, focus rings, disabled states, and destructive states;
- [ ] lock screen, authentication failure/success, idle grace;
- [ ] session actions in a disposable test session;
- [ ] desktop widgets, edit mode, persistence;
- [ ] plugin loading, invalid manifest, disabled plugin, source update;
- [ ] IPC and compatibility command alias.

### Display matrix

- [ ] small laptop display;
- [ ] standard desktop monitor;
- [ ] portrait monitor;
- [ ] ultrawide;
- [ ] two or more outputs;
- [ ] mixed resolutions and scale factors;
- [ ] fractional scaling;
- [ ] hotplug/unplug and primary-output change.

### Long-run/performance

- [ ] repeated panel/launcher open-close cycles;
- [ ] notification burst and reflow;
- [ ] wallpaper/theme switching loop;
- [ ] two-hour idle observation;
- [ ] idle CPU/GPU/memory baseline;
- [ ] startup time and search latency;
- [ ] logs contain no repeated warnings, secrets, clipboard payloads, tokens, or private notification bodies.

## Evidence

Record commands, versions, pass/fail counts, logs, screenshots, monitor topology, and genuine limitations in
`artifacts/VALIDATION_REPORT.md` and `docs/AUDIT.md`. Compilation alone is not proof of visual or interaction correctness.
