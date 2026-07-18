# Upstream provenance

Motion is an independent fork derived from the supplied **Noctalia 5.0.0-beta.3** source archive.

- Upstream project: Noctalia
- Upstream repository: `https://github.com/noctalia-dev/noctalia`
- Supplied archive directory: `noctalia-5.0.0-beta.3`
- Archive commit marker: `6b3632096231ecde16780937ef7ec26e8a1de388`
- Upstream license: MIT
- Upstream copyright: `Copyright (c) 2026 noctalia-dev`

The archive did not include `.git` metadata, so commit history could not be preserved inside this delivery. The marker
above came from the supplied archive metadata and is retained for traceability.

## Relationship

Motion is not an official Noctalia release and is not endorsed by the Noctalia maintainers. References to Noctalia are
retained where required for attribution, migration, compatibility aliases, inherited service endpoints, plugin source
compatibility, and historical context.

## Major fork changes

- Product, executable, desktop entry, package, application-facing identifiers, assets, and documentation renamed to
  Motion.
- Original Motion vector icon and wallpaper added; upstream owl/moon branding is not used in normal Motion UI.
- Central Material 3 Expressive-inspired token system and default expressive dynamic color scheme added.
- Safe one-time profile migration and legacy command/environment/hook compatibility added.
- IPC and single-instance runtime paths hardened against unsafe runtime directories and path replacement.
- Telemetry permanently disabled while preserving the legacy configuration key for parsing compatibility.
- Documentation, tests, CI, release packaging, and publication metadata rebuilt for the fork.

## Updating from upstream

Future maintainers should add Noctalia as an explicit Git remote after initializing this repository:

```sh
git remote add upstream https://github.com/noctalia-dev/noctalia.git
git fetch upstream --tags
```

Resolve changes deliberately. Do not bulk-replace product names in third-party notices, migration code, inherited API
endpoints, plugin IDs, or upstream attribution.
