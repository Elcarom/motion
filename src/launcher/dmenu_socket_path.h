#pragma once

#include <string>

namespace motion::launcher {

  struct DmenuSocketPathResult {
    std::string path;
    std::string error;
  };

  [[nodiscard]] DmenuSocketPathResult resolveDmenuSocketPath();

} // namespace motion::launcher
