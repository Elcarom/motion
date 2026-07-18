#!/usr/bin/env python3
"""Fail on accidental fork branding while allowing documented compatibility/attribution."""

from __future__ import annotations

import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
TEXT_SUFFIXES = {
    ".c", ".cc", ".cpp", ".h", ".hpp", ".md", ".txt", ".toml", ".json", ".nix", ".scm", ".yml", ".yaml",
    ".desktop", ".sh", ".xml", ".theme", ".colors", ".conf", ".ini",
}
SKIP_PARTS = {"third_party", ".git", "artifacts"}

# Noctalia references are valid only for attribution, migration, compatibility,
# inherited service/plugin identifiers, and regression fixtures.
ALLOWED_NOCTALIA_PATHS = {
    "LICENSE", "NOTICE", "UPSTREAM.md", "CREDITS.md", "README.md", "CHANGELOG.md",
    "docs/ARCHITECTURE.md", "docs/MIGRATION.md", "docs/CONFIGURATION.md", "docs/VALIDATION.md",
    "docs/AUDIT.md", "SECURITY.md",
    "src/core/legacy_migration.cpp", "src/core/legacy_migration.h", "src/main.cpp",
    "src/util/file_utils.h", "src/core/files/resource_paths.cpp", "src/hooks/hook_manager.cpp",
    "src/app/application_ipc.cpp", "src/shell/notification/notification_toast.cpp",
    "src/calendar/google_oauth.cpp", "src/calendar/google_oauth.h", "src/calendar/calendar_service.h",
    "src/system/location_service.cpp", "src/shell/control_center/tabs/network_tab.cpp",
    "src/shell/settings/plugin_store_content.cpp", "src/theme/community_templates.cpp",
    "src/theme/community_palettes.cpp", "src/config/config_types.cpp",
    "tests/legacy_migration_test.cpp", "tests/hook_manager_test.cpp", "tests/file_utils_paths_test.cpp",
    "tests/config_schema_roundtrip_test.cpp", "tests/config_validate/generated-config", "default.nix", "meson.build",
    "tools/branding_audit.py", "tools/validate_repository.py", ".github/workflows/ci.yml",
    "nix/home-module.nix", "nix/hjem-module.nix", "nix/nixos-module.nix",
}

errors: list[str] = []
intentional: list[str] = []
for path in sorted(ROOT.rglob("*")):
    if not path.is_file() or any(part in SKIP_PARTS for part in path.parts):
        continue
    if path.suffix.lower() not in TEXT_SUFFIXES and path.name not in {"LICENSE", "NOTICE"}:
        continue
    rel = path.relative_to(ROOT).as_posix()
    try:
        text = path.read_text(encoding="utf-8")
    except UnicodeDecodeError:
        continue
    if re.search(r"motion-dev|(?:api|docs|assets|discord|i18n)\.motion\.dev", text, re.IGNORECASE):
        errors.append(f"{rel}: references an unconfigured Motion organization/service")
    # Inherited optional network services and upstream plugin repositories are
    # deliberate compatibility dependencies, not product branding.
    branding_text = re.sub(r"(?:https?://)?(?:api\.)?noctalia\.dev[^\s\"')<,]*", "", text, flags=re.IGNORECASE)
    branding_text = re.sub(r"https?://github\.com/noctalia-dev/noctalia-plugins[^\s\"')<]*", "", branding_text, flags=re.IGNORECASE)
    if re.search(r"noctalia", branding_text, re.IGNORECASE):
        if rel in ALLOWED_NOCTALIA_PATHS:
            intentional.append(rel)
        else:
            errors.append(f"{rel}: unexpected Noctalia branding/reference")

# Product-facing assets must exist under Motion names.
for required in [
    "assets/motion.svg", "assets/motion-wallpaper.png", "assets/dev.motion.Motion.desktop",
]:
    if not (ROOT / required).is_file():
        errors.append(f"missing Motion asset: {required}")

if errors:
    print("Branding audit failed:", file=sys.stderr)
    for item in errors:
        print(f"- {item}", file=sys.stderr)
    raise SystemExit(1)

print(f"Branding audit passed; {len(set(intentional))} files contain allow-listed upstream compatibility/attribution.")
