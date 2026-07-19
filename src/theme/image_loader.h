#pragma once

#include <cstdint>
#include <expected>
#include <string>
#include <string_view>
#include <vector>

namespace motion::theme {

  // A 112×112 RGB pixel buffer ready for quantization/clustering. No alpha.
  // 112 * 112 * 3 = 37632 bytes.
  struct LoadedImage {
    std::vector<uint8_t> rgb; // size = 112 * 112 * 3
    int width = 112;
    int height = 112;
  };

  // Load `path`, decode via Wuffs, and resize to exactly 112×112 (aspect ratio
  // squashed) with triangle filtering and alpha stripped.
  std::expected<LoadedImage, std::string> loadAndResize(std::string_view path);

} // namespace motion::theme
