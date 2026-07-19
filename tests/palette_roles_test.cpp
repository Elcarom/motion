#include "theme/palette_generator.h"
#include "theme/palette_mapping.h"
#include "ui/palette.h"

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <string_view>
#include <vector>

namespace {

  bool expect(bool condition, std::string_view message) {
    if (!condition) {
      std::fprintf(stderr, "palette_roles_test: FAIL: %.*s\n", static_cast<int>(message.size()), message.data());
    }
    return condition;
  }

  bool hasVisibleRgb(const Color& color) { return color.r > 0.0f || color.g > 0.0f || color.b > 0.0f; }

} // namespace

int main() {
  using motion::theme::generate;
  using motion::theme::mapGeneratedPaletteMode;

  bool ok = true;
  std::vector<std::uint8_t> wallpaperRgb(112u * 112u * 3u);
  for (std::size_t i = 0; i < wallpaperRgb.size(); i += 3) {
    const auto pixel = static_cast<std::uint8_t>((i / 3u) % 256u);
    wallpaperRgb[i] = pixel;
    wallpaperRgb[i + 1] = static_cast<std::uint8_t>(255u - pixel);
    wallpaperRgb[i + 2] = static_cast<std::uint8_t>((pixel * 3u) % 256u);
  }

  const auto generatedResult = generate(wallpaperRgb);
  ok = expect(generatedResult.has_value(), "wallpaper produces an M3 Expressive palette") && ok;
  if (!generatedResult.has_value()) {
    return 1;
  }
  const auto& generated = *generatedResult;

  std::vector<std::uint8_t> secondWallpaperRgb(112u * 112u * 3u, 0u);
  for (std::size_t i = 0; i < secondWallpaperRgb.size(); i += 3) {
    secondWallpaperRgb[i] = 234u;
    secondWallpaperRgb[i + 1] = 88u;
    secondWallpaperRgb[i + 2] = 12u;
  }
  const auto secondGenerated = generate(secondWallpaperRgb);
  ok = expect(secondGenerated.has_value(), "second wallpaper produces an M3 Expressive palette") && ok;
  if (secondGenerated.has_value()) {
    ok = expect(
             generated.dark.at("source_color") != secondGenerated->dark.at("source_color"),
             "wallpaper pixels determine the generated palette"
         )
        && ok;
  }

  Palette mapped = mapGeneratedPaletteMode(generated.dark);
  setPalette(mapped);

  for (const auto& token : kColorRoleTokens) {
    ok = expect(colorRoleFromToken(token.token) == token.role, "role token round-trips") && ok;
    ok = expect(colorRoleToken(token.role) == token.token, "role serializes canonically") && ok;
  }

  ok = expect(hasVisibleRgb(colorForRole(ColorRole::PrimaryContainer)), "primary container is mapped") && ok;
  ok = expect(hasVisibleRgb(colorForRole(ColorRole::SecondaryContainer)), "secondary container is mapped") && ok;
  ok = expect(hasVisibleRgb(colorForRole(ColorRole::SurfaceContainerHigh)), "surface hierarchy is mapped") && ok;
  ok = expect(hasVisibleRgb(colorForRole(ColorRole::InverseSurface)), "inverse surface is mapped") && ok;
  ok = expect(
           onColorRoleFor(ColorRole::PrimaryContainer) == ColorRole::OnPrimaryContainer,
           "primary container resolves its semantic foreground"
       )
      && ok;
  ok = expect(
           onColorRoleFor(ColorRole::SurfaceContainerHighest) == ColorRole::OnSurface,
           "surface container resolves its semantic foreground"
       )
      && ok;

  const Color layer = stateLayerColor(ColorRole::SurfaceContainer, ColorRole::OnSurface, 0.0f);
  ok = expect(layer == colorForRole(ColorRole::SurfaceContainer), "zero-opacity state layer preserves container") && ok;
  const Color pressed = stateLayerColor(ColorRole::SurfaceContainer, ColorRole::OnSurface, 1.0f);
  ok = expect(pressed == colorForRole(ColorRole::OnSurface), "full-opacity state layer resolves content color") && ok;

  const Palette light = mapGeneratedPaletteMode(generated.light);
  const Palette midpoint = lerpPalette(mapped, light, 0.5f);
  ok = expect(
           midpoint.surfaceContainerHigh == lerpColor(mapped.surfaceContainerHigh, light.surfaceContainerHigh, 0.5f),
           "theme interpolation includes extended surface roles"
       )
      && ok;
  ok = expect(
           midpoint.errorContainer == lerpColor(mapped.errorContainer, light.errorContainer, 0.5f),
           "theme interpolation includes container roles"
       )
      && ok;

  return ok ? 0 : 1;
}
