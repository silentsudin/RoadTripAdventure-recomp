#!/usr/bin/env python3
"""Post-build step: put everything needed to build the game *on the user's machine* into
RoadTrip.app/Contents/Resources. Nothing here is derived from the game:

  recomp/ps2_recomp       the PS2Recomp recompiler (GPL-3.0)
  recomp/roadtrip.toml    recompiler config (function addresses / names only)
  recomp/ps2_vu1_recomp   the VU1 microcode recompiler (GPL-3.0)
  sdk/include/<n>/...     the runtime headers the generated C++ includes
  sdk/game_shim.cpp       registers the generated functions with the app
  sdk/flags.json          compiler flags matching the app's runtime ABI
  sdk/build_id            hash of all of the above; a change triggers a rebuild on launch
"""
from __future__ import annotations

import argparse
import hashlib
import json
import shlex
import shutil
import subprocess
import sys
from pathlib import Path

KEEP_PREFIXES = ("-D", "-std=", "-O", "-mmacosx-version-min=", "-f")
DROP_EXACT = {"-g", "-c", "-w", "-Winvalid-pch"}


def probe_command(compile_commands: Path, probe: Path) -> tuple[str, list[str], str]:
    for entry in json.loads(compile_commands.read_text()):
        if Path(entry["file"]).resolve() == probe.resolve():
            args = entry.get("arguments") or shlex.split(entry["command"])
            return args[0], args[1:], entry["directory"]
    sys.exit(f"bundle_sdk: {probe} not found in {compile_commands}")


def split_flags(args: list[str]) -> tuple[list[str], list[str]]:
    """Returns (portable flags, absolute include dirs)."""
    flags, includes = [], []
    it = iter(args)
    for a in it:
        if a in ("-o", "-MF", "-MT", "-MQ", "-isysroot", "-Xclang", "-include", "-x"):
            next(it, None)
            continue
        if a == "-I" or a == "-isystem":
            includes.append(next(it))
        elif a.startswith("-I"):
            includes.append(a[2:])
        elif a == "-arch":
            flags += [a, next(it)]
        elif a in DROP_EXACT or a.startswith("-M") or not a.startswith("-"):
            continue
        elif a.startswith(KEEP_PREFIXES):
            flags.append(a)
    return flags, includes


def needed_headers(compiler: str, args: list[str], probe: Path, cwd: str) -> list[Path]:
    clean = [a for a in args if a not in ("-c",)]
    # Drop output / pch-related args, then ask the compiler for the dependency list.
    out, skip = [], False
    for a in clean:
        if skip:
            skip = False
            continue
        if a in ("-o", "-MF", "-MT", "-MQ"):
            skip = True
            continue
        if a == str(probe) or a.startswith("-M"):
            continue
        out.append(a)
    res = subprocess.run([compiler, *out, "-M", str(probe)], cwd=cwd, capture_output=True, text=True)
    if res.returncode != 0:
        sys.exit(f"bundle_sdk: dependency scan failed:\n{res.stderr}")
    deps = res.stdout.replace("\\\n", " ").split(":", 1)[1].split()
    return [Path(d).resolve() for d in deps if not d.endswith(".cpp")]


def main() -> None:
    ap = argparse.ArgumentParser()
    ap.add_argument("--compile-commands", type=Path, required=True)
    ap.add_argument("--probe", type=Path, required=True)
    ap.add_argument("--shim", type=Path, required=True)
    ap.add_argument("--recomp", type=Path, required=True)
    ap.add_argument("--config", type=Path, required=True)
    ap.add_argument("--vu1recomp", type=Path)
    ap.add_argument("--resources", type=Path, required=True)
    a = ap.parse_args()

    compiler, args, cwd = probe_command(a.compile_commands, a.probe)
    flags, include_dirs = split_flags(args)
    include_dirs = [Path(d).resolve() for d in include_dirs]
    headers = needed_headers(compiler, args, a.probe, cwd)

    sdk = a.resources / "sdk"
    recomp = a.resources / "recomp"
    for d in (sdk, recomp):
        if d.exists():
            shutil.rmtree(d)
        d.mkdir(parents=True)

    # Copy each needed header under the first include dir that contains it, keeping relative layout.
    used_dirs: list[str] = []
    copied = 0
    for h in headers:
        for idx, inc in enumerate(include_dirs):
            try:
                rel = h.relative_to(inc)
            except ValueError:
                continue
            dst = sdk / "include" / str(idx) / rel
            dst.parent.mkdir(parents=True, exist_ok=True)
            shutil.copy2(h, dst)
            copied += 1
            if f"include/{idx}" not in used_dirs:
                used_dirs.append(f"include/{idx}")
            break
        # Headers outside every include dir are system/SDK headers: the user's toolchain has them.

    # Keep the original include order so lookups resolve the same way.
    includes = [f"include/{i}" for i in range(len(include_dirs)) if f"include/{i}" in used_dirs]
    (sdk / "flags.json").write_text(json.dumps({"flags": flags, "includes": includes}, indent=2) + "\n")
    shutil.copy2(a.shim, sdk / "game_shim.cpp")
    shutil.copy2(a.recomp, recomp / "ps2_recomp")
    shutil.copy2(a.config, recomp / "roadtrip.toml")
    if a.vu1recomp:
        shutil.copy2(a.vu1recomp, recomp / "ps2_vu1_recomp")

    h = hashlib.sha1()
    for p in sorted(x for x in a.resources.rglob("*") if x.is_file() and x.name != "build_id"):
        h.update(str(p.relative_to(a.resources)).encode())
        h.update(p.read_bytes())
    (sdk / "build_id").write_text(h.hexdigest() + "\n")
    print(f"bundle_sdk: {copied} headers, {len(flags)} flags, build id {h.hexdigest()[:12]}")


if __name__ == "__main__":
    main()
