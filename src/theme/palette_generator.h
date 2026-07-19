#pragma once

#include "theme/palette.h"

#include <cstdint>
#include <expected>
#include <string>
#include <vector>

namespace motion::theme {

  // Top-level entry point. Accepts a forced-112×112 RGB (no alpha) pixel buffer
  // and returns a Material 3 Expressive dark+light palette.
  //
  // The buffer must contain exactly 112 * 112 * 3 bytes.
  std::expected<GeneratedPalette, std::string> generate(const std::vector<uint8_t>& rgb112);

  // Internal path exposed for unit testing.
  GeneratedPalette generateExpressive(const std::vector<uint8_t>& rgb112);

} // namespace motion::theme
