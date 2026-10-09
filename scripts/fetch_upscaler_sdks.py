#!/usr/bin/env python3
"""Downloads the upscaler SDKs the Windows plugins are built against (pinned) into build/sdks and
prints the CMake options for plugins/upscalers:

    python scripts/fetch_upscaler_sdks.py [--out build/sdks] [--only ffx|dlss|xess ...]

  ffx   AMD FidelityFX SDK 1.1.4 (MIT): the ffx-api headers and the signed amd_fidelityfx_vk.dll
        (the last release line with a Vulkan backend; 2.x is DirectX 12 only)
  dlss  NVIDIA DLSS SDK 310.9.1: NGX headers, static library and nvngx_dlss.dll (NVIDIA RTX SDKs
        licence: redistributable in the add-on, never linked into the GPL app)
  xess  Intel XeSS SDK 3.0.2: headers, libxess.lib and libxess.dll (Intel Simplified Software
        License: redistribution in binary form with the licence)

Each is checked against its pinned commit or SHA-256. Needs git (and gh or an internet connection).
"""
from __future__ import annotations

import argparse
import hashlib
import shutil
import subprocess
import sys
import urllib.request
import zipfile
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent

FFX_URL = "https://github.com/GPUOpen-LibrariesAndSDKs/FidelityFX-SDK.git"
FFX_TAG = "v1.1.4"
FFX_COMMIT = "c6efa6bf7f2027b3ec94f28578bb5965eabb9e55"

DLSS_URL = "https://github.com/NVIDIA/DLSS.git"
DLSS_COMMIT = "374959484e79a640feaba44c93ac8cfb0a03f5b5"  # v310.9.1

XESS_URL = "https://github.com/intel/xess/releases/download/v3.0.2/XeSS_SDK_3.0.2.zip"
XESS_SHA256 = "88b8a373f30e33f3558a77a93e634f11b8132fc3047ea1a8edeead32b8471990"


def git(*args: str, cwd: Path | None = None) -> str:
    return subprocess.run(["git", "-c", "core.longpaths=true", *args], cwd=cwd, check=True, capture_output=True,
                          text=True).stdout.strip()


def sparse(url: str, dest: Path, paths: list[str], commit: str, ref: str | None) -> None:
    if (dest / ".git").exists() and git("rev-parse", "HEAD", cwd=dest) == commit:
        return
    if dest.exists():
        shutil.rmtree(dest)
    dest.parent.mkdir(parents=True, exist_ok=True)
    cmd = ["clone", "-q", "--filter=blob:none", "--sparse"]
    if ref:
        cmd += ["--depth", "1", "--branch", ref]
    git(*cmd, url, str(dest))
    if not ref:
        git("fetch", "-q", "--depth", "1", "origin", commit, cwd=dest)
        git("checkout", "-q", commit, cwd=dest)
    git("sparse-checkout", "set", *paths, cwd=dest)
    got = git("rev-parse", "HEAD", cwd=dest)
    if got != commit:
        sys.exit(f"fetch_upscaler_sdks: {dest.name} is at {got}, expected {commit}")


def fetch_xess(dest: Path) -> None:
    if (dest / "inc" / "xess" / "xess_vk.h").exists():
        return
    archive = dest.parent / "XeSS_SDK_3.0.2.zip"
    archive.parent.mkdir(parents=True, exist_ok=True)
    if not archive.exists() or hashlib.sha256(archive.read_bytes()).hexdigest() != XESS_SHA256:
        print(f"downloading {XESS_URL}", file=sys.stderr)
        urllib.request.urlretrieve(XESS_URL, archive)
    digest = hashlib.sha256(archive.read_bytes()).hexdigest()
    if digest != XESS_SHA256:
        sys.exit(f"fetch_upscaler_sdks: {archive.name} has SHA-256 {digest}, expected {XESS_SHA256}")
    if dest.exists():
        shutil.rmtree(dest)
    with zipfile.ZipFile(archive) as z:
        z.extractall(dest)


def main() -> None:
    ap = argparse.ArgumentParser()
    ap.add_argument("--out", type=Path, default=ROOT / "build" / "sdks")
    ap.add_argument("--only", nargs="+", choices=["ffx", "dlss", "xess"])
    args = ap.parse_args()
    args.out = args.out.resolve()
    wanted = set(args.only or ["ffx", "dlss", "xess"])
    options = []
    if "ffx" in wanted:
        sparse(FFX_URL, args.out / "ffx", ["ffx-api", "PrebuiltSignedDLL"], FFX_COMMIT, FFX_TAG)
        options.append(f"-DFFX_SDK_DIR={args.out / 'ffx'}")
    if "dlss" in wanted:
        sparse(DLSS_URL, args.out / "dlss", ["include", "lib/Windows_x86_64"], DLSS_COMMIT, None)
        options.append(f"-DDLSS_SDK_DIR={args.out / 'dlss'}")
    if "xess" in wanted:
        fetch_xess(args.out / "xess")
        options.append(f"-DXESS_SDK_DIR={args.out / 'xess'}")
    print(" ".join(options))


if __name__ == "__main__":
    main()
