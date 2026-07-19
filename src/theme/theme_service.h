#pragma once

#include "config/config_types.h"
#include "core/timer_manager.h"
#include "render/animation/animation_manager.h"
#include "theme/palette.h"
#include "ui/palette.h"

#include <cstdint>
#include <functional>
#include <optional>
#include <string>
#include <string_view>

class ConfigService;
class IpcService;

namespace motion::theme {

  class ThemeService {
  public:
    using ChangeCallback = std::function<void()>;
    using ResolvedCallback = std::function<void(const GeneratedPalette&, std::string_view)>;

    explicit ThemeService(ConfigService& config);

    void apply();
    void onConfigReload();
    void onWallpaperChange();
    void onAutoModeBoundary();
    void setAutoCoordinates(std::optional<double> latitude, std::optional<double> longitude);
    void toggleLightDark();
    void cycleMode();
    [[nodiscard]] ThemeMode configuredMode() const noexcept;
    [[nodiscard]] bool isLightMode() const noexcept;
    [[nodiscard]] std::string_view resolvedMode() const noexcept;

    void setChangeCallback(ChangeCallback callback);
    void setResolvedCallback(ResolvedCallback callback);
    void registerIpc(IpcService& ipc);

  private:
    void resolveAndSet(bool animate);
    std::optional<GeneratedPalette> resolveWallpaperGenerated(const std::string& wallpaperPath);
    void queueResolvedCallback(const GeneratedPalette& generated, std::string_view mode);
    void flushResolvedCallback(bool defer);
    void startTransition(const Palette& target);
    void finishTransition(bool deferResolvedCallback);
    void tickTransition();
    void rescheduleAutoTimer();

    ConfigService& m_config;

    std::optional<GeneratedPalette> m_wallpaperCacheGenerated;
    std::string m_wallpaperCachePath;
    std::int64_t m_wallpaperCacheMtimeNs = 0;

    ChangeCallback m_changeCallback;
    ResolvedCallback m_resolvedCallback;
    std::optional<GeneratedPalette> m_pendingResolvedPalette;
    std::string m_pendingResolvedMode;
    std::uint64_t m_resolvedCallbackGeneration = 0;

    AnimationManager m_animations;
    Timer m_transitionTimer;
    Palette m_fromPalette{};
    Palette m_targetPalette{};
    AnimationManager::Id m_transitionAnimId = 0;
    bool m_transitionResolvedCallbackFlushed = false;
    bool m_isLightMode = false;
    std::optional<double> m_autoLatitude;
    std::optional<double> m_autoLongitude;
    Timer m_autoTimer;
  };

} // namespace motion::theme
