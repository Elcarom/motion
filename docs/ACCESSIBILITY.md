# Accessibility

## Scope

Motion's architecture includes keyboard event handling, focusable reusable controls, semantic control state, focus
rings, configurable UI scale, high-contrast state, reduced motion, notification behavior, and pointer/touch protocol
paths. This transformation reviewed and documented those systems and centralized target, focus, type, shape, and motion
values.

No formal WCAG, EN 301 549, Section 508, or screen-reader certification is claimed. The transformation environment did
not provide a graphical Wayland session, accessibility bus, screen reader, touch device, or colorimeter.

## Interaction requirements

Interactive surfaces should support:

- Tab/Shift+Tab or surface-specific logical focus traversal;
- Enter and Space activation for buttons and toggles;
- Escape dismissal for menus, dialogs, launcher, control center, and transient panels when safe;
- arrow navigation for menus, lists, tabs, segmented controls, and option groups;
- a visible keyboard focus ring that is not replaced by hover styling;
- no keyboard trap inside a modal or scroll area;
- pointer targets of at least 32 logical px and touch targets of at least 44 logical px where touch is supported.

Focus order should follow reading and spatial order. Restoring focus to the invoking control after closing a transient
surface is preferred.

## Visual accessibility

- Use semantic on-colors rather than assuming black or white foregrounds.
- Preserve non-color indicators for selection, errors, muted state, recording, privacy, and disabled state.
- Do not suppress focus outlines for aesthetic reasons.
- Test light, dark, `high_contrast`, and `pure_black_dark` combinations.
- Text should reflow or truncate deliberately under `accessibility.ui_scale`, fractional output scale, and long
  translations.
- Avoid fixed-height text containers that clip ascenders, descenders, or multi-line content.

Material generated palettes provide paired roles, but actual component contrast must still be reviewed because opacity,
wallpaper translucency, imagery, and state layers affect the final result.

## Motion accessibility

Reduced motion must preserve final state and operability. Disable or simplify large spatial travel, springs, parallax,
continuous decorative effects, and cross-surface choreography. Fast opacity or direct state changes can remain when they
are needed to communicate visibility.

Test repeated open/close interactions with reduced motion enabled to catch zero-duration division, stale animation
state, delayed focus, and callbacks that assume an animation frame will occur.

## Notifications and timeouts

Critical notifications should not disappear before they can be read. Actions must remain keyboard reachable.
Notification content can be sensitive; screen-reader announcements should avoid exposing history after a lock or user
switch. Time-limited surfaces should pause or provide a persistent history where feasible.

## Release checklist

- [ ] Navigate launcher, control center, settings, notification history, tray menus, dock, session menu, and lock screen
      without a pointer.
- [ ] Verify visible focus and logical restoration after dismissal.
- [ ] Activate controls with Enter and Space; dismiss with Escape.
- [ ] Verify arrow navigation in menus, lists, tabs, and segmented controls.
- [ ] Test UI scale at 1.0, 1.25, 1.5, and 2.0.
- [ ] Test light, dark, high contrast, and pure black.
- [ ] Test reduced motion and zero-duration transitions.
- [ ] Test long translated strings and RTL where supported.
- [ ] Inspect foreground/background contrast on translucent surfaces.
- [ ] Test with Orca/AT-SPI if the stack exposes usable semantics; document unsupported nodes rather than claiming
      coverage.
- [ ] Test touch targets on real touch hardware when supported.
