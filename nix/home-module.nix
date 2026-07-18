{
  config,
  pkgs,
  lib,
  ...
}:
let
  cfg = config.programs.motion;
  jsonFormat = pkgs.formats.json { };
  tomlFormat = pkgs.formats.toml { };

  generateConfig =
    format: name: value:
    if lib.isString value then
      pkgs.writeText name value
    else if builtins.isPath value || lib.isStorePath value then
      value
    else
      format.generate name value;

  generateToml = generateConfig tomlFormat;
  generateJson = generateConfig jsonFormat;
in
{
  imports = [ (lib.mkAliasOptionModule [ "programs" "noctalia" ] [ "programs" "motion" ]) ];

  options.programs.motion = {
    enable = lib.mkEnableOption "Whether to enable motion, a lightweight Wayland shell and bar.";

    systemd.enable = lib.mkEnableOption "Enables a systemd user service for motion.";

    package = lib.mkOption {
      type = lib.types.nullOr lib.types.package;
      description = "The motion package to use.";
    };

    validateConfig = lib.mkOption {
      type = lib.types.bool;
      default = true;
      description = "Validate the configuration file at build time.";
    };

    settings = lib.mkOption {
      type =
        with lib.types;
        oneOf [
          tomlFormat.type
          str
          path
        ];
      default = { };
      description = ''
        Default settings for motion, Can be written as:
          - A Nix attrset (converted to TOML via nixpkgs' tomlFormat)
          - A raw TOML string
          - A path to a `.toml` file

        See README.md and docs/CONFIGURATION.md in the source tree for examples.

        Note: these settings can still be overwritten at runtime via the settings menu.
      '';
      example = lib.literalExpression ''
        shell = {
          font = "JetBrainsMono Nerd Font";
          settings_show_advanced = true;
        };

        theme = {
          mode = "dark";
          source = "builtin";
          builtin = "Catppuccin";
        };
      '';
    };

    customPalettes = lib.mkOption {
      type =
        with lib.types;
        oneOf [
          jsonFormat.type
          str
          path
        ];
      default = { };
      description = ''
        Custom color pallete options.

        See docs/DESIGN_SYSTEM.md and example.toml in the source tree.
      '';
    };
  };

  config = lib.mkIf cfg.enable {
    systemd.user.services.motion = lib.mkIf cfg.systemd.enable {
      Unit = {
        Description = "Motion - A lightweight Wayland shell and bar";
        PartOf = [ config.wayland.systemd.target ];
        After = [ config.wayland.systemd.target ];
        X-Restart-Triggers =
          lib.optional (cfg.settings != { }) "${config.xdg.configFile."motion/config.toml".source}"
          ++ lib.mapAttrsToList (
            name: _: "${config.xdg.configFile."motion/palettes/${name}.json".source}"
          ) cfg.customPalettes;
      };

      Service = {
        ExecStart = lib.getExe cfg.package;
        Restart = "on-failure";
      };

      Install.WantedBy = [ config.wayland.systemd.target ];
    };

    home.packages = lib.optional (cfg.package != null) cfg.package;

    xdg = {
      configFile = lib.mkMerge [
        (lib.mkIf (cfg.settings != { }) {
          "motion/config.toml".source =
            let
              rawConfig = generateToml "config.toml" cfg.settings;
            in
            if cfg.validateConfig && cfg.package != null then
              pkgs.runCommand "motion-config" { } ''
                ${lib.getExe cfg.package} config validate ${rawConfig}
                cp ${rawConfig} $out
              ''
            else
              rawConfig;
        })
        (lib.mapAttrs' (
          name: palette:
          lib.nameValuePair "motion/palettes/${name}.json" {
            source = generateJson "${name}-palette.json" palette;
          }
        ) cfg.customPalettes)
      ];
    };

    assertions = [
      {
        assertion = !cfg.systemd.enable || cfg.package != null;
        message = "programs.motion.package cannot be null when programs.motion.systemd.enable is true";
      }
    ];
  };

  _class = "homeManager";
}
