# Motion design system

## Purpose

Motion adapts Material 3 Expressive principles to a high-density Linux desktop shell. It does not import Android UI
widgets, depend on Android-only libraries, or claim official Material compliance. The goal is a coherent visual and
motion language across pointer-, keyboard-, touch-, and multi-monitor shell surfaces.

## Token architecture

`src/ui/design_tokens.h` is the canonical source for non-color design constants. `src/ui/style.h` is the compatibility
facade used by the existing controls and shell implementation. More than 160 source files consume `Style::` roles, so
routing that facade through the Motion tokens applies the density, shape, type, and motion system without duplicating
constants in every surface.

Token groups:

| Group | Examples | Intent |
|---|---|---|
| Spacing | 2, 4, 8, 12, 16, 24, 32 | Compact desktop rhythm and grouping |
| Shape | 4, 8, 12, 16, 24, full | Contrasting hierarchy, not one radius everywhere |
| Type | label 11–12, body 13–14, title 16–18, headline 22–28, display 36 | Readable at desktop density and scale |
| Size | 32 compact, 40 standard, 48 prominent, 44 touch minimum | Pointer efficiency with touch-safe opt-in |
| State layers | hover 8%, focus/pressed 12%, dragged 16% | Consistent interaction feedback |
| Elevation | 0, 1, 3, 6, 8, 12 | Logical hierarchy for flat/raised/floating surfaces |
| Motion | 90, 160, 220, 320, 420 ms | Fast feedback, standard transitions, emphasized entrances |
| Springs | spatial and effects presets | Central physics intent for spring-capable components |
| Breakpoints | 600, 840, 1200 logical px | Compact, medium, and expanded layout decisions |

New code should use semantic roles. A literal is acceptable only when it describes an external protocol, physical image
size, algorithm threshold, or surface-specific constraint that cannot be expressed by a shared token.

## Color system

The runtime `Palette` exposes primary, secondary, tertiary, error, surface, surface-container-equivalent, outline,
shadow, hover, and corresponding on-colors. Theme generation additionally produces the fuller Material role set used by
templates: containers, fixed roles, surface dim/bright, five surface-container levels, outline variant, scrim, inverse
roles, background, and terminal colors.

The current renderer maps generated `surface_container` to the compact runtime `surfaceVariant` role. This preserves
compatibility with the existing scene graph while allowing exported templates to consume the full generated role set.
Contributors should extend runtime roles only when a component genuinely needs a distinct semantic layer; avoid
hardcoded palette arithmetic inside feature code.

### Dynamic color

The default wallpaper scheme is `m3-expressive`. It is implemented through the vendored Material Color Utilities code
and is available in:

- configuration parsing and defaults;
- wallpaper palette generation;
- theme CLI parsing/help;
- template expression generation;
- custom-palette identifiers and tests.

Supported schemes include `m3-expressive`, `m3-tonal-spot`, `m3-content`, `m3-fruit-salad`, `m3-rainbow`,
`m3-monochrome`, and inherited custom modes. Unknown schemes fall back deterministically to `m3-expressive`.

Palette extraction remains asynchronous in the existing theme pipeline, generated palettes are cached, and theme
changes use palette interpolation rather than partially applying individual roles. Manual seed/fixed palette paths and
light/dark modes remain available.

## Typography

The type scale is designed for shell surfaces rather than documents:

- **Display 36:** lock-screen time or deliberate hero content.
- **Headline 28/22:** large panels and section introductions.
- **Title 18/16:** cards, dialogs, launcher groups, settings rows.
- **Body 14/13:** normal and compact supporting text.
- **Label 12/11:** controls, badges, timestamps, dense widgets.

The default family is a system `sans-serif` fallback. Users can set `shell.font_family`; the renderer uses Pango and
HarfBuzz for shaping. UI scale is applied in logical pixels. Controls must handle truncation, wrapping, and long
translations rather than assuming English lengths.

## Shape and elevation

Shape communicates function:

- small radii for dense controls, checkboxes, and compact list elements;
- medium radii for buttons, quick settings, menus, and cards;
- extra-large radii for major floating panels, search, launcher, and dialog shells;
- full/pill shape only for chips, toggles, sliders, and intentional selection indicators.

`corner_radius_scale` remains a user-level multiplier and can reduce shapes to square. Focus rings use a separate width
and color role and must remain visible regardless of corner-radius preference.

Elevation is expressed through surface role, shadow, outline, and spatial separation. Avoid stacking heavy shadows on
every card. Floating surfaces should use stronger hierarchy than nested content groups.

## Motion system

Motion should explain cause, spatial relationship, and state change. Existing animation primitives consume the shared
`Style::animFast`, `animNormal`, and `animSlow` aliases, now mapped to the Motion duration scale.

Use:

- **90 ms:** hover, press, icon, and direct manipulation feedback;
- **160–220 ms:** menus, quick-setting expansion, result updates, routine panel state changes;
- **320–420 ms:** launcher/control-center entrances, major mode changes, lock-screen and theme transitions.

Avoid arbitrary looping animation, long input-blocking transitions, per-frame process spawning, and layout work that can
be expressed as compositor-friendly transform/opacity changes. Animation must remain correct at variable refresh rates.

### Reduced motion

The inherited animation manager and configuration/state paths expose reduced-motion behavior. Components must provide a
stable end state when duration becomes zero or effects are reduced. Reduced motion should remove large spatial travel,
springs, and decorative interpolation while preserving essential visibility and state feedback.

## Components and states

Reusable controls live in `src/ui/controls`, dialogs in `src/ui/dialogs`, shared visuals in `src/ui/visuals`, and shell
surface patterns under `src/shell`. Button variants, inputs, selects, sliders, toggles, lists, scrollbars, tooltips,
dialogs, cards, notification surfaces, quick settings, media controls, and navigation elements should not be
reimplemented per feature.

Every interactive component should account for relevant states:

- default, hover, focus, keyboard focus, press, selected, dragged, disabled;
- loading and error where asynchronous work exists;
- pointer and touch activation where the protocol stack supports touch;
- theme change and scale change while visible.

State must never be conveyed only by color. Use selection geometry, iconography, text, focus rings, or motion as
appropriate.

## Responsive and multi-monitor guidance

- Work in output-local logical coordinates and use the output scale supplied by Wayland.
- Do not assume one panel edge, one bar thickness, one compositor, or one monitor.
- Derive panel sizing from available output geometry and surface anchors.
- Use scrolling, wrapping, and progressive disclosure for long content and large result sets.
- Test compact laptop, portrait, ultrawide, mixed-resolution, and fractional-scale configurations.
- Keep persistent bar and dock density compact; major panels may use larger expressive spacing.
- Preserve RTL-aware order where the current control/layout supports it.

## Accessibility rules

- Minimum compact pointer target is 32 logical px; touch-capable controls should use at least 44 px.
- Keyboard focus must be visible and distinct from hover.
- Enter/Space activate controls; Escape dismisses transient or modal surfaces when safe.
- Menus, tabs, segmented controls, and lists should support arrow-key navigation.
- Text and icons must maintain contrast in light, dark, and high-contrast modes.
- Avoid fixed heights that clip scaled or translated text.
- Do not use animation as the sole signal for state.

See `docs/ACCESSIBILITY.md` for the tested scope and release checklist.

## Contributor checklist

Before adding a visual literal or a new component:

1. Search `src/ui/design_tokens.h`, `src/ui/style.h`, and existing controls.
2. Select semantic palette roles instead of fixed colors.
3. Verify default, hover, focus, press, selected, disabled, loading, and error states.
4. Verify reduced-motion and zero-duration behavior.
5. Check light/dark/high-contrast, UI scale, long labels, keyboard-only use, and narrow widths.
6. Add focused logic tests when state resolution can be separated from rendering.
