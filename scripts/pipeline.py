#!/usr/bin/env python3
"""Road Trip Adventure recomp build pipeline.

The app contains no game code: it recompiles the game from the player's own disc on first
launch. Building the app therefore needs no ROM:

  bootstrap    init the PS2Recomp submodule (our fork, branch roadtrip)
  build        configure + build build/<preset>/RoadTrip.app (runtime, recompiler, SDK headers)
  run          launch the app (first launch asks for the disc, then builds the game)

`all` = bootstrap + build. Developer steps for maintaining config/roadtrip.toml (need --rom):

  tools        build ps2_analyzer + ps2_recomp into build/tools
  extract-elf  pull SLUS_203.98 out of your disc image and verify it
  analyze      run ps2_analyzer and merge with config/roadtrip.base.toml
  recomp       run ps2_recomp -> generated/ (for reading the C++; the app builds its own copy)

Examples:
  python3 scripts/pipeline.py all run
  python3 scripts/pipeline.py tools extract-elf analyze --rom "/path/to/Road Trip (USA).cue"
"""
from __future__ import annotations

import argparse
import hashlib
import json
import os
import shutil
import subprocess
import sys
import zlib
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "scripts"))

from iso9660 import IsoImage  # noqa: E402
from code_pointers import entry_point_selectors  # noqa: E402
from merge_config import merge_config  # noqa: E402

UPSTREAM = ROOT / "third_party" / "PS2Recomp"
BUILD = ROOT / "build"
TOOLS_BUILD = BUILD / "tools"
GAME_BUILD = BUILD / "game"
ROM_OUT = BUILD / "rom"
GENERATED = ROOT / "generated"
ROM_INFO = json.loads((ROOT / "config" / "rom.json").read_text())
ELF_NAME = ROM_INFO["boot_elf"]

STEPS = ["bootstrap", "tools", "extract-elf", "analyze", "recomp", "build", "run"]


def log(msg: str) -> None:
    print(f"\033[1;34m==>\033[0m {msg}", flush=True)


def run(cmd: Sequence[str | os.PathLike], cwd: Path | None = None, check: bool = True) -> int:
    print("   $", " ".join(str(c) for c in cmd), flush=True)
    return subprocess.run([str(c) for c in cmd], cwd=cwd, check=check).returncode


def tool(name: str) -> Path:
    for candidate in TOOLS_BUILD.rglob(name):
        if candidate.is_file() and os.access(candidate, os.X_OK):
            return candidate
    sys.exit(f"{name} not found under {TOOLS_BUILD}; run the 'tools' step first")


# --------------------------------------------------------------------------- steps

def step_bootstrap(_: argparse.Namespace) -> None:
    log("Initialising PS2Recomp submodule")
    run(["git", "submodule", "update", "--init"], cwd=ROOT)
    # paraLLEl-GS (Vulkan GS) and only the Granite pieces it needs; a plain --recursive would
    # pull every Granite dependency.
    pgs = UPSTREAM / "ps2xRuntime" / "third_party" / "parallel-gs"
    run(["git", "submodule", "update", "--init", "ps2xRuntime/third_party/parallel-gs"], cwd=UPSTREAM)
    run(["git", "submodule", "update", "--init", "Granite"], cwd=pgs)
    run(["git", "submodule", "update", "--init", "third_party/volk", "third_party/khronos/vulkan-headers"],
        cwd=pgs / "Granite")
    if sys.platform == "darwin" and not shutil.which("brew"):
        print("note: install MoltenVK (brew install molten-vk) for the Vulkan GS backend")
    for exe in ("cmake", "ninja"):
        if not shutil.which(exe):
            sys.exit(f"missing '{exe}' (brew install cmake ninja)")


def step_tools(args: argparse.Namespace) -> None:
    log("Building host tools (ps2_analyzer, ps2_recomp)")
    run(["cmake", "-S", UPSTREAM, "-B", TOOLS_BUILD, "-G", "Ninja",
         "-DCMAKE_BUILD_TYPE=Release",
         "-DPS2X_BUILD_RUNTIME=OFF", "-DPS2X_BUILD_TEST=OFF", "-DPS2X_BUILD_STUDIO=OFF"])
    run(["cmake", "--build", TOOLS_BUILD, "--target", "ps2_analyzer", "ps2_recomp",
         "-j", str(args.jobs)])


def find_rom(args: argparse.Namespace) -> Path:
    rom = args.rom or os.environ.get("RT_ROM")
    if not rom:
        sys.exit("pass --rom <path to .cue/.bin/.iso> or set RT_ROM")
    return Path(rom).expanduser()


def step_extract_elf(args: argparse.Namespace) -> None:
    rom = find_rom(args)
    log(f"Extracting {ELF_NAME} from {rom}")
    with IsoImage(rom) as iso:
        if iso.volume_id != ROM_INFO["volume_id"]:
            print(f"warning: volume id is {iso.volume_id!r}, expected {ROM_INFO['volume_id']!r}")
        data = iso.read_file(iso.find(ELF_NAME))
    sha1 = hashlib.sha1(data).hexdigest()
    crc = f"0x{zlib.crc32(data):08x}"
    expected = ROM_INFO["elf"]
    if sha1 != expected["sha1"]:
        msg = f"ELF sha1 {sha1} does not match the known SLUS-20398 build ({expected['sha1']})"
        if not args.force:
            sys.exit(msg + "\nre-run with --force to continue anyway")
        print("warning:", msg)
    ROM_OUT.mkdir(parents=True, exist_ok=True)
    (ROM_OUT / ELF_NAME).write_bytes(data)
    print(f"   wrote {ROM_OUT / ELF_NAME} ({len(data)} bytes, sha1 {sha1}, crc32 {crc})")


def step_analyze(_: argparse.Namespace) -> None:
    elf = ROM_OUT / ELF_NAME
    if not elf.exists():
        sys.exit("run the 'extract-elf' step first")
    raw = BUILD / "analyzer.toml"
    log("Running ps2_analyzer")
    run([tool("ps2_analyzer"), elf, raw])
    log("Merging analyzer output with config/roadtrip.base.toml")
    pointers = entry_point_selectors(elf)
    print(f"   {len(pointers)} lui/addiu code pointers found in .text")
    merge_config(raw, ROOT / "config" / "roadtrip.base.toml", ROOT / "config" / "roadtrip.toml",
                 elf_path=elf, output_dir=GENERATED, extra_entry_points=pointers)


def step_recomp(_: argparse.Namespace) -> None:
    cfg = ROOT / "config" / "roadtrip.toml"
    if not cfg.exists():
        sys.exit("run the 'analyze' step first")
    if GENERATED.exists():
        shutil.rmtree(GENERATED)
    GENERATED.mkdir()
    log("Running ps2_recomp")
    run([tool("ps2_recomp"), cfg], cwd=ROOT)
    count = len(list(GENERATED.glob("*.cpp")))
    print(f"   {count} C++ files in {GENERATED}")


def step_build(args: argparse.Namespace) -> None:
    log("Building RoadTrip.app")
    run(["cmake", "--preset", args.preset], cwd=ROOT)
    run(["cmake", "--build", "--preset", args.preset, "-j", str(args.jobs)], cwd=ROOT)


def step_run(args: argparse.Namespace) -> None:
    app = next(BUILD.glob(f"{args.preset}/**/RoadTrip.app"), None)
    if not app:
        sys.exit("RoadTrip.app not found; run the 'build' step first")
    cmd = [app / "Contents" / "MacOS" / "RoadTrip"]
    if args.rom:
        cmd += ["--rom", Path(args.rom).expanduser()]
    run(cmd, check=False)


HANDLERS = {
    "bootstrap": step_bootstrap,
    "tools": step_tools,
    "extract-elf": step_extract_elf,
    "analyze": step_analyze,
    "recomp": step_recomp,
    "build": step_build,
    "run": step_run,
}


def main() -> None:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("steps", nargs="+", choices=STEPS + ["all"])
    ap.add_argument("--rom", help="path to your Road Trip (USA) .cue/.bin/.iso (or set RT_ROM)")
    ap.add_argument("--preset", default="macos-release", help="CMake preset for build/run")
    ap.add_argument("--force", action="store_true", help="continue on ELF hash mismatch")
    ap.add_argument("-j", "--jobs", type=int, default=os.cpu_count() or 4)
    args = ap.parse_args()

    wanted = set(args.steps) | ({"bootstrap", "build"} if "all" in args.steps else set())
    steps = [s for s in STEPS if s in wanted]
    for s in steps:
        HANDLERS[s](args)


if __name__ == "__main__":
    main()
