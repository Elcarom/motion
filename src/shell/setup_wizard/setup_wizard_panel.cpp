#include "shell/setup_wizard/setup_wizard_panel.h"

#include "config/config_service.h"
#include "core/files/resource_paths.h"
#include "core/log.h"
#include "i18n/i18n.h"
#include "render/core/renderer.h"
#include "shell/panel/panel_manager.h"
#include "ui/builders.h"
#include "ui/dialogs/file_dialog.h"
#include "ui/palette.h"
#include "ui/style.h"

#include <cstdlib>
#include <filesystem>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace {

  constexpr Logger kLog("setup-wizard");

  std::unique_ptr<Label>
  makeLabel(std::string_view text, float fontSize, const ColorSpec& color, FontWeight fontWeight = FontWeight::Normal) {
    return ui::label({
        .text = std::string(text),
        .fontSize = fontSize,
        .fontWeight = fontWeight,
        .color = color,
    });
  }

  std::unique_ptr<Flex> makeCard(float scale, float fillOpacity, bool showBorder) {
    return ui::column(
        {.align = FlexAlign::Stretch,
         .gap = Style::spaceMd * scale,
         .configure = [scale, fillOpacity, showBorder](Flex& card) {
           card.setPadding(Style::spaceMd * scale, Style::spaceLg * scale);
           card.setCardStyle(scale, fillOpacity, showBorder);
         }}
    );
  }

  std::unique_ptr<Flex> makeRow(float scale) {
    return ui::row({
        .align = FlexAlign::Center,
        .justify = FlexJustify::SpaceBetween,
        .gap = Style::spaceMd * scale,
    });
  }

  std::unique_ptr<Flex> makeTextColumn() {
    return ui::column({
        .align = FlexAlign::Start,
        .gap = 2.0f,
        .flexGrow = 1.0f,
    });
  }

} // namespace

bool SetupWizardPanel::isFirstRun(const ConfigService& config) { return config.shouldRunSetupWizard(); }

void SetupWizardPanel::create() {
  const float scale = contentScale();
  const auto& cfg = m_config->config();

  auto root = ui::column(
      {.out = &m_root,
       .align = FlexAlign::Stretch,
       .justify = FlexJustify::SpaceBetween,
       .gap = Style::spaceLg * scale,
       .paddingV = 24.0f * scale,
       .paddingH = 28.0f * scale}
  );

  auto scroll = ui::scrollView({
      .scrollbarVisible = true,
      .viewportPaddingH = 0.0f,
      .viewportPaddingV = 0.0f,
      .flexGrow = 1.0f,
      .configure = [](ScrollView& scrollView) {
        scrollView.clearFill();
        scrollView.clearBorder();
      },
  });

  auto* content = scroll->content();
  content->setDirection(FlexDirection::Vertical);
  content->setAlign(FlexAlign::Stretch);
  content->setGap(Style::spaceLg * scale);

  // Header
  {
    auto header = ui::row({.align = FlexAlign::Center, .gap = Style::spaceMd * scale});

    header->addChild(
        ui::image({
            .out = &m_logo,
            .width = 44.0f * scale,
            .height = 44.0f * scale,
        })
    );

    auto copy = makeTextColumn();
    copy->setGap(Style::spaceXs * scale);
    copy->addChild(makeLabel(
        i18n::tr("setup-wizard.title"), 18.0f * scale, colorSpecFromRole(ColorRole::OnSurface), FontWeight::Bold
    ));
    copy->addChild(makeLabel(
        i18n::tr("setup-wizard.subtitle"), Style::fontSizeBody * scale, colorSpecFromRole(ColorRole::OnSurfaceVariant)
    ));
    header->addChild(std::move(copy));
    content->addChild(std::move(header));
  }

  content->addChild(ui::separator());

  // Privacy
  {
    auto card = makeCard(scale, panelCardOpacity(), panelBordersEnabled());
    auto col = makeTextColumn();
    col->addChild(makeLabel(
        "Local-first by design", Style::fontSizeBody * scale, colorSpecFromRole(ColorRole::OnSurface), FontWeight::Bold
    ));
    auto description = makeLabel(
        "Motion does not send telemetry, analytics, advertising identifiers, or remote logs.",
        Style::fontSizeCaption * scale, colorSpecFromRole(ColorRole::OnSurfaceVariant)
    );
    description->setMaxLines(8);
    col->addChild(std::move(description));
    card->addChild(std::move(col));
    content->addChild(std::move(card));
  }

  // Wallpaper
  {
    auto card = makeCard(scale, panelCardOpacity(), panelBordersEnabled());

    auto row = makeRow(scale);
    {
      auto col = makeTextColumn();
      col->addChild(makeLabel(
          i18n::tr("setup-wizard.wallpaper"), Style::fontSizeBody * scale, colorSpecFromRole(ColorRole::OnSurface),
          FontWeight::Bold
      ));
      const std::string currentPath = m_config->getDefaultWallpaperPath();
      auto pathLabel = makeLabel(
          currentPath.empty() ? i18n::tr("setup-wizard.no-wallpaper-selected") : currentPath,
          Style::fontSizeCaption * scale, colorSpecFromRole(ColorRole::OnSurfaceVariant)
      );
      pathLabel->setMaxWidth(330.0f * scale);
      pathLabel->setMaxLines(1);
      m_wallpaperLabel = pathLabel.get();
      col->addChild(std::move(pathLabel));
      row->addChild(std::move(col));
    }
    {
      row->addChild(
          ui::button({
              .text = i18n::tr("setup-wizard.browse"),
              .glyph = "image",
              .fontSize = Style::fontSizeBody * scale,
              .glyphSize = Style::fontSizeBody * scale,
              .variant = ButtonVariant::Outline,
              .minWidth = 112.0f * scale,
              .minHeight = Style::controlHeight * scale,
              .paddingV = Style::spaceSm * scale,
              .paddingH = Style::spaceMd * scale,
              .radius = Style::scaledRadiusMd(scale),
              .onClick = [this]() {
                FileDialogOptions options;
                options.mode = FileDialogMode::Open;
                options.defaultViewMode = FileDialogViewMode::Grid;
                options.title = i18n::tr("setup-wizard.select-wallpaper");
                options.extensions = {".png", ".jpg", ".jpeg", ".webp", ".bmp", ".gif"};
                std::filesystem::path startDir;
                if (!m_wallpaperDir.empty()) {
                  startDir = m_wallpaperDir;
                } else if (const char* home = std::getenv("HOME"); home != nullptr && home[0] != '\0') {
                  startDir = std::filesystem::path(home) / "Pictures";
                }
                options.startDirectory = std::move(startDir);
                (void)FileDialog::open(std::move(options), [this](std::optional<std::filesystem::path> result) {
                  if (!result.has_value()) {
                    return;
                  }
                  const std::string fullPath = result->string();
                  const std::string parentDir = result->parent_path().string();
                  m_wallpaperDir = parentDir;
                  if (m_wallpaperLabel != nullptr) {
                    m_wallpaperLabel->setText(fullPath);
                    m_wallpaperLabel->setColor(colorSpecFromRole(ColorRole::Primary));
                    m_wallpaperLabel->setMaxLines(1);
                  }
                  m_config->setOverride({"wallpaper", "directory"}, parentDir);
                  m_config->setWallpaperPath(std::nullopt, fullPath);
                });
              },
          })
      );
    }
    card->addChild(std::move(row));
    content->addChild(std::move(card));
  }

  // Theme
  {
    auto card = makeCard(scale, panelCardOpacity(), panelBordersEnabled());

    // Mode row
    {
      auto row = makeRow(scale);
      auto label = makeLabel(
          i18n::tr("setup-wizard.mode"), Style::fontSizeBody * scale, colorSpecFromRole(ColorRole::OnSurface)
      );
      label->setFlexGrow(1.0f);
      row->addChild(std::move(label));

      std::size_t modeIdx = 0;
      if (cfg.theme.mode == ThemeMode::Light) {
        modeIdx = 1;
      } else if (cfg.theme.mode == ThemeMode::Auto) {
        modeIdx = 2;
      }
      row->addChild(
          ui::select({
              .out = &m_modeSelect,
              .options =
                  std::vector<std::string>{
                      i18n::tr("settings.options.theme.mode.dark"), i18n::tr("settings.options.theme.mode.light"),
                      i18n::tr("common.states.auto")
                  },
              .selectedIndex = modeIdx,
              .fontSize = Style::fontSizeBody * scale,
              .controlHeight = Style::controlHeight * scale,
              .horizontalPadding = Style::spaceMd * scale,
              .onSelectionChanged =
                  [this](std::size_t index, std::string_view /*label*/) {
                    static constexpr const char* kModes[] = {"dark", "light", "auto"};
                    if (index < 3) {
                      m_config->setOverride({"theme", "mode"}, std::string(kModes[index]));
                    }
                  },
              .configure = [scale](Select& select) { select.setMinWidth(220.0f * scale); },
          })
      );
      card->addChild(std::move(row));
    }

    content->addChild(std::move(card));
  }

  root->addChild(std::move(scroll));

  // Footer
  {
    auto footer = ui::row({
        .align = FlexAlign::Center,
        .justify = FlexJustify::SpaceBetween,
    });

    footer->addChild(makeLabel(
        i18n::tr("setup-wizard.footer-note"), Style::fontSizeCaption * scale,
        colorSpecFromRole(ColorRole::OnSurfaceVariant)
    ));

    footer->addChild(
        ui::button({
            .text = i18n::tr("setup-wizard.get-started"),
            .glyph = "chevron-right",
            .fontSize = Style::fontSizeBody * scale,
            .glyphSize = Style::fontSizeBody * scale,
            .variant = ButtonVariant::Primary,
            .minWidth = 132.0f * scale,
            .minHeight = Style::controlHeight * scale,
            .paddingV = Style::spaceSm * scale,
            .paddingH = Style::spaceLg * scale,
            .radius = Style::scaledRadiusMd(scale),
            .onClick = [this]() { commit(); },
        })
    );
    root->addChild(std::move(footer));
  }

  setRoot(std::move(root));
}

void SetupWizardPanel::doLayout(Renderer& renderer, float width, float height) {
  if (m_logo != nullptr && !m_logo->hasImage()) {
    m_logo->setSourceFile(renderer, paths::assetPath("motion.svg").string(), 48 * static_cast<int>(contentScale()));
  }
  if (m_root != nullptr) {
    m_root->setPosition(0.0f, 0.0f);
    m_root->setSize(width, height);
    m_root->layout(renderer);
  }
}

void SetupWizardPanel::commit() {
  // Theme/palette overrides are written live by the select callbacks only when
  // the user actually changes them, so commit must not force any defaults here.
  if (m_config != nullptr) {
    (void)m_config->markSetupWizardCompleted();
  }
  kLog.info("setup complete");
  PanelManager::instance().close();
}

void SetupWizardPanel::onClose() {
  if (m_config != nullptr) {
    (void)m_config->markSetupWizardCompleted();
  }
}
