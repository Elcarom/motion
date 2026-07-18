#!/usr/bin/env python3
"""Create a deterministic source archive for Motion."""

from __future__ import annotations

import argparse
import gzip
import os
from pathlib import Path, PurePosixPath
import stat
import tarfile

EXCLUDED_DIR_NAMES = {
    ".git",
    ".cache",
    ".pytest_cache",
    "__pycache__",
}
EXCLUDED_PREFIXES = ("build-", "result")


def should_exclude(relative: Path) -> bool:
    return any(
        part in EXCLUDED_DIR_NAMES or part.startswith(EXCLUDED_PREFIXES)
        for part in relative.parts
    )


def normalized_info(source: Path, archive_name: str) -> tarfile.TarInfo:
    st = source.lstat()
    info = tarfile.TarInfo(archive_name)
    info.mtime = 0
    info.uid = 0
    info.gid = 0
    info.uname = "root"
    info.gname = "root"

    if source.is_symlink():
        info.type = tarfile.SYMTYPE
        info.linkname = os.readlink(source)
        info.mode = 0o777
        info.size = 0
    elif source.is_dir():
        info.type = tarfile.DIRTYPE
        info.mode = 0o755
        info.size = 0
    elif source.is_file():
        info.type = tarfile.REGTYPE
        info.mode = 0o755 if st.st_mode & stat.S_IXUSR else 0o644
        info.size = st.st_size
    else:
        raise ValueError(f"unsupported source entry: {source}")

    return info


def create_archive(source_root: Path, output: Path, prefix: str) -> int:
    source_root = source_root.resolve()
    output = output.resolve()
    output.parent.mkdir(parents=True, exist_ok=True)

    entries = [Path(".")]
    entries.extend(
        sorted(
            (
                path.relative_to(source_root)
                for path in source_root.rglob("*")
                if not should_exclude(path.relative_to(source_root))
            ),
            key=lambda path: path.as_posix(),
        )
    )

    with output.open("wb") as raw:
        with gzip.GzipFile(filename="", mode="wb", fileobj=raw, mtime=0) as compressed:
            with tarfile.open(fileobj=compressed, mode="w", format=tarfile.PAX_FORMAT) as archive:
                for relative in entries:
                    source = source_root if relative == Path(".") else source_root / relative
                    archive_relative = PurePosixPath(prefix)
                    if relative != Path("."):
                        archive_relative /= PurePosixPath(relative.as_posix())
                    info = normalized_info(source, archive_relative.as_posix())
                    if info.isfile():
                        with source.open("rb") as stream:
                            archive.addfile(info, stream)
                    else:
                        archive.addfile(info)

    return len(entries) - 1


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("output", type=Path, help="output .tar.gz path")
    parser.add_argument("--source", type=Path, default=Path.cwd(), help="repository root")
    parser.add_argument("--prefix", default="motion-0.1.1", help="archive root directory")
    args = parser.parse_args()

    if not args.source.is_dir():
        parser.error(f"source directory does not exist: {args.source}")
    if args.output.suffixes[-2:] != [".tar", ".gz"]:
        parser.error("output must use the .tar.gz suffix")

    count = create_archive(args.source, args.output, args.prefix)
    print(f"Created {args.output} with {count} source entries.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
