#!/usr/bin/env python3
"""Dependency-light repository, metadata, and asset validation for CI and release packaging."""

from __future__ import annotations

import json
import re
import struct
import sys
import tomllib
import xml.etree.ElementTree as ET
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
errors: list[str] = []
checks = 0


def check(condition: bool, message: str) -> None:
    global checks
    checks += 1
    if not condition:
        errors.append(message)


required = [
    "README.md", "LICENSE", "NOTICE", "UPSTREAM.md", "CONTRIBUTING.md", "CODE_OF_CONDUCT.md", "SECURITY.md",
    "CHANGELOG.md", "docs/AUDIT.md", "docs/ARCHITECTURE.md", "docs/DESIGN_SYSTEM.md", "docs/MIGRATION.md",
    "docs/ACCESSIBILITY.md", "docs/CONFIGURATION.md", "docs/TROUBLESHOOTING.md", "docs/VALIDATION.md",
    "meson.build", "flake.nix", "flake.lock", "motion.scm", "example.toml",
    "assets/motion.svg", "assets/motion-wallpaper.png", "assets/dev.motion.Motion.desktop",
]
for rel in required:
    check((ROOT / rel).is_file(), f"missing required file: {rel}")

meson = (ROOT / "meson.build").read_text(encoding="utf-8")
check("project('motion'" in meson, "Meson project is not named motion")
check("version: '5.0.0-motion.1'" in meson, "unexpected Motion version")
check("install_symlink('noctalia', pointing_to: 'motion'" in meson, "legacy command alias is missing")

# Validate every JSON document.
for path in sorted(ROOT.rglob("*.json")):
    if ".git" in path.parts:
        continue
    try:
        json.loads(path.read_text(encoding="utf-8"))
        checks += 1
    except Exception as exc:  # noqa: BLE001
        errors.append(f"invalid JSON {path.relative_to(ROOT)}: {exc}")

# Validate repository-owned TOML documents that are expected to be standalone.
for path in [ROOT / "example.toml", *sorted((ROOT / "assets/templates").rglob("*.toml"))]:
    try:
        tomllib.loads(path.read_text(encoding="utf-8"))
        checks += 1
    except Exception as exc:  # noqa: BLE001
        errors.append(f"invalid TOML {path.relative_to(ROOT)}: {exc}")

# Validate XML/SVG syntax.
for path in [ROOT / "assets/motion.svg", *sorted((ROOT / "protocols").glob("*.xml"))]:
    try:
        ET.parse(path)
        checks += 1
    except Exception as exc:  # noqa: BLE001
        errors.append(f"invalid XML {path.relative_to(ROOT)}: {exc}")

# Validate PNG signature and dimensions without external image libraries.
png = ROOT / "assets/motion-wallpaper.png"
try:
    data = png.read_bytes()
    check(data[:8] == b"\x89PNG\r\n\x1a\n", "Motion wallpaper is not a PNG")
    width, height = struct.unpack(">II", data[16:24])
    check((width, height) == (3840, 2160), f"unexpected wallpaper dimensions: {width}x{height}")
except Exception as exc:  # noqa: BLE001
    errors.append(f"cannot validate Motion wallpaper: {exc}")

# Desktop entry essentials.
desktop = (ROOT / "assets/dev.motion.Motion.desktop").read_text(encoding="utf-8")
for item in ["Name=Motion", "Exec=motion --daemon", "Icon=motion", "Exec=sh -c \"motion msg settings-open"]:
    check(item in desktop, f"desktop entry missing: {item}")

# Meson source/test paths must exist.
for rel in sorted(set(re.findall(r"['\"]((?:src|tests|assets|protocols)/[^'\"]+\.(?:cpp|c|h|xml|desktop|svg))['\"]", meson))):
    check((ROOT / rel).exists(), f"Meson references missing file: {rel}")

# Reject accidental generated/build output in the source tree.
for name in ["compile_commands.json", ".ninja_log", ".ninja_deps"]:
    check(not (ROOT / name).exists(), f"generated file present at repository root: {name}")
for path in ROOT.iterdir():
    if path.is_dir() and (path.name.startswith("build-") or path.name.startswith("result")):
        errors.append(f"build output present in source tree: {path.name}")

if errors:
    print(f"Repository validation failed ({len(errors)} errors, {checks} checks):", file=sys.stderr)
    for error in errors:
        print(f"- {error}", file=sys.stderr)
    raise SystemExit(1)

print(f"Repository validation passed ({checks} checks).")
