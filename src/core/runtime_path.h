#pragma once

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <filesystem>
#include <string>
#include <sys/stat.h>
#include <unistd.h>

namespace motion::runtime_path {

  [[nodiscard]] inline std::string sanitizedDisplayName() {
    const char* raw = std::getenv("WAYLAND_DISPLAY");
    std::string display = raw != nullptr && raw[0] != '\0' ? raw : "wayland-0";
    for (char& c : display) {
      const unsigned char u = static_cast<unsigned char>(c);
      if (std::isalnum(u) == 0 && c != '-' && c != '_') {
        c = '_';
      }
    }
    if (display.empty()) {
      display = "wayland-0";
    }
    if (display.size() > 64) {
      display.resize(64);
    }
    return display;
  }

  [[nodiscard]] inline std::filesystem::path trustedRuntimeDirectory() {
    const uid_t uid = ::geteuid();
    if (const char* raw = std::getenv("XDG_RUNTIME_DIR"); raw != nullptr && raw[0] != '\0') {
      const std::filesystem::path candidate(raw);
      struct stat st {};
      if (candidate.is_absolute() && ::stat(candidate.c_str(), &st) == 0 && S_ISDIR(st.st_mode) && st.st_uid == uid && (st.st_mode & 0022) == 0) {
        return candidate;
      }
    }
    return std::filesystem::path("/tmp");
  }

  [[nodiscard]] inline std::string path(std::string_view extension) {
    const auto dir = trustedRuntimeDirectory();
    std::string name = "motion-";
    if (dir == std::filesystem::path("/tmp")) {
      name += std::to_string(static_cast<unsigned long long>(::geteuid()));
      name += '-';
    }
    name += sanitizedDisplayName();
    name += '.';
    name += extension;
    return (dir / name).string();
  }

} // namespace motion::runtime_path
