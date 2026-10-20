#!/usr/bin/env python3
"""N1 (W13 fixture prereq): RT_PAD_SCRIPT drives the guest boot pad.

Pins the scheduler contract: each "vb:mask[:hold]" press is live for
hold+1 vblanks inclusive, scePadRead sees the active-low mask, and the
legacy RT_BOOT_START ladder is unchanged. Attract pads poll once per
port per vblank (2 reads/vblank), so trace counts pin the windows.
"""
import os
import re
import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
RT_APP = Path(os.environ.get("RT_APP_BIN", ROOT / "build/w-G0-verify/rt-app"))
FRAMES = 480


def run(script=None, boot_start=None, frames=FRAMES):
    env = dict(os.environ, RT_APP_NATIVE_BOOT="1", RT_APP_HEADLESS="1",
               RT_APP_NOAUDIO="1", RT_TIME="virtual", RT_APP_FRAMES=str(frames),
               RT_PAD_TRACE="1")
    env.pop("RT_PAD_SCRIPT", None)
    env.pop("RT_BOOT_START", None)
    if script:
        env["RT_PAD_SCRIPT"] = script
    if boot_start is not None:
        env["RT_BOOT_START"] = str(boot_start)
    out = subprocess.run([str(RT_APP)], env=env, capture_output=True,
                         text=True, timeout=300)
    assert out.returncode == 0, out.stderr[-800:]
    return out.stderr


def counts(err, mask):
    return len(re.findall(r"read buttons=" + mask + r"\b", err))


def first_at(err, mask):
    m = re.search(r"read buttons=" + mask + r"\b", err)
    return err.find(mask) if m else -1


def test_pad_script_windows_and_order():
    err = run(script="300:fff7,360:7fff:8")
    # press@300 hold 2 -> 3 vblanks * 2 ports; press@360 hold 8 -> 9 vblanks.
    # (masks arbitrary: fff7=Start, 7fff=bit15; scheduler is mask-agnostic)
    assert counts(err, "fff7") == 6, err[-800:]
    assert counts(err, "7fff") == 18, err[-800:]
    a, b = first_at(err, "fff7"), first_at(err, "7fff")
    assert 0 <= a < b, "press order wrong"


def test_legacy_boot_start_ladder_unchanged():
    # Up@300 (ffef), Cross@420 (bfff), Start@540 (fff7): frames must cover 542.
    err = run(boot_start=300, frames=620)
    assert counts(err, "ffef") == 6
    assert counts(err, "bfff") == 6
    assert counts(err, "fff7") == 6
