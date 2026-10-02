#!/usr/bin/env python3
"""Record a play-through section for the regression suite (tests/regression/sections).

    python3 scripts/record_section.py 020_peach_town_jobs --start adventure_first_save --produces after_jobs

Opens the game in a window at normal speed, in deterministic time (so the recording replays
exactly), starting from a checkpoint's memory card (or a blank card with --start boot). Play from
boot as usual (Continue from slot 1 to load the checkpoint). While playing:
  F6  check the picture here (a golden frame)
  F5  mark a moment (shown in reports)
End the section with an in-game save, then close the window. The script writes
sections/<name>.movie and <name>.toml, and stores the memory card as checkpoint --produces.
"""

from __future__ import annotations

import argparse
import os
import shutil
import subprocess
import sys
from pathlib import Path

REPO = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(REPO / "tests/regression"))

from rtharness import DEFAULT_APP  # noqa: E402
from rtharness.movie import Movie  # noqa: E402

SECTIONS = REPO / "tests/regression/sections"
TEST_DATA = Path(os.environ.get("RT_TEST_DATA", REPO / "build/regression"))
DEFAULT_BASE = Path.home() / "Library/Application Support/RoadTripRecomp"
FAKE_CLOCK = 1_000_000_000  # must match rtharness.Game


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("name", help="section name, sorting in play order (e.g. 020_peach_town_jobs)")
    parser.add_argument("--start", default="boot", help="checkpoint to start from (default: blank card)")
    parser.add_argument("--produces", help="store the memory card afterwards as this checkpoint")
    parser.add_argument("--title", default="")
    parser.add_argument("--base-data", default=os.environ.get("RT_TEST_BASE", str(DEFAULT_BASE)))
    parser.add_argument("--app", default=str(DEFAULT_APP))
    args = parser.parse_args()

    base = Path(args.base_data)
    work = TEST_DATA / "record" / args.name
    shutil.rmtree(work, ignore_errors=True)
    data = work / "data"
    (data / "saves/mc1").mkdir(parents=True)
    for d in ("disc", "game"):
        os.symlink(base / d, data / d)
    if args.start == "boot":
        (data / "saves/mc0").mkdir()
    else:
        checkpoint = TEST_DATA / "checkpoints" / args.start
        if not checkpoint.is_dir():
            sys.exit(f"no checkpoint '{args.start}' in {TEST_DATA / 'checkpoints'}; run the suite first")
        shutil.copytree(checkpoint, data / "saves/mc0")
    if (base / "input.toml").exists():
        shutil.copy(base / "input.toml", data / "input.toml")

    movie_path = SECTIONS / f"{args.name}.movie"
    env = dict(os.environ, RT_DATA_DIR=str(data), RT_TIME="virtual", RT_SPEED="1",
               RT_FAKE_CLOCK=str(FAKE_CLOCK), RT_MOVIE_RECORD=str(movie_path))
    print(f"[record] playing from {args.start}; F6 = golden frame, F5 = marker; save in-game, then quit")
    subprocess.run([args.app], env=env, cwd=work)

    movie = Movie.load(movie_path)
    if movie.end is None:
        sys.exit("[record] the movie has no end line; did the game exit normally?")
    toml = [f'title = "{args.title or args.name}"', f'movie = "{movie_path.name}"', f'start = "{args.start}"']
    if args.produces:
        toml.append(f'produces = "{args.produces}"')
        dest = TEST_DATA / "checkpoints" / args.produces
        shutil.rmtree(dest, ignore_errors=True)
        shutil.copytree(data / "saves/mc0", dest)
    (SECTIONS / f"{args.name}.toml").write_text("\n".join(toml) + "\n")
    print(f"[record] {movie.changes} input changes, {len(movie.goldens())} golden frames, "
          f"{movie.end} vblanks ({movie.end / 59.94:.0f} s)")
    print(f"[record] now run: python3 scripts/regress.py -k {args.name} --update-goldens")


if __name__ == "__main__":
    main()
