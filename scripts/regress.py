#!/usr/bin/env python3
"""Run the Road Trip regression suite (tests/regression).

    python3 scripts/regress.py                 # whole suite
    python3 scripts/regress.py -k smoke -n 4   # pytest arguments pass through (-n: parallel games)
    python3 scripts/regress.py --update-goldens

The suite runs the app headless in deterministic test mode against an installed data directory
(extracted disc + built game library): --base-data, $RT_TEST_BASE, or the app's own data directory.
With --rom (or $RT_ROM) and no installed data it installs into build/regression/base first.
Runs, checkpoints, golden images and failure diffs go to build/regression (never committed).
"""

from __future__ import annotations

import argparse
import os
import subprocess
import sys
import venv
from pathlib import Path

REPO = Path(__file__).resolve().parents[1]
SUITE = REPO / "tests/regression"
VENV = REPO / "build/regress-venv"
APP = REPO / "build/macos-release/RoadTrip.app/Contents/MacOS/RoadTrip"
DEFAULT_BASE = Path.home() / "Library/Application Support/RoadTripRecomp"


def ensure_venv() -> Path:
    python = VENV / "bin/python"
    stamp = VENV / ".requirements"
    requirements = (SUITE / "requirements.txt").read_text()
    if not python.exists():
        venv.create(VENV, with_pip=True)
    if not stamp.exists() or stamp.read_text() != requirements:
        subprocess.run([str(python), "-m", "pip", "install", "-q", "-r", str(SUITE / "requirements.txt")], check=True)
        stamp.write_text(requirements)
    return python


def prepare_base(base: Path, rom: str | None):
    """Makes sure `base` holds the extracted disc and a game library built for this app. A short
    headless launch does both (it installs from the ROM if needed and rebuilds the library when
    the app changed), so parallel test runs never race on it."""
    if not APP.exists():
        sys.exit(f"build the app first: {APP} is missing")
    installed = (base / "disc").is_dir()
    if not installed and not rom:
        sys.exit(f"no installed game in {base}; pass --rom <disc image> (or set RT_ROM)")
    env = dict(os.environ, RT_DATA_DIR=str(base), RT_HEADLESS="1", RT_EXIT_AT_VBLANK="2",
               RT_TIME="virtual", RT_SPEED="max")
    cmd = [str(APP)] + ([] if installed else ["--rom", rom])
    print(f"[regress] preparing {base} ...", flush=True)
    subprocess.run(cmd, env=env, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL, timeout=3600)
    if not (base / "game").is_dir():
        sys.exit(f"[regress] {base} has no built game library; run the app once to see why")


def check_no_game_data():
    """Golden images, checkpoints and dumps are game output and must stay out of git."""
    out = subprocess.run(["git", "status", "--porcelain", "--", "tests"], cwd=REPO, capture_output=True, text=True).stdout
    bad = [line for line in out.splitlines()
           if line[3:].endswith((".png", ".rgba", ".bin", ".ico")) or "/mc0/" in line]
    if bad:
        sys.exit("[regress] game output under tests/ must not be committed:\n" + "\n".join(bad))


def main():
    parser = argparse.ArgumentParser(add_help=False)
    parser.add_argument("--base-data", default=os.environ.get("RT_TEST_BASE"))
    parser.add_argument("--rom", default=os.environ.get("RT_ROM"))
    parser.add_argument("-h", "--help", action="store_true")
    args, pytest_args = parser.parse_known_args()
    if args.help:
        print(__doc__)
        return 0

    base = Path(args.base_data) if args.base_data else (DEFAULT_BASE if (DEFAULT_BASE / "disc").is_dir()
                                                        else REPO / "build/regression/base")
    python = ensure_venv()
    prepare_base(base, args.rom)
    check_no_game_data()
    return subprocess.run([str(python), "-m", "pytest", f"--base-data={base}", *pytest_args], cwd=SUITE).returncode


if __name__ == "__main__":
    sys.exit(main())
