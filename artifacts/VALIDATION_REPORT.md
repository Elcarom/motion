# Motion validation report

**Product:** Motion 0.1.1  
**Source:** supplied Noctalia 5.0.0-beta.3 archive  
**Validation date:** 2026-07-18  
**Status:** completed to the maximum extent supported by the available non-graphical build environment

## Material 3 Expressive redesign addendum

The repository-wide visual-system pass added complete runtime palette roles, semantic component/state styling, updated
theme exports, and the `palette_roles` Meson test. In the Windows workspace used for this pass, Python repository,
branding, translation, TOML, source-diff, and asset/template checks were available and passed. `just`, Meson, Ninja,
G++, Clang, clang-format, clang-tidy, a WSL distribution, and a graphical Wayland session were unavailable, so the new
native test and full build remain dependency-complete CI checks. The template comparison helper was also unavailable
because its external Python `lib.renderer` module was not installed.

## Environment

| Tool | Result |
|---|---|
| G++ | 14.2.0 |
| Python | 3.13.5 |
| Ninja | 1.12.1 |
| CMake | 3.31.6 |
| Git | 2.47.3 |
| Meson | unavailable |
| Just | unavailable |
| clang-format | unavailable |
| clang-tidy | unavailable |
| Wayland/graphics/system development stack | incomplete/unavailable to `pkg-config` |

The supplied archive contained 1,421 regular files and no Git metadata. The transformed tree contains 581 `.cpp` files,
667 `.h` files, and approximately 266,460 C/C++ source/test lines. The upstream archive commit marker is
`6b3632096231ecde16780937ef7ec26e8a1de388`.

## Completed local checks

| Check | Command | Outcome |
|---|---|---|
| Repository structure, metadata, config, assets, and Meson references | `python3 tools/validate_repository.py` | **PASS — 614 checks** |
| Product naming and compatibility allow-list | `python3 tools/branding_audit.py` | **PASS — 27 files contain intentional upstream compatibility/attribution references** |
| Translation catalog | `python3 tools/i18n-check.py` | **PASS — 784 `tr` keys and 7 `trp` base keys present in 2,217 catalog entries** |
| Shell syntax | `find tools tests -type f -name '*.sh' -print0 \| xargs -0 -r -n1 bash -n` | **PASS — 5 scripts** |
| GitHub YAML parsing | Python `yaml.safe_load` over `.github/**/*.yml` and `.yaml` | **PASS — 7 files** |
| Fake fork endpoints | repository scan excluding the audit rule itself | **PASS — no `motion.dev`/`motion-dev` publication endpoints** |
| Common secret patterns | repository scan for private-key and common token signatures | **PASS — no matches** |
| Build/cache residue | directory scan for build trees, result links, Python/test caches | **PASS — no residue in delivery tree** |
| Deterministic package tool syntax | `python3 -m py_compile tools/package_source.py` | **PASS** |

## Standalone C++ regression tests

The following 21 tests were compiled directly with G++ and executed successfully using project-equivalent strict flags:

```text
-std=c++23 -O0 -Wall -Wextra -Wpedantic -Werror
-Wno-missing-field-initializers -Isrc -pthread
```

1. `legacy_migration`
2. `file_utils_paths`
3. `runtime_path`
4. `animation_manager`
5. `ipc_service`
6. `notification_filter`
7. `workspace_alert_service`
8. `battery_hook_state`
9. `audio_glyphs`
10. `string_utils`
11. `cpu_temp_sensor`
12. `i18n_language_tag`
13. `i18n_supported_languages`
14. `plugin_source_locks`
15. `image_source_log`
16. `app_identity`
17. `ical_recurrence`
18. `hook_manager`
19. `desktop_entry_launch`
20. `process`
21. `plugin_git_export`

A final focused rerun of the five new or security-sensitive tests — `legacy_migration`, `file_utils_paths`,
`runtime_path`, `hook_manager`, and `ipc_service` — passed **5/5**. The IPC test deliberately exercises refusal to
replace an unsafe regular file; the emitted warning is expected and the file remains intact after service destruction.

`triad_workspace_backend` could not be compiled standalone because the container lacks the external
`nlohmann/json.hpp` development header. This is an environment dependency limitation, not a recorded test failure.

## Packaging and clean-source checks

- `tools/package_source.py` is included for deterministic source packaging.
- The delivery process generates a normalized `motion-0.1.1` source archive outside the repository.
- The archive is extracted into a new directory and repository validation, branding audit, translation checks, shell
  syntax, and YAML parsing are rerun against that extracted tree.
- SHA-256 checksums and a repository file manifest are generated as separate delivery artifacts.

## Checks not executable locally

| Check | Why unavailable | Exact reproduction | CI coverage |
|---|---|---|---|
| Full Meson configure/build/link | Meson and native development packages absent | `just configure debug && just build debug` | Yes, debug and release |
| Complete Meson test suite | Dependency-complete build unavailable | `just test debug --print-errorlogs` | Yes |
| clang-format | `clang-format` absent | `clang-format --dry-run -Werror $(find src tests -type f \( -name '*.cpp' -o -name '*.h' \))` | Yes |
| clang-tidy/static analysis | `clang-tidy` and compile database absent | `just lint debug` | Yes |
| ASan/UBSan | Dependency-complete sanitizer build unavailable | `just configure asan && just test asan --print-errorlogs` | Yes |
| Graphical shell smoke tests | No Wayland compositor, GPU/session, D-Bus/PipeWire/polkit environment | Follow `docs/VALIDATION.md` | Manual matrix required |
| Runtime performance profiling | No graphical session or representative hardware | Follow long-run section in `docs/VALIDATION.md` | Manual |
| Formal accessibility testing | No assistive-technology graphical session | Follow `docs/ACCESSIBILITY.md` | Logic/build only; manual required |
| Live dependency vulnerability scan | No vulnerability database/scanner available | Run distribution and GitHub security scans | Dependabot plus publication environment |

## Conclusion

All validation that could be performed safely in the supplied environment passed. Full native compilation, graphical
runtime behavior, compositor integration, formal accessibility, and performance are deliberately not claimed as locally
verified. Dependency-complete CI and explicit manual matrices are included for those remaining checks.
