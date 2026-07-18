#include "core/files/resource_paths.h"

#include "core/log.h"

#include <algorithm>
#include <array>
#include <cstdlib>
#include <optional>
#include <string>
#include <system_error>
#include <unistd.h>
#include <vector>

namespace paths {

  namespace {

    constexpr Logger kLog("paths");

    std::filesystem::path installedAssetsRoot() {
      const std::filesystem::path datadir(MOTION_INSTALL_DATADIR);
      if (datadir.is_absolute()) {
        return datadir / "motion" / "assets";
      }
      return std::filesystem::path(MOTION_INSTALL_PREFIX) / datadir / "motion" / "assets";
    }

    std::filesystem::path sourceAssetsRoot() { return std::filesystem::path(MOTION_SOURCE_ASSETS_DIR); }

    bool isAssetRoot(const std::filesystem::path& root) {
      if (root.empty()) {
        return false;
      }

      std::error_code ec;
      return std::filesystem::exists(root / "emoji.json", ec)
          && std::filesystem::exists(root / "fonts" / "tabler.ttf", ec)
          && std::filesystem::exists(root / "templates" / "builtin.toml", ec)
          && std::filesystem::exists(root / "translations" / "en.json", ec);
    }

    std::optional<std::filesystem::path> executablePath() {
      std::array<char, 4096> buffer{};
      const ssize_t count = ::readlink("/proc/self/exe", buffer.data(), buffer.size() - 1);
      if (count <= 0 || static_cast<std::size_t>(count) >= buffer.size() - 1) {
        return std::nullopt;
      }

      buffer[static_cast<std::size_t>(count)] = '\0';
      return std::filesystem::path(buffer.data());
    }

    void appendUnique(std::vector<std::filesystem::path>& candidates, const std::filesystem::path& candidate) {
      if (candidate.empty()) {
        return;
      }
      if (!std::ranges::contains(candidates, candidate)) {
        candidates.push_back(candidate);
      }
    }

    std::vector<std::filesystem::path> assetCandidates() {
      std::vector<std::filesystem::path> candidates;

      const char* assetsEnv = std::getenv("MOTION_ASSETS_DIR");
      const char* assetsEnvName = "MOTION_ASSETS_DIR";
      if (assetsEnv == nullptr || assetsEnv[0] == '\0') {
        assetsEnv = std::getenv("NOCTALIA_ASSETS_DIR");
        assetsEnvName = "NOCTALIA_ASSETS_DIR";
      }
      if (assetsEnv != nullptr && assetsEnv[0] != '\0') {
        const std::filesystem::path overridePath(assetsEnv);
        if (isAssetRoot(overridePath)) {
          candidates.push_back(overridePath);
          return candidates;
        }
        kLog.warn("{} is not a valid asset bundle: {}", assetsEnvName, overridePath.string());
      }

      if (auto exe = executablePath()) {
        const std::filesystem::path exeDir = exe->parent_path();
        appendUnique(candidates, exeDir / "assets");
        appendUnique(candidates, exeDir.parent_path() / "assets");

        const std::filesystem::path datadir(MOTION_INSTALL_DATADIR);
        if (!datadir.empty() && !datadir.is_absolute()) {
          appendUnique(candidates, exeDir.parent_path() / datadir / "motion" / "assets");
        }
        appendUnique(candidates, exeDir.parent_path() / "share" / "motion" / "assets");
      }

      appendUnique(candidates, installedAssetsRoot());
      appendUnique(candidates, sourceAssetsRoot());
      return candidates;
    }

    std::filesystem::path resolveAssetsRoot() {
      for (const auto& candidate : assetCandidates()) {
        if (isAssetRoot(candidate)) {
          kLog.debug("using assets from {}", candidate.string());
          return candidate;
        }
      }

      const std::filesystem::path fallback = installedAssetsRoot();
      kLog.warn("could not locate a valid asset bundle; defaulting to {}", fallback.string());
      return fallback;
    }

  } // namespace

  const std::filesystem::path& assetsRoot() {
    static const std::filesystem::path s_root = resolveAssetsRoot();
    return s_root;
  }

  std::filesystem::path assetPath(std::string_view relativePath) {
    return assetsRoot() / std::filesystem::path(std::string(relativePath));
  }

} // namespace paths
