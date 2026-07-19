# Migrating from Noctalia to Motion

## Summary

Motion can bootstrap a new profile from an existing Noctalia profile without modifying the original. Migration runs at
the beginning of normal application startup, before configuration is loaded.

## Directory mapping

| Legacy source | Motion destination |
|---|---|
| `$NOCTALIA_CONFIG_HOME/noctalia` or `$XDG_CONFIG_HOME/noctalia` | `$MOTION_CONFIG_HOME/motion` or `$XDG_CONFIG_HOME/motion` |
| `$NOCTALIA_STATE_HOME/noctalia` or `$XDG_STATE_HOME/noctalia` | `$MOTION_STATE_HOME/motion` or `$XDG_STATE_HOME/motion` |
| `$NOCTALIA_DATA_HOME/noctalia` or `$XDG_DATA_HOME/noctalia` | `$MOTION_DATA_HOME/motion` or `$XDG_DATA_HOME/motion` |

When XDG bases are unset, the normal `~/.config`, `~/.local/state`, and `~/.local/share` fallbacks are used.

## Safety behavior

For each directory independently:

1. If the Motion destination already exists, migration skips it.
2. If the Noctalia source does not exist, nothing is created.
3. The source tree is copied to a unique sibling staging directory.
4. Recursive copying uses `copy_symlinks`; symlink targets are not followed into unrelated locations.
5. The staging directory is renamed atomically to the Motion destination.
6. A failure removes the staging directory and leaves the source untouched.
7. Successful migration writes `.migrated-from-noctalia-v1` in the Motion state directory.

The marker records what was copied. Destination existence, rather than the marker alone, is the primary idempotency
control, so partial legacy profiles can be handled safely.

## Compatibility aliases

Motion intentionally accepts:

- installed command alias: `noctalia` → `motion`;
- `NOCTALIA_CONFIG_HOME`, `NOCTALIA_STATE_HOME`, `NOCTALIA_DATA_HOME`, `NOCTALIA_CACHE_HOME`;
- `NOCTALIA_ASSETS_DIR`;
- legacy `NOCTALIA_*` hook variables emitted alongside `MOTION_*` variables;
- `noctalia-glyph:` icon prefix alongside `motion-glyph:`;

New configuration and automation should use Motion names.

## Configuration changes

Most schema keys remain compatible because the fork preserves the supplied shell functionality. Notable behavior:

- `shell.telemetry_enabled` is accepted but ignored; Motion never sends telemetry.
- Wallpaper palettes are always generated with Material 3 Expressive.
- New paths and generated caches use the `motion` directory name.
- Community template, Google OAuth, geolocation, and plugin defaults can still point to inherited Noctalia
  services or repositories where no fork-operated service exists.

## Recommended migration procedure

```sh
# Stop Noctalia first so its state is stable.
pkill -x noctalia || true

# Start Motion once. It copies missing profile directories automatically.
motion --daemon

# Inspect startup logs and compare the profiles.
diff -ru ~/.config/noctalia ~/.config/motion || true
```

Do not delete the Noctalia profile until Motion has been used successfully through a full session.

## Rollback

1. Stop Motion.
2. Start the original Noctalia binary with its unchanged legacy directories.
3. Remove or rename Motion's directories only when you are certain they contain no new Motion-only state.

Because migration never edits the source profile, rollback does not require restoring a generated backup.

## Re-running migration

Migration intentionally does not overwrite a Motion destination. To re-copy a specific profile area:

```sh
mv ~/.config/motion ~/.config/motion.previous
motion --daemon
```

Repeat for state or data only when required. Preserve the renamed directory until the new copy is verified.

## Manual recovery

If startup reports a migration warning:

- check ownership and permissions of the XDG base directories;
- ensure the destination parent exists or is creatable;
- remove stale `.motion-migration-*` sibling staging directories only after Motion is stopped;
- verify there is enough disk space;
- manually copy with symlink preservation if needed:

```sh
cp -a ~/.config/noctalia ~/.config/motion
```

Never merge state databases or credentials blindly. Compare files and prefer the newer valid Motion state.
