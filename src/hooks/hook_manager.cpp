#include "hooks/hook_manager.h"

#include "core/log.h"

#include <cstdlib>
#include <optional>
#include <string>
#include <vector>

namespace {

  constexpr Logger kLog("hooks");

} // namespace

void HookManager::setCommandRunner(CommandRunner runner) { m_runner = std::move(runner); }

void HookManager::setBlockingCommandRunner(CommandRunner runner) { m_blockingRunner = std::move(runner); }

void HookManager::reload(const HooksConfig& config) { m_config = config; }

void HookManager::fire(HookKind kind) const { (void)fireWithRunner(kind, m_runner); }

bool HookManager::fireBlocking(HookKind kind) const {
  return fireWithRunner(kind, m_blockingRunner ? m_blockingRunner : m_runner);
}

bool HookManager::fireWithRunner(HookKind kind, const CommandRunner& runner) const {
  if (kind == HookKind::Count || !runner) {
    return false;
  }
  const auto& cmds = m_config.commands[static_cast<std::size_t>(kind)];
  if (cmds.empty()) {
    return true;
  }
  const std::string_view name = hookKindKey(kind);
  kLog.debug("hook '{}' running {} command(s)", name, cmds.size());
  bool ok = true;
  for (const auto& cmd : cmds) {
    if (!runner(cmd)) {
      kLog.warn("hook '{}' command failed: {}", name, cmd);
      ok = false;
    }
  }
  return ok;
}

void HookManager::fire(HookKind kind, std::initializer_list<EnvVar> env) const {
  fireWithEnv(kind, std::span<const EnvVar>(env.begin(), env.size()));
}

void HookManager::fire(HookKind kind, const std::vector<EnvVar>& env) const { fireWithEnv(kind, env); }

void HookManager::fireWithEnv(HookKind kind, std::span<const EnvVar> env) const {
  struct SavedValue {
    std::string key;
    std::optional<std::string> value;
  };

  std::vector<SavedValue> saved;
  saved.reserve(env.size() * 2);
  const auto setTemporarily = [&saved](std::string key, const std::string& value) {
    if (const char* previous = ::getenv(key.c_str()); previous != nullptr) {
      saved.push_back({key, std::string(previous)});
    } else {
      saved.push_back({key, std::nullopt});
    }
    (void)::setenv(key.c_str(), value.c_str(), 1);
  };

  for (const auto& [key, value] : env) {
    const std::string currentKey(key);
    setTemporarily(currentKey, value);
    if (currentKey.starts_with("MOTION_")) {
      setTemporarily("NOCTALIA_" + currentKey.substr(7), value);
    }
  }

  fire(kind);

  for (auto it = saved.rbegin(); it != saved.rend(); ++it) {
    if (it->value.has_value()) {
      (void)::setenv(it->key.c_str(), it->value->c_str(), 1);
    } else {
      (void)::unsetenv(it->key.c_str());
    }
  }
}
