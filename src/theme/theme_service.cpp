#include "theme/theme_service.h"

#include "config/config_service.h"
#include "core/deferred_call.h"
#include "core/files/resource_paths.h"
#include "core/log.h"
#include "core/scoped_timer.h"
#include "ipc/ipc_service.h"
#include "system/day_night_schedule.h"
#include "theme/image_loader.h"
#include "theme/palette_generator.h"
#include "theme/palette_mapping.h"
#include "theme/palette_transform.h"
#include "ui/app_icon_colorization.h"
#include "ui/design_tokens.h"
#include "util/string_utils.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <filesystem>
#include <string>
#include <string_view>
#include <system_error>
#include <utility>

namespace motion::theme {

  namespace {

    constexpr auto kLog = Logger("theme");
    constexpr float kTransitionDurationMs = static_cast<float>(motion::design::motion::longEnter);
    constexpr std::chrono::milliseconds kTransitionTick{8};

    std::string resolvedModeName(
        const ThemeConfig& cfg, const LocationConfig& location, std::optional<double> latitude,
        std::optional<double> longitude
    ) {
      if (cfg.mode == ThemeMode::Auto) {
        const auto eval = day_night_schedule::evaluate(location, latitude, longitude);
        return eval.night ? "dark" : "light";
      }
      return cfg.mode == ThemeMode::Light ? "light" : "dark";
    }

  } // namespace

  ThemeService::ThemeService(ConfigService& config) : m_config(config) {}

  void ThemeService::apply() { resolveAndSet(false); }

  void ThemeService::onConfigReload() { resolveAndSet(true); }

  void ThemeService::onWallpaperChange() { resolveAndSet(true); }

  void ThemeService::onAutoModeBoundary() {
    if (m_config.config().theme.mode == ThemeMode::Auto) {
      resolveAndSet(true);
    }
  }

  void ThemeService::setAutoCoordinates(std::optional<double> latitude, std::optional<double> longitude) {
    if (latitude.has_value() && !std::isfinite(*latitude)) {
      latitude.reset();
    }
    if (longitude.has_value() && !std::isfinite(*longitude)) {
      longitude.reset();
    }
    if (m_autoLatitude == latitude && m_autoLongitude == longitude) {
      return;
    }
    m_autoLatitude = latitude;
    m_autoLongitude = longitude;
    if (m_config.config().theme.mode == ThemeMode::Auto) {
      resolveAndSet(true);
    }
  }

  void ThemeService::toggleLightDark() { m_config.setThemeMode(m_isLightMode ? ThemeMode::Dark : ThemeMode::Light); }

  void ThemeService::cycleMode() {
    ThemeMode next = ThemeMode::Dark;
    switch (configuredMode()) {
    case ThemeMode::Dark:
      next = ThemeMode::Light;
      break;
    case ThemeMode::Light:
      next = ThemeMode::Auto;
      break;
    case ThemeMode::Auto:
      next = ThemeMode::Dark;
      break;
    }
    m_config.setThemeMode(next);
  }

  ThemeMode ThemeService::configuredMode() const noexcept { return m_config.config().theme.mode; }

  bool ThemeService::isLightMode() const noexcept { return m_isLightMode; }

  std::string_view ThemeService::resolvedMode() const noexcept { return m_isLightMode ? "light" : "dark"; }

  void ThemeService::setChangeCallback(ChangeCallback callback) { m_changeCallback = std::move(callback); }

  void ThemeService::setResolvedCallback(ResolvedCallback callback) { m_resolvedCallback = std::move(callback); }

  void ThemeService::queueResolvedCallback(const GeneratedPalette& generated, std::string_view mode) {
    if (!m_resolvedCallback) {
      return;
    }
    m_pendingResolvedPalette = generated;
    m_pendingResolvedMode = std::string(mode);
    ++m_resolvedCallbackGeneration;
  }

  void ThemeService::flushResolvedCallback(bool defer) {
    if (!m_resolvedCallback || !m_pendingResolvedPalette.has_value()) {
      return;
    }

    GeneratedPalette generated = std::move(*m_pendingResolvedPalette);
    std::string mode = std::move(m_pendingResolvedMode);
    const std::uint64_t generation = m_resolvedCallbackGeneration;
    m_pendingResolvedPalette.reset();
    m_pendingResolvedMode.clear();

    if (defer) {
      DeferredCall::callLater([this, generation, generated = std::move(generated), mode = std::move(mode)]() mutable {
        if (generation == m_resolvedCallbackGeneration && m_resolvedCallback) {
          m_resolvedCallback(generated, mode);
        }
      });
      return;
    }

    m_resolvedCallback(generated, mode);
  }

  std::optional<GeneratedPalette> ThemeService::resolveWallpaperGenerated(const std::string& wallpaperPath) {
    if (wallpaperPath.empty()) {
      kLog.warn("cannot derive the M3 Expressive palette: no wallpaper path is available");
      return std::nullopt;
    }

    std::error_code ec;
    const auto writeTime = std::filesystem::last_write_time(wallpaperPath, ec);
    const std::int64_t mtimeNs = ec ? 0 : writeTime.time_since_epoch().count();
    if (mtimeNs != 0
        && m_wallpaperCacheGenerated.has_value()
        && m_wallpaperCachePath == wallpaperPath
        && m_wallpaperCacheMtimeNs == mtimeNs) {
      return m_wallpaperCacheGenerated;
    }

    profiling::StopWatch loadWatch;
    auto image = loadAndResize(wallpaperPath);
    if (profiling::enabled()) {
      kLog.info("theme: wallpaper load+resize: {:.1f} ms", loadWatch.elapsedMs());
    }
    if (!image) {
      kLog.warn("failed to load wallpaper '{}': {}", wallpaperPath, image.error());
      return std::nullopt;
    }

    profiling::StopWatch genWatch;
    auto generated = generate(image->rgb);
    if (profiling::enabled()) {
      kLog.info("theme: M3 Expressive palette generate: {:.1f} ms", genWatch.elapsedMs());
    }
    if (!generated) {
      kLog.warn("failed to generate M3 Expressive palette from wallpaper: {}", generated.error());
      return std::nullopt;
    }

    if (mtimeNs != 0) {
      m_wallpaperCacheGenerated = *generated;
      m_wallpaperCachePath = wallpaperPath;
      m_wallpaperCacheMtimeNs = mtimeNs;
    }
    return *generated;
  }

  void ThemeService::resolveAndSet(bool animate) {
    profiling::ScopedTimer timer(kLog, "theme: resolveAndSet");
    const auto& cfg = m_config.config().theme;
    const std::string mode = resolvedModeName(cfg, m_config.config().location, m_autoLatitude, m_autoLongitude);

    std::string wallpaperPath = m_config.getPaletteWallpaperPath();
    auto generated = resolveWallpaperGenerated(wallpaperPath);
    if (!generated.has_value()) {
      const auto bundled = paths::assetPath("motion-wallpaper.png");
      if (bundled.string() != wallpaperPath) {
        generated = resolveWallpaperGenerated(bundled.string());
      }
    }
    if (!generated.has_value()) {
      kLog.error("no wallpaper could provide an M3 Expressive palette; retaining the current palette");
      return;
    }

    if (m_config.config().accessibility.highContrast) {
      applyHighContrast(*generated);
    }

    const Palette resolvedPalette = mapGeneratedPaletteMode(mode == "light" ? generated->light : generated->dark);
    queueResolvedCallback(*generated, mode);
    m_isLightMode = mode == "light";

    if (animate) {
      setResolvedThemeLight(m_isLightMode);
      notifyShellAppIconColorizationChanged();
      startTransition(resolvedPalette);
    } else {
      if (m_transitionAnimId == 0 && palette == resolvedPalette) {
        flushResolvedCallback(false);
        return;
      }
      if (m_transitionAnimId != 0) {
        m_animations.cancel(m_transitionAnimId);
        m_transitionAnimId = 0;
      }
      m_transitionTimer.stop();
      setResolvedThemeLight(m_isLightMode);
      notifyShellAppIconColorizationChanged();
      setPalette(resolvedPalette);
      if (m_changeCallback) {
        m_changeCallback();
      }
      flushResolvedCallback(false);
    }
    rescheduleAutoTimer();
  }

  void ThemeService::rescheduleAutoTimer() {
    m_autoTimer.stop();
    if (m_config.config().theme.mode != ThemeMode::Auto) {
      return;
    }
    constexpr auto kAutoRecheckInterval = std::chrono::minutes(1);
    const auto nextBoundary =
        day_night_schedule::evaluate(m_config.config().location, m_autoLatitude, m_autoLongitude).untilBoundary;
    const auto delay =
        std::min(nextBoundary, std::chrono::duration_cast<std::chrono::milliseconds>(kAutoRecheckInterval));
    m_autoTimer.start(delay, [this]() { onAutoModeBoundary(); });
  }

  void ThemeService::startTransition(const Palette& target) {
    if (m_transitionAnimId == 0 && palette == target) {
      flushResolvedCallback(false);
      return;
    }
    if (m_transitionAnimId != 0 && m_targetPalette == target) {
      flushResolvedCallback(true);
      return;
    }
    if (m_transitionAnimId != 0) {
      m_animations.cancel(m_transitionAnimId);
      m_transitionAnimId = 0;
    }
    m_fromPalette = palette;
    m_targetPalette = target;
    m_transitionResolvedCallbackFlushed = false;
    m_transitionAnimId = m_animations.animate(
        0.0f, 1.0f, kTransitionDurationMs, Easing::EaseOutCubic,
        [this](float progress) {
          setPalette(lerpPalette(m_fromPalette, m_targetPalette, progress));
          if (m_changeCallback) {
            m_changeCallback();
          }
          if (!m_transitionResolvedCallbackFlushed) {
            m_transitionResolvedCallbackFlushed = true;
            flushResolvedCallback(true);
          }
        },
        [this]() { finishTransition(false); }
    );
    if (m_transitionAnimId == 0) {
      m_transitionTimer.stop();
      return;
    }
    m_transitionTimer.startRepeating(kTransitionTick, [this] { tickTransition(); });
  }

  void ThemeService::finishTransition(bool deferResolvedCallback) {
    m_transitionAnimId = 0;
    m_transitionTimer.stop();
    setPalette(m_targetPalette);
    if (m_changeCallback) {
      m_changeCallback();
    }
    flushResolvedCallback(deferResolvedCallback);
  }

  void ThemeService::tickTransition() {
    if (!m_animations.hasActive()) {
      m_transitionTimer.stop();
      return;
    }
    m_animations.tick(static_cast<float>(kTransitionTick.count()));
  }

  void ThemeService::registerIpc(IpcService& ipc) {
    ipc.registerHandler(
        "theme-mode-toggle",
        [this](const std::string&) -> std::string {
          toggleLightDark();
          return "ok\n";
        },
        "theme-mode-toggle", "Toggle theme mode between dark and light"
    );
    ipc.registerHandler(
        "theme-mode-get",
        [this](const std::string&) -> std::string {
          std::string out(resolvedMode());
          out.push_back('\n');
          return out;
        },
        "theme-mode-get", "Print the current resolved theme mode"
    );
    ipc.registerHandler(
        "theme-mode-set",
        [this](const std::string& args) -> std::string {
          const std::string token = StringUtils::trim(args);
          const auto mode = enumFromKey(kThemeModes, token);
          if (!mode.has_value()) {
            return "error: expected dark, light, or auto\n";
          }
          m_config.setThemeMode(*mode);
          return "ok\n";
        },
        "theme-mode-set <dark|light|auto>", "Set the wallpaper-derived M3 Expressive palette mode"
    );
  }

} // namespace motion::theme
