# Troubleshooting

## Motion cannot find assets

Symptoms include missing fonts, icons, translations, shaders, or immediate startup failure.

- Do not copy only the binary.
- Verify the installed tree contains `share/motion/assets`.
- For a portable bundle, place `assets/` beside `motion`, or `share/motion/assets` beside `bin/motion`.
- Set `MOTION_ASSETS_DIR` to an absolute asset tree for development diagnostics.

## Another instance is already running

Use:

```sh
motion msg settings-open
```

Inspect the socket/lock location in the logs. A valid login session should provide a private `XDG_RUNTIME_DIR`. Motion
will not replace an unexpected non-socket or another user's path. Stop stale Motion processes before removing a
confirmed stale uid-owned socket.

## IPC fails under `/tmp`

This usually means `XDG_RUNTIME_DIR` is missing, not absolute, not owned by the user, or writable by group/others. Fix the
login/session setup rather than forcing a shared runtime directory. The `/tmp` fallback includes the uid and creates a
mode-0600 socket.

## Configuration does not load

```sh
motion --version
python3 - <<'PY'
import tomllib
with open("/home/USER/.config/motion/config.toml", "rb") as f:
    tomllib.load(f)
print("TOML syntax is valid")
PY
```

Replace the path with the actual configuration file. Move the file aside to test clean defaults. Compare against
`example.toml`. Invalid values should be logged with schema context.

## Migration warning

See `docs/MIGRATION.md`. Check directory ownership, free space, stale `.motion-migration-*` staging directories, and
whether a destination already exists. The legacy source is never modified by the automatic migration.

## Black or corrupted rendering

- Confirm the compositor supports required Wayland protocols.
- Update the GPU driver and Mesa stack.
- Try `shell.shared_gl_context = false` for drivers with shared-context issues.
- Test one monitor and integer scale before reintroducing mixed/fractional scaling.
- Capture logs with render/debug output enabled; do not include private notification or clipboard content.

## No audio controls

Verify PipeWire and WirePlumber are running and the development/runtime versions match. Check:

```sh
systemctl --user status pipewire wireplumber
wpctl status
```

## Missing network, Bluetooth, battery, or brightness controls

These surfaces depend on system services and hardware capabilities. Verify NetworkManager, BlueZ, UPower, power
profiles, and `ddcutil` independently. Devices without a battery, Bluetooth adapter, or controllable brightness should
show a graceful unavailable/empty state.

## Dynamic color does not update

- Verify the wallpaper path is readable.
- Remove only the relevant generated cache under `~/.cache/motion` after stopping Motion.
- Re-test with a local raster image; remote or unsupported formats can fail before extraction.
- Check logs for fallback to the bundled wallpaper's M3 Expressive palette.

## High CPU or GPU use

- Disable decorative visualizers and wallpaper transitions temporarily.
- Check for rapidly changing script widgets or hooks.
- Look for repeated service errors causing retry loops.
- Compare reduced-motion mode.
- Profile a release build; debug/sanitizer builds are not representative.

Suggested tools include `perf`, `sysprof`, compositor frame timing, and driver-specific GPU tools. Record idle CPU,
memory, frame pacing, monitor count, scale factors, and enabled widgets.

## Plugin problems

Disable the plugin and restart. Review the source, manifest, and logs. Plugins execute with user privileges and are not a
security sandbox. Remove only the plugin's materialized/cache directory when recovering; preserve persistent plugin data
unless intentionally resetting it.

## Lock-screen or authentication failure

Do not repeatedly test power/session actions on an unsaved session. Validate the PAM service configuration and polkit
agent separately. Keep an alternate TTY or remote session available during development. Graphical lock and session
actions are part of the manual release checklist.
