#include "core/legacy_migration.h"

#include <cassert>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <string>
#include <unistd.h>

namespace {

  void writeFile(const std::filesystem::path& path, std::string_view content) {
    std::filesystem::create_directories(path.parent_path());
    std::ofstream out(path, std::ios::trunc);
    assert(out.is_open());
    out << content;
  }

  std::string readFile(const std::filesystem::path& path) {
    std::ifstream in(path);
    return {std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>()};
  }

} // namespace

int main() {
  const auto root = std::filesystem::temp_directory_path()
      / ("motion-legacy-migration-test-" + std::to_string(static_cast<long long>(::getpid())));
  std::filesystem::remove_all(root);

  const auto configBase = root / "config";
  const auto stateBase = root / "state";
  const auto dataBase = root / "data";
  ::setenv("MOTION_CONFIG_HOME", configBase.c_str(), 1);
  ::setenv("MOTION_STATE_HOME", stateBase.c_str(), 1);
  ::setenv("MOTION_DATA_HOME", dataBase.c_str(), 1);
  ::setenv("NOCTALIA_CONFIG_HOME", configBase.c_str(), 1);
  ::setenv("NOCTALIA_STATE_HOME", stateBase.c_str(), 1);
  ::setenv("NOCTALIA_DATA_HOME", dataBase.c_str(), 1);

  writeFile(configBase / "noctalia/config.toml", "[theme]\nmode = \"dark\"\n");
  writeFile(stateBase / "noctalia/state.toml", "selected = 3\n");
  writeFile(dataBase / "noctalia/palettes/legacy.json", "{}\n");

  const auto first = motion::legacy::migrateNoctaliaProfile();
  assert(first.warnings.empty());
  assert(first.migrated.size() == 3);
  assert(readFile(configBase / "motion/config.toml").find("dark") != std::string::npos);
  assert(readFile(stateBase / "motion/state.toml").find("3") != std::string::npos);
  assert(std::filesystem::exists(dataBase / "motion/palettes/legacy.json"));
  assert(std::filesystem::exists(stateBase / "motion/.migrated-from-noctalia-v1"));

  // Source remains untouched and a second pass never overwrites Motion data.
  writeFile(configBase / "motion/config.toml", "[theme]\nmode = \"light\"\n");
  const auto second = motion::legacy::migrateNoctaliaProfile();
  assert(second.migrated.empty());
  assert(second.warnings.empty());
  assert(readFile(configBase / "motion/config.toml").find("light") != std::string::npos);
  assert(std::filesystem::exists(configBase / "noctalia/config.toml"));

  std::filesystem::remove_all(root);
  return 0;
}
