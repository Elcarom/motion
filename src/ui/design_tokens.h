#pragma once

// Motion's desktop-adapted Material 3 Expressive design tokens.
//
// Values are logical pixels and milliseconds. Components should consume these
// semantic roles instead of scattering literals. Compact desktop density is
// intentional: minimum pointer targets remain 32 px, while touch-capable
// surfaces can opt into the 44 px touch target.
namespace motion::design {

  namespace spacing {
    inline constexpr float none = 0.0f;
    inline constexpr float xxs = 2.0f;
    inline constexpr float xs = 4.0f;
    inline constexpr float sm = 8.0f;
    inline constexpr float md = 12.0f;
    inline constexpr float lg = 16.0f;
    inline constexpr float xl = 24.0f;
    inline constexpr float xxl = 32.0f;
    inline constexpr float xxxl = 40.0f;
  } // namespace spacing

  namespace shape {
    inline constexpr float none = 0.0f;
    inline constexpr float extraSmall = 4.0f;
    inline constexpr float small = 8.0f;
    inline constexpr float medium = 12.0f;
    inline constexpr float large = 16.0f;
    inline constexpr float extraLarge = 24.0f;
    inline constexpr float full = 999.0f;

    // Semantic geometry. These aliases intentionally contrast dense desktop
    // controls with the larger shapes reserved for important shell surfaces.
    inline constexpr float compactControl = small;
    inline constexpr float standardControl = medium;
    inline constexpr float prominentControl = 20.0f;
    inline constexpr float selection = large;
    inline constexpr float card = 20.0f;
    inline constexpr float menu = large;
    inline constexpr float popup = 28.0f;
    inline constexpr float panel = 32.0f;
    inline constexpr float dialog = 28.0f;
    inline constexpr float circular = full;
  } // namespace shape

  namespace type {
    struct Token {
      float size;
      float lineHeight;
      float letterSpacing;
    };

    inline constexpr Token labelSmallToken{11.0f, 16.0f, 0.5f};
    inline constexpr Token labelMediumToken{12.0f, 16.0f, 0.5f};
    inline constexpr Token labelLargeToken{14.0f, 20.0f, 0.1f};
    inline constexpr Token bodySmallToken{13.0f, 18.0f, 0.4f};
    inline constexpr Token bodyMediumToken{14.0f, 20.0f, 0.25f};
    inline constexpr Token bodyLargeToken{16.0f, 24.0f, 0.15f};
    inline constexpr Token titleSmallToken{16.0f, 22.0f, 0.1f};
    inline constexpr Token titleMediumToken{18.0f, 24.0f, 0.0f};
    inline constexpr Token titleLargeToken{22.0f, 28.0f, 0.0f};
    inline constexpr Token headlineSmallToken{24.0f, 32.0f, 0.0f};
    inline constexpr Token headlineMediumToken{28.0f, 36.0f, 0.0f};
    inline constexpr Token displaySmallToken{36.0f, 44.0f, 0.0f};
    inline constexpr Token displayMediumToken{45.0f, 52.0f, 0.0f};

    // Size-only aliases keep existing consumers source-compatible while the
    // richer tokens are used by controls that own wrapping and baselines.
    inline constexpr float labelSmall = labelSmallToken.size;
    inline constexpr float labelMedium = labelMediumToken.size;
    inline constexpr float labelLarge = labelLargeToken.size;
    inline constexpr float bodySmall = bodySmallToken.size;
    inline constexpr float bodyMedium = bodyMediumToken.size;
    inline constexpr float bodyLarge = bodyLargeToken.size;
    inline constexpr float titleSmall = titleSmallToken.size;
    inline constexpr float titleMedium = titleMediumToken.size;
    inline constexpr float titleLarge = titleLargeToken.size;
    inline constexpr float headlineSmall = headlineSmallToken.size;
    inline constexpr float headlineMedium = headlineMediumToken.size;
    inline constexpr float displaySmall = displaySmallToken.size;
    inline constexpr float displayMedium = displayMediumToken.size;
  } // namespace type

  namespace size {
    inline constexpr float pointerTargetMinimum = 32.0f;
    inline constexpr float touchTargetMinimum = 44.0f;
    inline constexpr float controlCompact = 32.0f;
    inline constexpr float controlStandard = 40.0f;
    inline constexpr float controlProminent = 48.0f;
    inline constexpr float iconSmall = 16.0f;
    inline constexpr float iconMedium = 20.0f;
    inline constexpr float iconLarge = 24.0f;
    inline constexpr float iconExtraLarge = 32.0f;
    inline constexpr float barCompact = 32.0f;
    inline constexpr float barStandard = 48.0f;
    inline constexpr float dockItem = 44.0f;
    inline constexpr float menuItem = 36.0f;
    inline constexpr float listRow = 40.0f;
    inline constexpr float listRowTwoLine = 52.0f;
    inline constexpr float panelCompact = 320.0f;
    inline constexpr float panelStandard = 392.0f;
    inline constexpr float searchHero = 56.0f;
    inline constexpr float quickSettingTile = 72.0f;
    inline constexpr float launcherTile = 148.0f;
    inline constexpr float launcherTileMinWidth = 128.0f;
  } // namespace size

  namespace border {
    inline constexpr float hairline = 1.0f;
    inline constexpr float strong = 2.0f;
    inline constexpr float focus = 2.0f;
    inline constexpr float focusHighContrast = 3.0f;
  } // namespace border

  namespace state {
    inline constexpr float hovered = 0.08f;
    inline constexpr float focused = 0.12f;
    inline constexpr float pressed = 0.12f;
    inline constexpr float dragged = 0.16f;
    inline constexpr float disabledContent = 0.38f;
    inline constexpr float disabledContainer = 0.12f;
    inline constexpr float disabledOutline = 0.50f;
    inline constexpr float selected = 0.12f;
  } // namespace state

  namespace elevation {
    inline constexpr float level0 = 0.0f;
    inline constexpr float level1 = 1.0f;
    inline constexpr float level2 = 3.0f;
    inline constexpr float level3 = 6.0f;
    inline constexpr float level4 = 8.0f;
    inline constexpr float level5 = 12.0f;
  } // namespace elevation

  namespace motion {
    inline constexpr int instant = 0;
    inline constexpr int fast = 90;
    inline constexpr int shortEnter = 160;
    inline constexpr int standard = 220;
    inline constexpr int emphasized = 320;
    inline constexpr int longEnter = 420;

    enum class Intent {
      Feedback,
      StateChange,
      EnterExit,
      EmphasizedSpatial,
    };

    [[nodiscard]] constexpr int duration(Intent intent, bool reduced = false) noexcept {
      if (reduced) {
        return instant;
      }
      switch (intent) {
      case Intent::Feedback:
        return fast;
      case Intent::StateChange:
        return standard;
      case Intent::EnterExit:
        return emphasized;
      case Intent::EmphasizedSpatial:
        return longEnter;
      }
      return standard;
    }

    // Expressive spring presets. The renderer currently uses curve-based
    // interpolation; these values are retained as centralized intent for
    // spring-capable components and future physics integration.
    struct Spring {
      float dampingRatio;
      float stiffness;
    };
    inline constexpr Spring spatial{.dampingRatio = 0.72f, .stiffness = 420.0f};
    inline constexpr Spring effects{.dampingRatio = 0.86f, .stiffness = 520.0f};
  } // namespace motion

  namespace breakpoint {
    inline constexpr float compact = 600.0f;
    inline constexpr float medium = 840.0f;
    inline constexpr float expanded = 1200.0f;
  } // namespace breakpoint

  namespace density {
    inline constexpr float compactScale = 0.90f;
    inline constexpr float standardScale = 1.0f;
    inline constexpr float comfortableScale = 1.12f;
  } // namespace density

} // namespace motion::design
