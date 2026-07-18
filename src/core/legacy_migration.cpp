#include "core/legacy_migration.h"

#include <array>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <string>
#include <system_error>
#include <unistd.h>

namespace motion::legacy {

  namespace {

    struct DirectoryPair {
      const char* label;
      std::filesystem::path legacy;
      std::filesystem::path current;
    };

    std::filesystem::path envBase(const char* primary, const char* legacy, const char* xdg, const char* fallback) {
      if (const char* value = std::getenv(primary); value != nullptr && value[0] != '\0') {
        return value;
      }
      if (const char* value = std::getenv(legacy); value != nullptr && value[0] != '\0') {
        return value;
      }
      if (const char* value = std::getenv(xdg); value != nullptr && value[0] != '\0') {
        return value;
      }
      if (const char* home = std::getenv("HOME"); home != nullptr && home[0] != '\0') {
        return std::filesystem::path(home) / fallback;
      }
      return {};
    }

    std::filesystem::path legacyBase(const char* legacy, const char* xdg, const char* fallback) {
      if (const char* value = std::getenv(legacy); value != nullptr && value[0] != '\0') {
        return value;
      }
      if (const char* value = std::getenv(xdg); value != nullptr && value[0] != '\0') {
        return value;
      }
      if (const char* home = std::getenv("HOME"); home != nullptr && home[0] != '\0') {
        return std::filesystem::path(home) / fallback;
      }
      return {};
    }

    bool copyAtomically(const DirectoryPair& pair, std::string& warning) {
      std::error_code ec;
      if (pair.legacy.empty() || pair.current.empty() || pair.legacy == pair.current) {
        return false;
      }
      if (!std::filesystem::is_directory(pair.legacy, ec) || ec) {
        return false;
      }
      ec.clear();
      if (std::filesystem::exists(pair.current, ec)) {
        return false;
      }
      if (ec) {
        warning = std::string(pair.label) + ": could not inspect destination: " + ec.message();
        return false;
      }

      std::filesystem::create_directories(pair.current.parent_path(), ec);
      if (ec) {
        warning = std::string(pair.label) + ": could not create destination parent: " + ec.message();
        return false;
      }

      const auto staged = pair.current.parent_path()
          / (".motion-migration-" + std::to_string(static_cast<long long>(::getpid())) + "-" + pair.label);
      std::filesystem::remove_all(staged, ec);
      ec.clear();
      std::filesystem::copy(
          pair.legacy, staged, std::filesystem::copy_options::recursive | std::filesystem::copy_options::copy_symlinks,
          ec
      );
      if (ec) {
        std::filesystem::remove_all(staged, ec);
        warning = std::string(pair.label) + ": copy failed: " + ec.message();
        return false;
      }

      std::filesystem::rename(staged, pair.current, ec);
      if (ec) {
        std::filesystem::remove_all(staged, ec);
        warning = std::string(pair.label) + ": atomic install failed: " + ec.message();
        return false;
      }
      return true;
    }

  } // namespace

  MigrationReport migrateNoctaliaProfile() {
    MigrationReport report;

    const auto currentConfigBase = envBase("MOTION_CONFIG_HOME", "NOCTALIA_CONFIG_HOME", "XDG_CONFIG_HOME", ".config");
    const auto currentStateBase = envBase("MOTION_STATE_HOME", "NOCTALIA_STATE_HOME", "XDG_STATE_HOME", ".local/state");
    const auto currentDataBase = envBase("MOTION_DATA_HOME", "NOCTALIA_DATA_HOME", "XDG_DATA_HOME", ".local/share");
    const auto legacyConfigBase = legacyBase("NOCTALIA_CONFIG_HOME", "XDG_CONFIG_HOME", ".config");
    const auto legacyStateBase = legacyBase("NOCTALIA_STATE_HOME", "XDG_STATE_HOME", ".local/state");
    const auto legacyDataBase = legacyBase("NOCTALIA_DATA_HOME", "XDG_DATA_HOME", ".local/share");

    const std::array<DirectoryPair, 3> pairs = {{
        {"config", legacyConfigBase / "noctalia", currentConfigBase / "motion"},
        {"state", legacyStateBase / "noctalia", currentStateBase / "motion"},
        {"data", legacyDataBase / "noctalia", currentDataBase / "motion"},
    }};

    for (const auto& pair : pairs) {
      std::string warning;
      if (copyAtomically(pair, warning)) {
        report.migrated.push_back(
            std::string(pair.label) + ": " + pair.legacy.string() + " -> " + pair.current.string()
        );
      } else if (!warning.empty()) {
        report.warnings.push_back(std::move(warning));
      }
    }

    if (!report.migrated.empty() && !currentStateBase.empty()) {
      std::error_code ec;
      const auto stateDir = currentStateBase / "motion";
      std::filesystem::create_directories(stateDir, ec);
      if (!ec) {
        std::ofstream marker(stateDir / ".migrated-from-noctalia-v1", std::ios::trunc);
        if (marker) {
          marker << "Motion copied the legacy Noctalia profile without modifying its source.\n";
          for (const auto& item : report.migrated) {
            marker << item << '\n';
          }
        }
      }
    }

    return report;
  }

} // namespace motion::legacy
