#include "core/runtime_path.h"

#include <cassert>
#include <cstdlib>
#include <filesystem>
#include <string>
#include <sys/stat.h>
#include <unistd.h>

int main() {
  const auto base = std::filesystem::temp_directory_path()
      / ("motion-runtime-path-test-" + std::to_string(static_cast<unsigned long long>(::getpid())));
  std::filesystem::remove_all(base);
  std::filesystem::create_directories(base);
  ::chmod(base.c_str(), 0700);

  ::setenv("XDG_RUNTIME_DIR", base.c_str(), 1);
  ::setenv("WAYLAND_DISPLAY", "../../wayland socket", 1);
  const std::filesystem::path trusted = motion::runtime_path::trustedRuntimeDirectory();
  assert(trusted == base);
  const std::filesystem::path safePath = motion::runtime_path::path("sock");
  assert(safePath.parent_path() == base);
  assert(safePath.filename().string().find('/') == std::string::npos);
  assert(safePath.filename().string().find("..") == std::string::npos);

  ::chmod(base.c_str(), 0777);
  assert(motion::runtime_path::trustedRuntimeDirectory() == std::filesystem::path("/tmp"));
  const std::filesystem::path fallback = motion::runtime_path::path("lock");
  assert(fallback.parent_path() == std::filesystem::path("/tmp"));
  assert(fallback.filename().string().starts_with(
      "motion-" + std::to_string(static_cast<unsigned long long>(::geteuid())) + "-"
  ));

  std::filesystem::remove_all(base);
  return 0;
}
