"""Attract card TEXT display-list contract in EE RAM (native rebuild vs oracle).

Provenance: PS2Recomp oracle EERM dump, oracle parked at vblank 1306 (after
{"cmd":"run","vblanks":1305}); the native rebuild is paired to it with
RT_RAM_DUMP="1306:<path>" (raw 32 MiB EE RAM, no header).

The AD block at 0x336320 must match the oracle qword by qword. The critical
entry is 0x336370/0x336378: TEX2_1 (reg 0x16) value psm=0x14, cbp=7104, csa=4,
i.e. bank<<56 | 0x13780001400000. Regression: uiDrawTextCell
(gen/00204310.cpp) and uiDrawNumberFixed (gen/002046c0.cpp) took the bank as
uint32 although Ghidra has `ulong param_4`; `param_4 << 0x38` was UB and the
store was dropped, leaving a stale TEXA constant under reg 0x16.

The 0x29a840 region is intentionally NOT pinned (ELF .reginfo loader gap,
known inert).

Skips when the rt-app binary is missing (override with RT_APP_BIN).
"""

from __future__ import annotations

import os
import struct
import subprocess
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
RT_APP = Path(os.environ.get("RT_APP_BIN", ROOT / "build" / "w-G0-verify" / "rt-app"))

RAM_SIZE = 32 << 20
VBLANK = 1306
FRAMES = VBLANK + 1

EXPECTED = {
    0x336320: 0x2013780E65321A40,
    0x336328: 0x06,
    0x336330: 0x05,
    0x336338: 0x08,
    0x336340: 0x10AB400000008054,
    0x336348: 0x0E,
    0x336350: 0x0000008000000044,
    0x336358: 0x42,
    0x336360: 0x0000004000008080,
    0x336368: 0x3B,
    0x336370: 0x0413780001400000,
    0x336378: 0x16,
    0x336380: 0x000000BC1843D01A,
    0x336388: 0x08,
}


@pytest.mark.skipif(not RT_APP.exists(), reason=f"{RT_APP} not built")
def test_text_list_tex2_ram(tmp_path):
    dump = tmp_path / "eeram_1306.bin"
    env = dict(
        os.environ,
        RT_APP_NATIVE_BOOT="1", RT_APP_HEADLESS="1", RT_APP_NOAUDIO="1",
        RT_TIME="virtual", RT_APP_FRAMES=str(FRAMES),
        RT_RAM_DUMP=f"{VBLANK}:{dump}",
    )
    subprocess.run([str(RT_APP)], cwd=ROOT, env=env, timeout=1800,
                   stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)

    assert dump.exists(), "RT_RAM_DUMP produced no file"
    assert dump.stat().st_size == RAM_SIZE, \
        f"RAM dump size {dump.stat().st_size} != {RAM_SIZE} (header added?)"

    ram = dump.read_bytes()
    actual = {a: struct.unpack_from("<Q", ram, a)[0] for a in EXPECTED}

    print("\naddr      expected            actual              ok")
    for a, exp in EXPECTED.items():
        print(f"{a:#08x}  {exp:#018x}  {actual[a]:#018x}  {'OK' if actual[a] == exp else 'DIFF'}")

    # the fix itself: value store under reg 0x16 must exist (not a stale TEXA)
    assert actual[0x336370] == 0x0413780001400000, \
        f"TEX2_1 value @0x336370 = {actual[0x336370]:#018x} (bank<<56 store dropped?)"
    assert actual[0x336378] == 0x16, f"TEX2_1 reg id @0x336378 = {actual[0x336378]:#x}"

    diffs = [f"{a:#x}: expected {exp:#018x} got {actual[a]:#018x}"
             for a, exp in EXPECTED.items() if actual[a] != exp]
    assert not diffs, "text display-list AD block differs from oracle:\n  " + "\n  ".join(diffs)
