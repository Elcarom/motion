#include "util/file_utils.h"

#include <cassert>
#include <cstdlib>
#include <filesystem>
#include <string>
#include <sys/stat.h>
#include <unistd.h>

namespace {
  void clearOverrides() {
    for (const char* name : {
             "MOTION_CONFIG_HOME",
             "MOTION_STATE_HOME",
             "MOTION_DATA_HOME",
             "MOTION_CACHE_HOME",
             "NOCTALIA_CONFIG_HOME",
             "NOCTALIA_STATE_HOME",
             "NOCTALIA_DATA_HOME",
             "NOCTALIA_CACHE_HOME",
             "XDG_CONFIG_HOME",
             "XDG_STATE_HOME",
             "XDG_DATA_HOME",
             "XDG_CACHE_HOME",
             "HOME",
         }) {
      ::unsetenv(name);
    }
  }
} // namespace

int main() {
  clearOverrides();
  ::setenv("HOME", "/home/motion-test", 1);
  assert(FileUtils::configDir() == "/home/motion-test/.config/motion");
  assert(FileUtils::stateDir() == "/home/motion-test/.local/state/motion");
  assert(FileUtils::dataDir() == "/home/motion-test/.local/share/motion");
  assert(FileUtils::cacheDir() == "/home/motion-test/.cache/motion");

  ::setenv("MOTION_CACHE_HOME", "/current-cache", 1);
  ::setenv("NOCTALIA_CACHE_HOME", "/legacy-cache", 1);
  assert(FileUtils::cacheDir() == "/current-cache/motion");

  ::unsetenv("MOTION_CACHE_HOME");
  assert(FileUtils::cacheDir() == "/legacy-cache/motion");

  clearOverrides();
  const std::filesystem::path fallback = FileUtils::cacheDir();
  assert(!fallback.empty());
  struct stat st{};
  const std::filesystem::path privateRoot = fallback.filename() == "cache" ? fallback.parent_path() : fallback;
  assert(::lstat(privateRoot.c_str(), &st) == 0);
  assert(S_ISDIR(st.st_mode));
  assert(st.st_uid == ::geteuid());
  assert((st.st_mode & 0077) == 0);
  return 0;
}
