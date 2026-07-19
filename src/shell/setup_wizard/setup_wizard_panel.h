#pragma once

#include "shell/panel/panel.h"

#include <cstdint>
#include <string>

class ConfigService;
class Flex;
class Image;
class Label;
class Select;
class WaylandConnection;

class SetupWizardPanel : public Panel {
public:
  SetupWizardPanel(ConfigService* config, WaylandConnection* /*wayland*/) : m_config(config) {}

  void create() override;
  void onClose() override;

  [[nodiscard]] float preferredWidth() const override { return scaled(620.0f); }
  [[nodiscard]] float preferredHeight() const override { return scaled(580.0f); }
  [[nodiscard]] bool hasDecoration() const override { return true; }
  [[nodiscard]] LayerShellLayer layer() const override { return LayerShellLayer::Overlay; }
  [[nodiscard]] LayerShellKeyboard keyboardMode() const override { return LayerShellKeyboard::OnDemand; }

  static bool isFirstRun(const ConfigService& config);

private:
  void doLayout(Renderer& renderer, float width, float height) override;
  void commit();

  ConfigService* m_config = nullptr;
  Flex* m_root = nullptr;
  Image* m_logo = nullptr;
  Select* m_modeSelect = nullptr;
  Label* m_wallpaperLabel = nullptr;
  std::string m_wallpaperDir;
};
