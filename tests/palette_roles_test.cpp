#include "theme/builtin_palettes.h"
#include "theme/fixed_palette.h"
#include "ui/palette.h"

#include <cstdio>
#include <string_view>

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
  using motion::theme::expandBuiltinPalette;
  using motion::theme::findBuiltinPalette;
  using motion::theme::mapGeneratedPaletteMode;

  bool ok = true;
  const auto* builtin = findBuiltinPalette("Motion");
  ok = expect(builtin != nullptr, "Motion built-in palette exists") && ok;
  if (builtin == nullptr) {
    return 1;
  }

  const auto generated = expandBuiltinPalette(*builtin);
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
