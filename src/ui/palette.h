#pragma once

#include "render/core/color.h"
#include "ui/signal.h"
#include "ui/style.h"

#include <array>
#include <cstdint>
#include <optional>
#include <string_view>

enum class ColorRole : std::uint8_t {
  Primary,
  OnPrimary,
  PrimaryContainer,
  OnPrimaryContainer,
  Secondary,
  OnSecondary,
  SecondaryContainer,
  OnSecondaryContainer,
  Tertiary,
  OnTertiary,
  TertiaryContainer,
  OnTertiaryContainer,
  Error,
  OnError,
  ErrorContainer,
  OnErrorContainer,
  Background,
  OnBackground,
  Surface,
  OnSurface,
  SurfaceVariant,
  OnSurfaceVariant,
  SurfaceDim,
  SurfaceBright,
  SurfaceContainerLowest,
  SurfaceContainerLow,
  SurfaceContainer,
  SurfaceContainerHigh,
  SurfaceContainerHighest,
  SurfaceTint,
  Outline,
  OutlineVariant,
  InverseSurface,
  InverseOnSurface,
  InversePrimary,
  Scrim,
  Shadow,
  // Compatibility roles accepted in configuration. Canonical controls use
  // state layers over their own container instead of switching to these.
  Hover,
  OnHover,
};

struct ColorRoleToken {
  ColorRole role;
  std::string_view token;
};

inline constexpr std::array<ColorRoleToken, 39> kColorRoleTokens = {{
    {ColorRole::Primary, "primary"},
    {ColorRole::OnPrimary, "on_primary"},
    {ColorRole::PrimaryContainer, "primary_container"},
    {ColorRole::OnPrimaryContainer, "on_primary_container"},
    {ColorRole::Secondary, "secondary"},
    {ColorRole::OnSecondary, "on_secondary"},
    {ColorRole::SecondaryContainer, "secondary_container"},
    {ColorRole::OnSecondaryContainer, "on_secondary_container"},
    {ColorRole::Tertiary, "tertiary"},
    {ColorRole::OnTertiary, "on_tertiary"},
    {ColorRole::TertiaryContainer, "tertiary_container"},
    {ColorRole::OnTertiaryContainer, "on_tertiary_container"},
    {ColorRole::Error, "error"},
    {ColorRole::OnError, "on_error"},
    {ColorRole::ErrorContainer, "error_container"},
    {ColorRole::OnErrorContainer, "on_error_container"},
    {ColorRole::Background, "background"},
    {ColorRole::OnBackground, "on_background"},
    {ColorRole::Surface, "surface"},
    {ColorRole::OnSurface, "on_surface"},
    {ColorRole::SurfaceVariant, "surface_variant"},
    {ColorRole::OnSurfaceVariant, "on_surface_variant"},
    {ColorRole::SurfaceDim, "surface_dim"},
    {ColorRole::SurfaceBright, "surface_bright"},
    {ColorRole::SurfaceContainerLowest, "surface_container_lowest"},
    {ColorRole::SurfaceContainerLow, "surface_container_low"},
    {ColorRole::SurfaceContainer, "surface_container"},
    {ColorRole::SurfaceContainerHigh, "surface_container_high"},
    {ColorRole::SurfaceContainerHighest, "surface_container_highest"},
    {ColorRole::SurfaceTint, "surface_tint"},
    {ColorRole::Outline, "outline"},
    {ColorRole::OutlineVariant, "outline_variant"},
    {ColorRole::InverseSurface, "inverse_surface"},
    {ColorRole::InverseOnSurface, "inverse_on_surface"},
    {ColorRole::InversePrimary, "inverse_primary"},
    {ColorRole::Scrim, "scrim"},
    {ColorRole::Shadow, "shadow"},
    {ColorRole::Hover, "hover"},
    {ColorRole::OnHover, "on_hover"},
}};

[[nodiscard]] constexpr Color clearColor() noexcept { return rgba(0.0f, 0.0f, 0.0f, 0.0f); }

struct ColorSpec {
  std::optional<ColorRole> role;
  Color fixed = clearColor();
  float alpha = 1.0f;
};

constexpr bool operator==(const ColorSpec& a, const ColorSpec& b) noexcept {
  return a.role == b.role && a.fixed == b.fixed && a.alpha == b.alpha;
}

[[nodiscard]] constexpr ColorSpec clearColorSpec() noexcept {
  return ColorSpec{.role = std::nullopt, .fixed = clearColor(), .alpha = 1.0f};
}

struct Palette {
  Color primary;
  Color onPrimary;
  Color secondary;
  Color onSecondary;
  Color tertiary;
  Color onTertiary;
  Color error;
  Color onError;
  Color surface;
  Color onSurface;
  Color surfaceVariant;
  Color onSurfaceVariant;
  Color outline;
  Color shadow;
  Color hover;
  Color onHover;
  Color primaryContainer;
  Color onPrimaryContainer;
  Color secondaryContainer;
  Color onSecondaryContainer;
  Color tertiaryContainer;
  Color onTertiaryContainer;
  Color errorContainer;
  Color onErrorContainer;
  Color background;
  Color onBackground;
  Color surfaceDim;
  Color surfaceBright;
  Color surfaceContainerLowest;
  Color surfaceContainerLow;
  Color surfaceContainer;
  Color surfaceContainerHigh;
  Color surfaceContainerHighest;
  Color surfaceTint;
  Color outlineVariant;
  Color inverseSurface;
  Color inverseOnSurface;
  Color inversePrimary;
  Color scrim;
};

constexpr bool operator==(const Palette& lhs, const Palette& rhs) noexcept {
  return lhs.primary == rhs.primary
      && lhs.onPrimary == rhs.onPrimary
      && lhs.secondary == rhs.secondary
      && lhs.onSecondary == rhs.onSecondary
      && lhs.tertiary == rhs.tertiary
      && lhs.onTertiary == rhs.onTertiary
      && lhs.error == rhs.error
      && lhs.onError == rhs.onError
      && lhs.surface == rhs.surface
      && lhs.onSurface == rhs.onSurface
      && lhs.surfaceVariant == rhs.surfaceVariant
      && lhs.onSurfaceVariant == rhs.onSurfaceVariant
      && lhs.outline == rhs.outline
      && lhs.shadow == rhs.shadow
      && lhs.hover == rhs.hover
      && lhs.onHover == rhs.onHover
      && lhs.primaryContainer == rhs.primaryContainer
      && lhs.onPrimaryContainer == rhs.onPrimaryContainer
      && lhs.secondaryContainer == rhs.secondaryContainer
      && lhs.onSecondaryContainer == rhs.onSecondaryContainer
      && lhs.tertiaryContainer == rhs.tertiaryContainer
      && lhs.onTertiaryContainer == rhs.onTertiaryContainer
      && lhs.errorContainer == rhs.errorContainer
      && lhs.onErrorContainer == rhs.onErrorContainer
      && lhs.background == rhs.background
      && lhs.onBackground == rhs.onBackground
      && lhs.surfaceDim == rhs.surfaceDim
      && lhs.surfaceBright == rhs.surfaceBright
      && lhs.surfaceContainerLowest == rhs.surfaceContainerLowest
      && lhs.surfaceContainerLow == rhs.surfaceContainerLow
      && lhs.surfaceContainer == rhs.surfaceContainer
      && lhs.surfaceContainerHigh == rhs.surfaceContainerHigh
      && lhs.surfaceContainerHighest == rhs.surfaceContainerHighest
      && lhs.surfaceTint == rhs.surfaceTint
      && lhs.outlineVariant == rhs.outlineVariant
      && lhs.inverseSurface == rhs.inverseSurface
      && lhs.inverseOnSurface == rhs.inverseOnSurface
      && lhs.inversePrimary == rhs.inversePrimary
      && lhs.scrim == rhs.scrim;
}

extern Palette palette;

[[nodiscard]] const Color& colorForRole(ColorRole role) noexcept;
[[nodiscard]] Color colorForRole(ColorRole role, float alpha) noexcept;
[[nodiscard]] std::optional<ColorRole> colorRoleFromToken(std::string_view token);
[[nodiscard]] std::string_view colorRoleToken(ColorRole role) noexcept;
[[nodiscard]] ColorSpec colorSpecFromRole(ColorRole role, float alpha = 1.0f) noexcept;
[[nodiscard]] ColorSpec fixedColorSpec(const Color& color) noexcept;
[[nodiscard]] Color resolveColorSpec(const ColorSpec& color) noexcept;
// Resolves a Material state layer as an opaque composite over its container.
// This keeps hover/focus/press semantics tied to the component's base role.
[[nodiscard]] Color stateLayerColor(ColorRole container, ColorRole content, float opacity) noexcept;
// Returns the semantic foreground role intended for a role-backed container.
[[nodiscard]] ColorRole onColorRoleFor(ColorRole container) noexcept;
[[nodiscard]] bool isLightPalette() noexcept;
// Set by ThemeService from resolved light/dark mode (not inferred from palette colors).
[[nodiscard]] bool isResolvedLightTheme() noexcept;
void setResolvedThemeLight(bool light) noexcept;

void setPalette(const Palette& p);

[[nodiscard]] inline ColorSpec scrollbarTrackColor() noexcept {
  return colorSpecFromRole(ColorRole::OutlineVariant, 0.36f);
}
[[nodiscard]] inline ColorSpec scrollbarThumbColor() noexcept {
  return colorSpecFromRole(ColorRole::OnSurfaceVariant, 0.64f);
}
[[nodiscard]] inline ColorSpec focusRingColorSpec() noexcept { return colorSpecFromRole(ColorRole::Primary); }

// Fired after setPalette() writes. Controls subscribe in their constructor
// and re-apply palette-derived colors to their scene nodes on each emit.
Signal<>& paletteChanged();

// Linearly interpolates each field of two palettes in sRGB space. Used by
// ThemeService to drive smooth cross-fade transitions on theme changes.
Palette lerpPalette(const Palette& a, const Palette& b, float t);
