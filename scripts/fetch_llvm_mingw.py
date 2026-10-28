#!/usr/bin/env python3
"""Downloads the pinned host-native llvm-mingw release (clang + lld + libc++ + mingw-w64, UCRT)
that the Windows build and the app's on-PC game compiler use, and prints its directory.

    python scripts/fetch_llvm_mingw.py [--out build/llvm-mingw]

The archive's SHA-256 is checked; bump VERSION and SHA256 together to move to another release.
"""
from __future__ import annotations

import argparse
import hashlib
import platform
import sys
import tarfile
import urllib.request
import zipfile
from pathlib import Path

VERSION = "20261006"
SHA256 = "317492c456aa27ee607a5919f1d2d38dcdc1112516a24d0bf4b00d078f52d17a"
HOST = platform.system()
if HOST == "Darwin":
    NAME = f"llvm-mingw-{VERSION}-ucrt-macos-universal"
    SUFFIX = ".tar.xz"
    SHA256 = "a7a19b11048e89372637d26279cdb7c430ed3cf8c1e224310421312bf986ebf6"
elif HOST == "Windows":
    NAME = f"llvm-mingw-{VERSION}-ucrt-x86_64"
    SUFFIX = ".zip"
else:
    NAME = f"llvm-mingw-{VERSION}-ucrt-ubuntu-22.04-x86_64"
    SUFFIX = ".tar.xz"
    SHA256 = "5f9c6ed95b2d4bdb2869a488c5fd5857fbdabcf288a0aa3eb1da43f6a08d8ab4"
URL = f"https://github.com/mstorsjo/llvm-mingw/releases/download/{VERSION}/{NAME}{SUFFIX}"


def main() -> None:
    ap = argparse.ArgumentParser()
    ap.add_argument("--out", type=Path, default=Path(__file__).resolve().parent.parent / "build" / "llvm-mingw")
    args = ap.parse_args()
    root = args.out / NAME
    compiler = root / "bin" / "x86_64-w64-mingw32-clang++"
    if compiler.exists() or compiler.with_suffix(".exe").exists():
        print(root.as_posix())
        return
    args.out.mkdir(parents=True, exist_ok=True)
    archive = args.out / f"{NAME}{SUFFIX}"
    if not archive.exists() or hashlib.sha256(archive.read_bytes()).hexdigest() != SHA256:
        print(f"downloading {URL}", file=sys.stderr)
        urllib.request.urlretrieve(URL, archive)
    digest = hashlib.sha256(archive.read_bytes()).hexdigest()
    if digest != SHA256:
        sys.exit(f"fetch_llvm_mingw: {archive.name} has SHA-256 {digest}, expected {SHA256}")
    if SUFFIX == ".zip":
        with zipfile.ZipFile(archive) as z:
            z.extractall(args.out)
    else:
        with tarfile.open(archive) as tar:
            tar.extractall(args.out, filter="data")
    print(root.as_posix())


if __name__ == "__main__":
    main()
