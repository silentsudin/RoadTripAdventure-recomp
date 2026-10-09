#!/usr/bin/env python3
"""Downloads the pinned llvm-mingw release (clang + lld + libc++ + mingw-w64, UCRT, x86_64 host)
that the Windows build and the app's on-PC game compiler use, and prints its directory.

    python scripts/fetch_llvm_mingw.py [--out build/llvm-mingw]

The archive's SHA-256 is checked; bump VERSION and SHA256 together to move to another release.
"""
from __future__ import annotations

import argparse
import hashlib
import sys
import urllib.request
import zipfile
from pathlib import Path

VERSION = "20261006"
SHA256 = "317492c456aa27ee607a5919f1d2d38dcdc1112516a24d0bf4b00d078f52d17a"
NAME = f"llvm-mingw-{VERSION}-ucrt-x86_64"
URL = f"https://github.com/mstorsjo/llvm-mingw/releases/download/{VERSION}/{NAME}.zip"


def main() -> None:
    ap = argparse.ArgumentParser()
    ap.add_argument("--out", type=Path, default=Path(__file__).resolve().parent.parent / "build" / "llvm-mingw")
    args = ap.parse_args()
    root = args.out / NAME
    if (root / "bin" / "x86_64-w64-mingw32-clang++.exe").exists():
        print(root)
        return
    args.out.mkdir(parents=True, exist_ok=True)
    archive = args.out / f"{NAME}.zip"
    if not archive.exists() or hashlib.sha256(archive.read_bytes()).hexdigest() != SHA256:
        print(f"downloading {URL}", file=sys.stderr)
        urllib.request.urlretrieve(URL, archive)
    digest = hashlib.sha256(archive.read_bytes()).hexdigest()
    if digest != SHA256:
        sys.exit(f"fetch_llvm_mingw: {archive.name} has SHA-256 {digest}, expected {SHA256}")
    with zipfile.ZipFile(archive) as z:
        z.extractall(args.out)
    print(root)


if __name__ == "__main__":
    main()
