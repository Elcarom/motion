{
  config,
  lib,
  ...
}:
let
  cfg = config.programs.motion;
in
{
  imports = [ (lib.mkAliasOptionModule [ "programs" "noctalia" ] [ "programs" "motion" ]) ];

  options.programs.motion = {
    enable = lib.mkEnableOption "Whether to enable motion, a lightweight Wayland shell and bar.";

    package = lib.mkOption {
      type = lib.types.nullOr lib.types.package;
      default = null;
      description = "The motion package to install.";
    };

    systemd = {
      enable = lib.mkEnableOption "Enables a systemd user service for motion.";

      target = lib.mkOption {
        type = lib.types.str;
        default = "graphical-session.target";
        example = "hyprland-session.target";
        description = "The systemd user target for the motion service.";
      };
    };

    recommendedServices.enable = lib.mkEnableOption ''
      NixOS services used by Motion v5 integrations, including NetworkManager,
      Bluetooth, UPower, and a power profile service.
    '';
  };

  config = lib.mkIf cfg.enable (
    lib.mkMerge [
      {
        environment.systemPackages = lib.optional (cfg.package != null) cfg.package;

        systemd.user.services.motion = lib.mkIf cfg.systemd.enable {
          description = "Motion - A lightweight Wayland shell and bar";
              partOf = [ cfg.systemd.target ];
          after = [ cfg.systemd.target ];
          wantedBy = [ cfg.systemd.target ];
          restartTriggers = [ cfg.package ];

          environment.PATH = lib.mkForce null;

          serviceConfig = {
            ExecStart = lib.getExe cfg.package;
            Restart = "on-failure";
          };
        };

        assertions = [
          {
            assertion = !cfg.systemd.enable || cfg.package != null;
            message = "programs.motion.package cannot be null when programs.motion.systemd.enable is true";
          }
        ];
      }

      (lib.mkIf cfg.recommendedServices.enable {
        networking.networkmanager.enable = lib.mkDefault true;
        hardware.bluetooth.enable = lib.mkDefault true;
        services.upower.enable = lib.mkDefault true;
        services.power-profiles-daemon.enable = lib.mkIf (!config.services.tuned.enable) (
          lib.mkDefault true
        );
      })
    ]
  );
}
