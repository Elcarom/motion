#include "theme/palette_mapping.h"

#include "render/core/color.h"


namespace motion::theme {

  namespace {

    constexpr std::uint32_t kOpaqueBlack = 0xff000000U;
    constexpr std::uint32_t kOpaqueWhite = 0xffffffffU;

    ::Color tokenToColor(const TokenMap& tokens, std::string_view key) {
      auto it = tokens.find(std::string(key));
      if (it == tokens.end())
        return hex("#ff00ff");
      return rgbHex(it->second & 0x00FFFFFFU);
    }

    std::uint32_t colorToArgb(const ::Color& color) {
      auto toByte = [](float value) {
        return static_cast<std::uint32_t>(value <= 0.0f ? 0.0f : (value >= 1.0f ? 255.0f : value * 255.0f + 0.5f));
      };
      return kOpaqueBlack | (toByte(color.r) << 16U) | (toByte(color.g) << 8U) | toByte(color.b);
    }

    void setToken(TokenMap& dst, std::string_view key, const ::Color& color) {
      dst[std::string(key)] = colorToArgb(color);
    }

    bool hasToken(const TokenMap& tokens, std::string_view key) { return tokens.contains(std::string(key)); }

    std::uint32_t tokenOr(const TokenMap& tokens, std::string_view key, std::uint32_t fallback) {
      auto it = tokens.find(std::string(key));
      return it == tokens.end() ? fallback : it->second;
    }

    void setMissingToken(TokenMap& tokens, std::string_view key, std::uint32_t value) {
      if (!hasToken(tokens, key)) {
        tokens[std::string(key)] = value;
      }
    }

    const ::Color& ansiColorForKey(const TerminalAnsiColors& colors, std::string_view key) {
      if (key == kTerminalBlackJsonKey)
        return colors.black;
      if (key == kTerminalRedJsonKey)
        return colors.red;
      if (key == kTerminalGreenJsonKey)
        return colors.green;
      if (key == kTerminalYellowJsonKey)
        return colors.yellow;
      if (key == kTerminalBlueJsonKey)
        return colors.blue;
      if (key == kTerminalMagentaJsonKey)
        return colors.magenta;
      if (key == kTerminalCyanJsonKey)
        return colors.cyan;
      return colors.white;
    }

    const ::Color& directColorForKey(const TerminalPalette& terminal, std::string_view key) {
      if (key == kTerminalForegroundJsonKey)
        return terminal.foreground;
      if (key == kTerminalBackgroundJsonKey)
        return terminal.background;
      if (key == kTerminalCursorJsonKey)
        return terminal.cursor;
      if (key == kTerminalCursorTextJsonKey)
        return terminal.cursorText;
      if (key == kTerminalSelectionFgJsonKey)
        return terminal.selectionFg;
      return terminal.selectionBg;
    }

    const TerminalAnsiColors& ansiGroupForKey(const TerminalPalette& terminal, std::string_view key) {
      return key == kTerminalBrightJsonKey ? terminal.bright : terminal.normal;
    }

    ::Color& directColorSlot(TerminalPalette& terminal, std::string_view key) {
      if (key == kTerminalForegroundJsonKey) {
        return terminal.foreground;
      }
      if (key == kTerminalBackgroundJsonKey) {
        return terminal.background;
      }
      if (key == kTerminalCursorJsonKey) {
        return terminal.cursor;
      }
      if (key == kTerminalCursorTextJsonKey) {
        return terminal.cursorText;
      }
      if (key == kTerminalSelectionFgJsonKey) {
        return terminal.selectionFg;
      }
      return terminal.selectionBg;
    }

    void applyAnsiColors(TokenMap& tokens, std::string_view prefix, const TerminalAnsiColors& colors) {
      for (const auto key : kTerminalAnsiColorJsonKeys) {
        std::string tokenKey(prefix);
        tokenKey += "_";
        tokenKey += key;
        setToken(tokens, tokenKey, ansiColorForKey(colors, key));
      }
    }

    void setAnsiColor(TerminalAnsiColors& colors, std::string_view jsonKey, const ::Color& color) {
      if (jsonKey == kTerminalBlackJsonKey) {
        colors.black = color;
      } else if (jsonKey == kTerminalRedJsonKey) {
        colors.red = color;
      } else if (jsonKey == kTerminalGreenJsonKey) {
        colors.green = color;
      } else if (jsonKey == kTerminalYellowJsonKey) {
        colors.yellow = color;
      } else if (jsonKey == kTerminalBlueJsonKey) {
        colors.blue = color;
      } else if (jsonKey == kTerminalMagentaJsonKey) {
        colors.magenta = color;
      } else if (jsonKey == kTerminalCyanJsonKey) {
        colors.cyan = color;
      } else {
        colors.white = color;
      }
    }

  } // namespace

  void applyTerminalPalette(TokenMap& tokens, const TerminalPalette& terminal) {
    for (const auto& group : kTerminalAnsiGroupTokenKeys) {
      applyAnsiColors(tokens, group.tokenPrefix, ansiGroupForKey(terminal, group.jsonKey));
    }
    for (const auto& key : kTerminalDirectColorTokenKeys) {
      setToken(tokens, key.tokenKey, directColorForKey(terminal, key.jsonKey));
    }
  }

  void synthesizeTerminalPaletteTokens(TokenMap& tokens) {
    const std::uint32_t background = tokenOr(tokens, "background", tokenOr(tokens, "surface", kOpaqueBlack));
    const std::uint32_t foreground = tokenOr(tokens, "on_surface", kOpaqueWhite);
    const std::uint32_t surface = tokenOr(tokens, "surface", background);
    const std::uint32_t surfaceVariant = tokenOr(tokens, "surface_variant", surface);
    const std::uint32_t onSurfaceVariant = tokenOr(tokens, "on_surface_variant", foreground);
    const std::uint32_t outline = tokenOr(tokens, "outline", onSurfaceVariant);
    const std::uint32_t error = tokenOr(tokens, "error", foreground);
    const std::uint32_t primary = tokenOr(tokens, "primary", foreground);
    const std::uint32_t secondary = tokenOr(tokens, "secondary", primary);
    const std::uint32_t tertiary = tokenOr(tokens, "tertiary", secondary);
    const std::uint32_t primaryFixedDim = tokenOr(tokens, "primary_fixed_dim", primary);
    const std::uint32_t secondaryFixedDim = tokenOr(tokens, "secondary_fixed_dim", secondary);

    setMissingToken(tokens, "terminal_foreground", foreground);
    setMissingToken(tokens, "terminal_background", background);
    setMissingToken(tokens, "terminal_cursor", foreground);
    setMissingToken(tokens, "terminal_cursor_text", background);
    setMissingToken(tokens, "terminal_selection_fg", onSurfaceVariant);
    setMissingToken(tokens, "terminal_selection_bg", surfaceVariant);

    setMissingToken(tokens, "terminal_normal_black", surfaceVariant);
    setMissingToken(tokens, "terminal_normal_red", error);
    setMissingToken(tokens, "terminal_normal_green", primary);
    setMissingToken(tokens, "terminal_normal_yellow", secondary);
    setMissingToken(tokens, "terminal_normal_blue", tertiary);
    setMissingToken(tokens, "terminal_normal_magenta", primaryFixedDim);
    setMissingToken(tokens, "terminal_normal_cyan", secondaryFixedDim);
    setMissingToken(tokens, "terminal_normal_white", foreground);

    setMissingToken(tokens, "terminal_bright_black", outline);
    setMissingToken(tokens, "terminal_bright_red", error);
    setMissingToken(tokens, "terminal_bright_green", primary);
    setMissingToken(tokens, "terminal_bright_yellow", secondary);
    setMissingToken(tokens, "terminal_bright_blue", tertiary);
    setMissingToken(tokens, "terminal_bright_magenta", primaryFixedDim);
    setMissingToken(tokens, "terminal_bright_cyan", secondaryFixedDim);
    setMissingToken(tokens, "terminal_bright_white", foreground);
  }

  void synthesizeTerminalPaletteTokens(GeneratedPalette& palette) {
    if (!palette.dark.empty()) {
      synthesizeTerminalPaletteTokens(palette.dark);
    }
    if (!palette.light.empty()) {
      synthesizeTerminalPaletteTokens(palette.light);
    }
  }

  ::Palette mapGeneratedPaletteMode(const TokenMap& t) {
    return ::Palette{
        .primary = tokenToColor(t, "primary"),
        .onPrimary = tokenToColor(t, "on_primary"),
        .secondary = tokenToColor(t, "secondary"),
        .onSecondary = tokenToColor(t, "on_secondary"),
        .tertiary = tokenToColor(t, "tertiary"),
        .onTertiary = tokenToColor(t, "on_tertiary"),
        .error = tokenToColor(t, "error"),
        .onError = tokenToColor(t, "on_error"),
        .surface = tokenToColor(t, "surface"),
        .onSurface = tokenToColor(t, "on_surface"),
        .surfaceVariant = tokenToColor(t, "surface_variant"),
        .onSurfaceVariant = tokenToColor(t, "on_surface_variant"),
        .outline = tokenToColor(t, "outline"),
        .shadow = tokenToColor(t, "shadow"),
        .hover = tokenToColor(t, "tertiary"),
        .onHover = tokenToColor(t, "on_tertiary"),
        .primaryContainer = tokenToColor(t, "primary_container"),
        .onPrimaryContainer = tokenToColor(t, "on_primary_container"),
        .secondaryContainer = tokenToColor(t, "secondary_container"),
        .onSecondaryContainer = tokenToColor(t, "on_secondary_container"),
        .tertiaryContainer = tokenToColor(t, "tertiary_container"),
        .onTertiaryContainer = tokenToColor(t, "on_tertiary_container"),
        .errorContainer = tokenToColor(t, "error_container"),
        .onErrorContainer = tokenToColor(t, "on_error_container"),
        .background = tokenToColor(t, "background"),
        .onBackground = tokenToColor(t, "on_background"),
        .surfaceDim = tokenToColor(t, "surface_dim"),
        .surfaceBright = tokenToColor(t, "surface_bright"),
        .surfaceContainerLowest = tokenToColor(t, "surface_container_lowest"),
        .surfaceContainerLow = tokenToColor(t, "surface_container_low"),
        .surfaceContainer = tokenToColor(t, "surface_container"),
        .surfaceContainerHigh = tokenToColor(t, "surface_container_high"),
        .surfaceContainerHighest = tokenToColor(t, "surface_container_highest"),
        .surfaceTint = tokenToColor(t, "surface_tint"),
        .outlineVariant = tokenToColor(t, "outline_variant"),
        .inverseSurface = tokenToColor(t, "inverse_surface"),
        .inverseOnSurface = tokenToColor(t, "inverse_on_surface"),
        .inversePrimary = tokenToColor(t, "inverse_primary"),
        .scrim = tokenToColor(t, "scrim"),
    };
  }

  TerminalPalette terminalPaletteFromTokens(const TokenMap& tokens) {
    TerminalPalette terminal{};
    for (const auto& group : kTerminalAnsiGroupTokenKeys) {
      TerminalAnsiColors& colors = group.jsonKey == kTerminalBrightJsonKey ? terminal.bright : terminal.normal;
      for (const auto key : kTerminalAnsiColorJsonKeys) {
        std::string tokenKey(group.tokenPrefix);
        tokenKey.push_back('_');
        tokenKey.append(key);
        setAnsiColor(colors, key, tokenToColor(tokens, tokenKey));
      }
    }
    for (const auto& key : kTerminalDirectColorTokenKeys) {
      directColorSlot(terminal, key.jsonKey) = tokenToColor(tokens, key.tokenKey);
    }
    return terminal;
  }

} // namespace motion::theme
