# Credits and third-party notices

## Noctalia upstream

Motion is built from the MIT-licensed Noctalia 5.0.0-beta.3 codebase by `noctalia-dev`. The original copyright and MIT
license are preserved in [`LICENSE`](LICENSE). Provenance and the supplied archive marker are recorded in
[`UPSTREAM.md`](UPSTREAM.md).

The original Noctalia designers, maintainers, translators, contributors, testers, and community created the architecture
and majority of the functionality on which Motion depends. Motion does not claim ownership of that work.

Upstream branding credits for the source project include MrDowntempo (Noctalia owl/moon logo) and SaberJ2X (Talia
mascot). Those upstream brand assets are not used as Motion's normal product identity.

## Motion identity

The Motion icon (`assets/motion.svg`) and wallpaper (`assets/motion-wallpaper.png`) are original repository-native assets
created for this fork. They use abstract flowing ribbons and do not copy Google, Android, Pixel, or Noctalia branding.

## Icons and audio

- [Tabler Icons](https://tabler.io/icons) — primary shell icon set.
- Riyan Resdian on Noun Project — inherited plug icon attribution.
- Universfield on Pixabay — inherited notification sound effect.
- Lucas McCallister on Freesound — inherited volume-change feedback sound effect.

## Dynamically linked system libraries

Wayland and wayland-protocols; Mesa/EGL/GLES or libepoxy; Cairo; Pango/PangoCairo; HarfBuzz; FreeType; Fontconfig;
librsvg; GLib/GObject/GIO; libxkbcommon; sdbus-c++; PipeWire; WirePlumber; libcurl; libwebp; libqalculate; libxml2;
polkit; Linux-PAM; and optional jemalloc. Each library is governed by its own license.

## Vendored or source-bundled components

- dr_wav — MIT-0 / public-domain option
- fzy — MIT
- md4c — MIT
- Luau — MIT
- Material Color Utilities — Apache-2.0
- nlohmann/json — MIT
- stb — MIT / public domain
- toml++ — MIT
- Wuffs — Apache-2.0

Refer to files inside `third_party/`, dependency source headers, and package metadata for exact notices and versions.

## Runtime integrations

Motion can integrate with compositor IPC, D-Bus services, PipeWire/WirePlumber, NetworkManager, BlueZ, UPower,
power-profiles-daemon, polkit, PAM, `ddcutil`, and user-selected external commands. These projects are independent of
Motion.
