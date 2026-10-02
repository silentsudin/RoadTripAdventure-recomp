"""Real-time performance, as players run the game (normal timing, not lockstep). Opt-in: --perf."""

from __future__ import annotations

import os
import re
import statistics
import subprocess
from pathlib import Path

import pytest

QUICK_RACE = "10:start,13:down,15:cross,19:cross,23:cross,27:cross,31:cross,35:cross,39:cross,43:cross,47:cross:50"
FPS_LINE = re.compile(r"\[fps\] game ([\d.]+) fps .*?vif1/vu1 thread (\d+)% busy, gs thread (\d+)% busy")


@pytest.mark.perf
def test_quick_race_holds_60_fps(options, test_data):
    work = test_data / "runs" / "perf_quick_race"
    work.mkdir(parents=True, exist_ok=True)
    base = Path(options.base_data)
    env = dict(os.environ, RT_DATA_DIR=str(base), RT_SHOW_FPS="1", RT_INPUT_SCRIPT=QUICK_RACE,
               RT_EXIT_AT_VBLANK=str(95 * 60), RT_HEADLESS="1")
    log = subprocess.run([options.app], env=env, cwd=work, capture_output=True, text=True, timeout=600).stderr
    samples = [tuple(float(g) for g in m.groups()) for m in FPS_LINE.finditer(log)]
    race = samples[-12:]  # the last ~24 s: racing
    assert len(race) >= 8, "not enough [fps] samples; did the game run?"
    fps = statistics.median(s[0] for s in race)
    worker = max(s[1] for s in race)
    print(f"race: median {fps:.1f} fps, VIF1/VU1 thread up to {worker:.0f}% busy")
    assert fps >= 57, f"the race should hold 60 fps (median {fps:.1f})"
    assert worker <= 90, f"the VIF1/VU1 thread is nearly saturated ({worker:.0f}%)"
