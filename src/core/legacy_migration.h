#pragma once

#include <string>
#include <vector>

namespace motion::legacy {

  struct MigrationReport {
    std::vector<std::string> migrated;
    std::vector<std::string> warnings;
  };

  // Copies an existing Noctalia profile to Motion's XDG directories when the
  // corresponding Motion directory does not exist. Source directories are
  // never changed or removed. Each destination is staged in a sibling
  // temporary directory and atomically renamed into place.
  [[nodiscard]] MigrationReport migrateNoctaliaProfile();

} // namespace motion::legacy
