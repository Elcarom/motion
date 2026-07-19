#pragma once

#include "ui/design_tokens.h"
#include "ui/signal.h"

namespace Style {

  inline constexpr int barThicknessDefault = static_cast<int>(motion::design::size::barStandard);

  inline constexpr int animFast = motion::design::motion::fast;
  inline constexpr int animNormal = motion::design::motion::standard;
  inline constexpr int animSlow = motion::design::motion::longEnter;

  inline constexpr float radiusSm = motion::design::shape::extraSmall;
  inline constexpr float radiusMd = motion::design::shape::standardControl;
  inline constexpr float radiusLg = motion::design::shape::large;
  inline constexpr float radiusXl = motion::design::shape::extraLarge;

  inline constexpr float radiusButton = motion::design::shape::prominentControl;
  inline constexpr float radiusIconButton = motion::design::shape::circular;
  inline constexpr float radiusField = motion::design::shape::standardControl;
  inline constexpr float radiusRow = motion::design::shape::standardControl;
  inline constexpr float radiusCard = motion::design::shape::card;
  inline constexpr float radiusMenu = motion::design::shape::menu;
  inline constexpr float radiusPopup = motion::design::shape::popup;
  inline constexpr float radiusPanel = motion::design::shape::panel;
  inline constexpr float radiusDialog = motion::design::shape::dialog;

  inline constexpr float borderWidth = motion::design::border::hairline;
  inline constexpr float emphasizedBorderWidth = motion::design::border::focusHighContrast;
  inline constexpr float focusRingWidth = motion::design::border::focus;
  inline constexpr float disabledOutlineAlpha = motion::design::state::disabledOutline;

  inline constexpr float spaceXs = motion::design::spacing::xs;
  inline constexpr float spaceSm = motion::design::spacing::sm;
  inline constexpr float spaceMd = motion::design::spacing::md;
  inline constexpr float spaceLg = motion::design::spacing::lg;

  inline constexpr float cardPadding = motion::design::spacing::lg;
  inline constexpr float panelPadding = motion::design::spacing::xl;

  inline constexpr float barCapsulePadding = motion::design::spacing::sm;
  inline constexpr float baseGlyphSize = motion::design::size::iconSmall;

  inline constexpr float fontSizeMini = motion::design::type::labelSmall;
  inline constexpr float fontSizeCaption = motion::design::type::bodySmall;
  inline constexpr float fontSizeBody = motion::design::type::bodyMedium;
  inline constexpr float fontSizeBodyLarge = motion::design::type::bodyLarge;
  inline constexpr float fontSizeTitle = motion::design::type::titleSmall;
  inline constexpr float fontSizeTitleLarge = motion::design::type::titleLarge;
  inline constexpr float fontSizeHeader = motion::design::type::headlineSmall;
  inline constexpr float fontSizeDisplay = motion::design::type::displaySmall;

  inline constexpr float controlHeightSm = motion::design::size::controlCompact;
  inline constexpr float controlHeight = motion::design::size::controlStandard;
  inline constexpr float controlHeightLg = motion::design::size::controlProminent;
  inline constexpr float searchHeroHeight = motion::design::size::searchHero;
  inline constexpr float quickSettingTileHeight = motion::design::size::quickSettingTile;
  inline constexpr float launcherTileHeight = motion::design::size::launcherTile;
  inline constexpr float launcherTileMinWidth = motion::design::size::launcherTileMinWidth;
  inline constexpr float scrollWheelStep = 56.0f;

  inline constexpr float scrollbarWidth = 6.0f;
  inline constexpr float scrollbarHoverWidth = 8.0f;
  inline constexpr float scrollbarGap = spaceSm;
  inline constexpr float scrollbarMinThumbHeight = 24.0f;

  inline constexpr float toggleThumbSizeSm = 14.0f;
  inline constexpr float toggleInsetSm = 2.0f;
  inline constexpr float toggleTravelSm = 12.0f;
  inline constexpr float toggleThumbSizeMd = 18.0f;
  inline constexpr float toggleInsetMd = 3.0f;
  inline constexpr float toggleTravelMd = 16.0f;
  inline constexpr float toggleThumbSizeLg = 22.0f;
  inline constexpr float toggleInsetLg = 4.0f;
  inline constexpr float toggleTravelLg = 20.0f;

  inline constexpr float sliderDefaultWidth = 180.0f;
  inline constexpr float sliderTrackHeight = 8.0f;
  inline constexpr float sliderThumbSize = 20.0f;
  inline constexpr float sliderHorizontalPadding = 2.0f;

  [[nodiscard]] float cornerRadiusScale() noexcept;
  void setCornerRadiusScale(float scale) noexcept;

  [[nodiscard]] bool buttonBordersEnabled() noexcept;
  void setButtonBordersEnabled(bool enabled);
  Signal<>& buttonBordersChanged();

  [[nodiscard]] float scaledRadius(float radius, float localScale = 1.0f) noexcept;
  [[nodiscard]] float scaledRadiusSm(float localScale = 1.0f) noexcept;
  [[nodiscard]] float scaledRadiusMd(float localScale = 1.0f) noexcept;
  [[nodiscard]] float scaledRadiusLg(float localScale = 1.0f) noexcept;
  [[nodiscard]] float scaledRadiusXl(float localScale = 1.0f) noexcept;
  [[nodiscard]] float scaledSemanticRadius(float radius, float localScale = 1.0f) noexcept;

} // namespace Style
