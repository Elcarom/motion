#pragma once

#include <string>

namespace motion::theme {

  struct GeneratedPalette;
  enum class Variant { Dark, Light, Both };

  // Serialize a GeneratedPalette to JSON. Values are "#rrggbb" strings emitted
  // in the canonical iteration order from tokens.h. `Both` wraps dark+light in
  // `{"dark": {...}, "light": {...}}`; `Dark`/`Light` emit a flat token map.
  std::string toJson(const GeneratedPalette& palette, Variant variant);

} // namespace motion::theme
