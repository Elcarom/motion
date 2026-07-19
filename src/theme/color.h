#pragma once

#include <cstdint>
#include <string>
#include <tuple>

namespace motion::theme {

  // Minimal RGB colour (0-255 per channel) with hex/HSL/ARGB conversions.
  struct Color {
    int r = 0;
    int g = 0;
    int b = 0;

    Color() = default;
    constexpr Color(int red, int green, int blue) : r(red), g(green), b(blue) {}

    static Color fromHex(std::string_view hex); // accepts #RRGGBB or RRGGBB
    static Color fromHsl(double h, double s, double l);
    static Color fromArgb(uint32_t argb);

    std::string toHex() const;                        // "#rrggbb"
    std::tuple<double, double, double> toHsl() const; // (h°, s, l) — h in [0,360)
    uint32_t toArgb() const {
      return 0xff000000u
          | (static_cast<uint32_t>(r) << 16)
          | (static_cast<uint32_t>(g) << 8)
          | static_cast<uint32_t>(b);
    }
  };

} // namespace motion::theme
