#pragma once

#include "theme/palette_mapping.h"

namespace motion::theme {

  // Accessibility transform applied after M3 Expressive wallpaper generation.
  void applyHighContrast(TokenMap& tokens, bool isDark);
  void applyHighContrast(GeneratedPalette& palette);

} // namespace motion::theme
