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
  } // namespace spacing

  namespace shape {
    inline constexpr float none = 0.0f;
    inline constexpr float extraSmall = 4.0f;
    inline constexpr float small = 8.0f;
    inline constexpr float medium = 12.0f;
    inline constexpr float large = 16.0f;
    inline constexpr float extraLarge = 24.0f;
    inline constexpr float full = 999.0f;
  } // namespace shape

  namespace type {
    inline constexpr float labelSmall = 11.0f;
    inline constexpr float labelMedium = 12.0f;
    inline constexpr float bodySmall = 13.0f;
    inline constexpr float bodyMedium = 14.0f;
    inline constexpr float titleSmall = 16.0f;
    inline constexpr float titleMedium = 18.0f;
    inline constexpr float headlineSmall = 22.0f;
    inline constexpr float headlineMedium = 28.0f;
    inline constexpr float displaySmall = 36.0f;
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
  } // namespace size

  namespace state {
    inline constexpr float hovered = 0.08f;
    inline constexpr float focused = 0.12f;
    inline constexpr float pressed = 0.12f;
    inline constexpr float dragged = 0.16f;
    inline constexpr float disabledContent = 0.38f;
    inline constexpr float disabledContainer = 0.12f;
    inline constexpr float disabledOutline = 0.50f;
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

} // namespace motion::design
