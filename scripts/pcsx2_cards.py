#!/usr/bin/env python3
"""Move Road Trip saves between the regression suite's checkpoints and PCSX2 folder memory cards.

    python3 scripts/pcsx2_cards.py export <checkpoint> [--card RoadTripCheck.ps2]
    python3 scripts/pcsx2_cards.py import <card folder> <checkpoint>

export: writes the checkpoint's save into a PCSX2 *folder* memory card in PCSX2's memcards
directory. Select that card for slot 1 in PCSX2 (Settings > Memory Cards), boot the game, and
Continue: the save made by this build must load in the original-hardware emulator too.

import: copies Road Trip's save from a PCSX2 folder memory card into a checkpoint, so progress
made in PCSX2 can be the starting point of new sections (record them with record_section.py).

The progress in either direction is printed with tools/save_parser.py, so the two can be compared.
"""

from __future__ import annotations

import argparse
import os
import shutil
import sys
from pathlib import Path

REPO = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(REPO / "tools"))

import save_parser  # noqa: E402

TEST_DATA = Path(os.environ.get("RT_TEST_DATA", REPO / "build/regression"))
PCSX2_CARDS = Path.home() / "Library/Application Support/PCSX2/memcards"
SAVE_DIR = "BASLUS-20398"


def show(save_dir: Path):
    data = (save_dir / SAVE_DIR).read_bytes()
    for name, value in save_parser.decode(data).items():
        if not isinstance(value, bytes):
            print(f"  {name:14} {value}")


def export(checkpoint: str, card: str):
    src = TEST_DATA / "checkpoints" / checkpoint / SAVE_DIR
    if not src.is_dir():
        sys.exit(f"no save in checkpoint {checkpoint}")
    dest = PCSX2_CARDS / card
    if dest.exists() and not dest.is_dir():
        sys.exit(f"{dest} exists and is not a folder card")
    dest.mkdir(parents=True, exist_ok=True)
    target = dest / SAVE_DIR
    shutil.rmtree(target, ignore_errors=True)
    shutil.copytree(src, target)  # PCSX2 builds its index files (_pcsx2_*) itself
    print(f"exported to {dest}; select '{card}' for slot 1 in PCSX2 and Continue. Progress:")
    show(target)


def import_card(card: Path, checkpoint: str):
    src = card / SAVE_DIR
    if not (src / SAVE_DIR).is_file():
        sys.exit(f"no Road Trip save in {card} (expected {SAVE_DIR}/{SAVE_DIR}); only folder cards are supported")
    dest = TEST_DATA / "checkpoints" / checkpoint
    shutil.rmtree(dest, ignore_errors=True)
    dest.mkdir(parents=True)
    shutil.copytree(src, dest / SAVE_DIR, ignore=shutil.ignore_patterns("_pcsx2_*"))
    print(f"imported as checkpoint '{checkpoint}'. Progress:")
    show(dest / SAVE_DIR)


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = parser.add_subparsers(dest="cmd", required=True)
    e = sub.add_parser("export")
    e.add_argument("checkpoint")
    e.add_argument("--card", default="RoadTripCheck.ps2")
    i = sub.add_parser("import")
    i.add_argument("card", type=Path)
    i.add_argument("checkpoint")
    args = parser.parse_args()
    if args.cmd == "export":
        export(args.checkpoint, args.card)
    else:
        import_card(args.card, args.checkpoint)


if __name__ == "__main__":
    main()
