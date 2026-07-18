# Baseline validation

Source archive: `motion-5.0.0-beta.3(1).zip`
Archive commit marker: `6b3632096231ecde16780937ef7ec26e8a1de388`
Archive contained 1,421 regular files and no `.git` metadata.

## Environment

- GCC/G++ 14.2.0 available.
- Ninja 1.12.1 available.
- CMake 3.31.6 available.
- Python 3.13.5 available.
- Git 2.47.3 available.
- Meson, Just, clang-format, and clang-tidy unavailable.
- Most native development dependencies are unavailable to pkg-config (Wayland, Cairo, Pango, sdbus-c++, PipeWire, WirePlumber, GLib, polkit, libqalculate, md4c, nlohmann-json, toml++, jemalloc).

## Baseline result

A native baseline configure/build/test could not be started because Meson and required development dependencies are absent. Static repository inventory and source-level validation were completed before transformation. This limitation is preserved in `docs/AUDIT.md` and covered by CI for a dependency-complete Linux runner.
