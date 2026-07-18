#include "ui/palette.h"

#include "render/core/color.h"
#include "theme/builtin_palettes.h"
#include "theme/fixed_palette.h"
#include "util/string_utils.h"

#include <algorithm>
#include <string>

Palette palette = [] {
  const auto* builtin = motion::theme::findBuiltinPalette("Motion");
  const auto generated = motion::theme::expandBuiltinPalette(*builtin);
  return motion::theme::mapGeneratedPaletteMode(generated.dark);
}();
bool g_resolvedThemeLight = false;

namespace {

  std::string normalizedRoleToken(std::string_view token) {
    std::string normalized = StringUtils::trim(token);
    StringUtils::toLowerInPlace(normalized);
    return normalized;
  }

} // namespace

const Color& colorForRole(ColorRole role) noexcept {
  switch (role) {
  case ColorRole::Primary:
    return palette.primary;
  case ColorRole::OnPrimary:
    return palette.onPrimary;
  case ColorRole::PrimaryContainer:
    return palette.primaryContainer;
  case ColorRole::OnPrimaryContainer:
    return palette.onPrimaryContainer;
  case ColorRole::Secondary:
    return palette.secondary;
  case ColorRole::OnSecondary:
    return palette.onSecondary;
  case ColorRole::SecondaryContainer:
    return palette.secondaryContainer;
  case ColorRole::OnSecondaryContainer:
    return palette.onSecondaryContainer;
  case ColorRole::Tertiary:
    return palette.tertiary;
  case ColorRole::OnTertiary:
    return palette.onTertiary;
  case ColorRole::TertiaryContainer:
    return palette.tertiaryContainer;
  case ColorRole::OnTertiaryContainer:
    return palette.onTertiaryContainer;
  case ColorRole::Error:
    return palette.error;
  case ColorRole::OnError:
    return palette.onError;
  case ColorRole::ErrorContainer:
    return palette.errorContainer;
  case ColorRole::OnErrorContainer:
    return palette.onErrorContainer;
  case ColorRole::Background:
    return palette.background;
  case ColorRole::OnBackground:
    return palette.onBackground;
  case ColorRole::Surface:
    return palette.surface;
  case ColorRole::OnSurface:
    return palette.onSurface;
  case ColorRole::SurfaceVariant:
    return palette.surfaceVariant;
  case ColorRole::OnSurfaceVariant:
    return palette.onSurfaceVariant;
  case ColorRole::SurfaceDim:
    return palette.surfaceDim;
  case ColorRole::SurfaceBright:
    return palette.surfaceBright;
  case ColorRole::SurfaceContainerLowest:
    return palette.surfaceContainerLowest;
  case ColorRole::SurfaceContainerLow:
    return palette.surfaceContainerLow;
  case ColorRole::SurfaceContainer:
    return palette.surfaceContainer;
  case ColorRole::SurfaceContainerHigh:
    return palette.surfaceContainerHigh;
  case ColorRole::SurfaceContainerHighest:
    return palette.surfaceContainerHighest;
  case ColorRole::SurfaceTint:
    return palette.surfaceTint;
  case ColorRole::Outline:
    return palette.outline;
  case ColorRole::OutlineVariant:
    return palette.outlineVariant;
  case ColorRole::InverseSurface:
    return palette.inverseSurface;
  case ColorRole::InverseOnSurface:
    return palette.inverseOnSurface;
  case ColorRole::InversePrimary:
    return palette.inversePrimary;
  case ColorRole::Scrim:
    return palette.scrim;
  case ColorRole::Shadow:
    return palette.shadow;
  case ColorRole::Hover:
    return palette.hover;
  case ColorRole::OnHover:
    return palette.onHover;
  }

  return palette.onSurface;
}

Color colorForRole(ColorRole role, float alpha) noexcept {
  Color color = colorForRole(role);
  color.a *= alpha;
  return color;
}

std::optional<ColorRole> colorRoleFromToken(std::string_view token) {
  const std::string normalized = normalizedRoleToken(token);
  for (const auto& option : kColorRoleTokens) {
    if (normalized == option.token) {
      return option.role;
    }
  }
  return std::nullopt;
}

std::string_view colorRoleToken(ColorRole role) noexcept {
  for (const auto& option : kColorRoleTokens) {
    if (option.role == role) {
      return option.token;
    }
  }
  return "on_surface";
}

ColorSpec colorSpecFromRole(ColorRole role, float alpha) noexcept {
  return ColorSpec{.role = role, .fixed = clearColor(), .alpha = alpha};
}

ColorSpec fixedColorSpec(const Color& color) noexcept {
  return ColorSpec{.role = std::nullopt, .fixed = color, .alpha = 1.0f};
}

Color resolveColorSpec(const ColorSpec& color) noexcept {
  Color resolved = color.role.has_value() ? colorForRole(*color.role) : color.fixed;
  resolved.a *= color.alpha;
  return resolved;
}

Color stateLayerColor(ColorRole container, ColorRole content, float opacity) noexcept {
  return lerpColor(colorForRole(container), colorForRole(content), std::clamp(opacity, 0.0f, 1.0f));
}

ColorRole onColorRoleFor(ColorRole container) noexcept {
  switch (container) {
  case ColorRole::Primary:
    return ColorRole::OnPrimary;
  case ColorRole::PrimaryContainer:
    return ColorRole::OnPrimaryContainer;
  case ColorRole::Secondary:
    return ColorRole::OnSecondary;
  case ColorRole::SecondaryContainer:
    return ColorRole::OnSecondaryContainer;
  case ColorRole::Tertiary:
    return ColorRole::OnTertiary;
  case ColorRole::TertiaryContainer:
    return ColorRole::OnTertiaryContainer;
  case ColorRole::Error:
    return ColorRole::OnError;
  case ColorRole::ErrorContainer:
    return ColorRole::OnErrorContainer;
  case ColorRole::Background:
    return ColorRole::OnBackground;
  case ColorRole::InverseSurface:
    return ColorRole::InverseOnSurface;
  case ColorRole::Surface:
  case ColorRole::SurfaceVariant:
  case ColorRole::SurfaceDim:
  case ColorRole::SurfaceBright:
  case ColorRole::SurfaceContainerLowest:
  case ColorRole::SurfaceContainerLow:
  case ColorRole::SurfaceContainer:
  case ColorRole::SurfaceContainerHigh:
  case ColorRole::SurfaceContainerHighest:
    return ColorRole::OnSurface;
  case ColorRole::OnPrimary:
  case ColorRole::OnPrimaryContainer:
  case ColorRole::OnSecondary:
  case ColorRole::OnSecondaryContainer:
  case ColorRole::OnTertiary:
  case ColorRole::OnTertiaryContainer:
  case ColorRole::OnError:
  case ColorRole::OnErrorContainer:
  case ColorRole::OnBackground:
  case ColorRole::OnSurface:
  case ColorRole::OnSurfaceVariant:
  case ColorRole::SurfaceTint:
  case ColorRole::Outline:
  case ColorRole::OutlineVariant:
  case ColorRole::InverseOnSurface:
  case ColorRole::InversePrimary:
  case ColorRole::Scrim:
  case ColorRole::Shadow:
  case ColorRole::Hover:
  case ColorRole::OnHover:
    return ColorRole::OnSurface;
  }
  return ColorRole::OnSurface;
}

bool isLightPalette() noexcept { return relativeLuminance(palette.surface) > 0.179f; }

bool isResolvedLightTheme() noexcept { return g_resolvedThemeLight; }

void setResolvedThemeLight(bool light) noexcept { g_resolvedThemeLight = light; }

Signal<>& paletteChanged() {
  static Signal<> signal;
  return signal;
}

void setPalette(const Palette& p) {
  if (palette == p) {
    return;
  }
  palette = p;
  paletteChanged().emit();
}

Palette lerpPalette(const Palette& a, const Palette& b, float t) {
  return Palette{
      .primary = lerpColor(a.primary, b.primary, t),
      .onPrimary = lerpColor(a.onPrimary, b.onPrimary, t),
      .secondary = lerpColor(a.secondary, b.secondary, t),
      .onSecondary = lerpColor(a.onSecondary, b.onSecondary, t),
      .tertiary = lerpColor(a.tertiary, b.tertiary, t),
      .onTertiary = lerpColor(a.onTertiary, b.onTertiary, t),
      .error = lerpColor(a.error, b.error, t),
      .onError = lerpColor(a.onError, b.onError, t),
      .surface = lerpColor(a.surface, b.surface, t),
      .onSurface = lerpColor(a.onSurface, b.onSurface, t),
      .surfaceVariant = lerpColor(a.surfaceVariant, b.surfaceVariant, t),
      .onSurfaceVariant = lerpColor(a.onSurfaceVariant, b.onSurfaceVariant, t),
      .outline = lerpColor(a.outline, b.outline, t),
      .shadow = lerpColor(a.shadow, b.shadow, t),
      .hover = lerpColor(a.hover, b.hover, t),
      .onHover = lerpColor(a.onHover, b.onHover, t),
      .primaryContainer = lerpColor(a.primaryContainer, b.primaryContainer, t),
      .onPrimaryContainer = lerpColor(a.onPrimaryContainer, b.onPrimaryContainer, t),
      .secondaryContainer = lerpColor(a.secondaryContainer, b.secondaryContainer, t),
      .onSecondaryContainer = lerpColor(a.onSecondaryContainer, b.onSecondaryContainer, t),
      .tertiaryContainer = lerpColor(a.tertiaryContainer, b.tertiaryContainer, t),
      .onTertiaryContainer = lerpColor(a.onTertiaryContainer, b.onTertiaryContainer, t),
      .errorContainer = lerpColor(a.errorContainer, b.errorContainer, t),
      .onErrorContainer = lerpColor(a.onErrorContainer, b.onErrorContainer, t),
      .background = lerpColor(a.background, b.background, t),
      .onBackground = lerpColor(a.onBackground, b.onBackground, t),
      .surfaceDim = lerpColor(a.surfaceDim, b.surfaceDim, t),
      .surfaceBright = lerpColor(a.surfaceBright, b.surfaceBright, t),
      .surfaceContainerLowest = lerpColor(a.surfaceContainerLowest, b.surfaceContainerLowest, t),
      .surfaceContainerLow = lerpColor(a.surfaceContainerLow, b.surfaceContainerLow, t),
      .surfaceContainer = lerpColor(a.surfaceContainer, b.surfaceContainer, t),
      .surfaceContainerHigh = lerpColor(a.surfaceContainerHigh, b.surfaceContainerHigh, t),
      .surfaceContainerHighest = lerpColor(a.surfaceContainerHighest, b.surfaceContainerHighest, t),
      .surfaceTint = lerpColor(a.surfaceTint, b.surfaceTint, t),
      .outlineVariant = lerpColor(a.outlineVariant, b.outlineVariant, t),
      .inverseSurface = lerpColor(a.inverseSurface, b.inverseSurface, t),
      .inverseOnSurface = lerpColor(a.inverseOnSurface, b.inverseOnSurface, t),
      .inversePrimary = lerpColor(a.inversePrimary, b.inversePrimary, t),
      .scrim = lerpColor(a.scrim, b.scrim, t),
  };
}
