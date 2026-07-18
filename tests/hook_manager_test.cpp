#include "hooks/hook_manager.h"

#include <cassert>
#include <cstdlib>
#include <string>
#include <string_view>
#include <vector>

std::string_view hookKindKey(HookKind kind) {
  if (kind == HookKind::WallpaperChanged) {
    return "wallpaper_changed";
  }
  return "unknown";
}

namespace {

  const char* envOrEmpty(const char* name) {
    if (const char* value = std::getenv(name)) {
      return value;
    }
    return "";
  }

} // namespace

int main() {
  constexpr const char* kPathName = "MOTION_WALLPAPER_PATH";
  constexpr const char* kConnectorName = "MOTION_WALLPAPER_CONNECTOR";
  constexpr const char* kLegacyPathName = "NOCTALIA_WALLPAPER_PATH";
  constexpr const char* kLegacyConnectorName = "NOCTALIA_WALLPAPER_CONNECTOR";

  ::unsetenv(kPathName);
  ::setenv(kPathName, "preexisting", 1);
  ::unsetenv(kConnectorName);
  ::unsetenv(kLegacyPathName);
  ::setenv(kLegacyConnectorName, "legacy-preexisting", 1);

  HookManager hooks;
  HooksConfig config;
  config.commands[static_cast<std::size_t>(HookKind::WallpaperChanged)] = {"record-wallpaper-hook"};
  hooks.reload(config);

  std::vector<std::string> commands;
  std::string pathSeen;
  std::string connectorSeen;
  std::string legacyPathSeen;
  std::string legacyConnectorSeen;
  hooks.setCommandRunner([&](const std::string& command) {
    commands.push_back(command);
    pathSeen = envOrEmpty(kPathName);
    connectorSeen = envOrEmpty(kConnectorName);
    legacyPathSeen = envOrEmpty(kLegacyPathName);
    legacyConnectorSeen = envOrEmpty(kLegacyConnectorName);
    return true;
  });

  hooks.fire(HookKind::WallpaperChanged, {{kPathName, "/tmp/motion test/wallpaper.png"}, {kConnectorName, "DP-1"}});

  assert(commands.size() == 1);
  assert(commands[0] == "record-wallpaper-hook");
  assert(pathSeen == "/tmp/motion test/wallpaper.png");
  assert(connectorSeen == "DP-1");
  assert(legacyPathSeen == "/tmp/motion test/wallpaper.png");
  assert(legacyConnectorSeen == "DP-1");
  assert(std::string(envOrEmpty(kPathName)) == "preexisting");
  assert(std::getenv(kConnectorName) == nullptr);
  assert(std::getenv(kLegacyPathName) == nullptr);
  assert(std::string(envOrEmpty(kLegacyConnectorName)) == "legacy-preexisting");

  return 0;
}
