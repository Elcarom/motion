#include "theme/palette_transform.h"

#include "cpp/cam/hct.h"

#include <algorithm>

namespace motion::theme {

  namespace mcu = material_color_utilities;

  void applyHighContrast(TokenMap& tokens, bool isDark) {
    for (auto& [key, value] : tokens) {
      mcu::Hct hct(value);
      double tone = hct.get_tone();

      if (key == "outline" || key == "outline_variant") {
        // Force borders to be highly visible: extremely light in dark mode, extremely dark in light mode
        tone = isDark ? std::max(tone, 80.0) : std::min(tone, 20.0);
      } else {
        // Aggressive contrast stretch for everything else:
        // Push darks darker and lights lighter to separate backgrounds from foregrounds
        if (tone < 50.0) {
          tone = std::max(0.0, tone - 20.0);
        } else {
          tone = std::min(100.0, tone + 20.0);
        }
      }

      hct.set_tone(tone);
      value = hct.ToInt();
    }
  }

  void applyHighContrast(GeneratedPalette& palette) {
    if (!palette.dark.empty()) {
      applyHighContrast(palette.dark, true);
    }
    if (!palette.light.empty()) {
      applyHighContrast(palette.light, false);
    }
  }

} // namespace motion::theme
