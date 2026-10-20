"""Attract-mode "card band" picture contract (native rebuild vs PS2Recomp oracle).

Ground truth: oracle (RT_GS_BACKEND=cpu, headless, RT_TIME=virtual). The band
x in [209,455), y in [136,149) of the 640x224 RGBA scanout must hash (SHA-256
prefix) to EXPECTED at each guest vblank. One headless native run dumps all
frames through the RT_NATIVE_GS_DUMP seam ("vb:path,..."; 12-byte header then
573440 raw scanout bytes).

Skips when the rt-app binary is missing (override with RT_APP_BIN).
"""

from __future__ import annotations

import hashlib
import os
import subprocess
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
RT_APP = Path(os.environ.get("RT_APP_BIN", ROOT / "build" / "w-G0-verify" / "rt-app"))

HEADER = 12
SCANOUT = 573440
W, H, BPP = 640, 224, 4
X0, X1, Y0, Y1 = 209, 455, 136, 149

OLD, NEW = "a7ca7a63d3c7a0c7", "3e45c5141106e978"
# Expectations from the per-vblank oracle drive (build/scratch/parity/oracle_per_vb_drive.py,
# frame label == park): oracle new-card window is park 1306..1337, i.e. native labels 1308..1336.
# (The first EXPECTED used /tmp/so_o*.raw, whose driver advanced 4 labels per 3 vblanks — wrong.)
EXPECTED = {**{vb: NEW for vb in range(1308, 1337, 4)},
            **{vb: OLD for vb in range(1340, 1349, 4)}}
SANITY = {1300: OLD, 1304: OLD}  # static old-card windows: validate dump/strip/stride
FRAMES = 1349


def band_hash(raw: bytes) -> str:
    stride = W * BPP
    band = b"".join(raw[y * stride + X0 * BPP: y * stride + X1 * BPP]
                    for y in range(Y0, Y1))
    return hashlib.sha256(band).hexdigest()[:16]


@pytest.mark.skipif(not RT_APP.exists(), reason=f"{RT_APP} not built")
def test_card_band_attract(tmp_path):
    vbs = sorted({**EXPECTED, **SANITY})
    dumps = {vb: tmp_path / f"cbt_{vb}.bin" for vb in vbs}
    env = dict(
        os.environ,
        RT_APP_NATIVE_BOOT="1", RT_APP_HEADLESS="1", RT_APP_NOAUDIO="1",
        RT_TIME="virtual", RT_APP_FRAMES=str(FRAMES),
        RT_NATIVE_GS_DUMP=",".join(f"{vb}:{p}" for vb, p in dumps.items()),
    )
    subprocess.run([str(RT_APP)], cwd=ROOT, env=env, timeout=1800,
                   stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)

    actual, bad_size = {}, []
    for vb, p in dumps.items():
        if not p.exists() or p.stat().st_size != HEADER + SCANOUT:
            bad_size.append((vb, p.stat().st_size if p.exists() else None))
            continue
        actual[vb] = band_hash(p.read_bytes()[HEADER:])
    assert not bad_size, f"dump size != {HEADER + SCANOUT}: {bad_size}"

    allexp = {**EXPECTED, **SANITY}
    print("\nvb    expected          actual            ok")
    for vb in vbs:
        print(f"{vb:<5} {allexp[vb]}  {actual[vb]}  {'OK' if actual[vb] == allexp[vb] else 'DIFF'}")

    # extraction sanity first: a mismatch here is a TEST bug, not a port defect
    for vb, exp in SANITY.items():
        assert actual[vb] == exp, f"SANITY vb{vb}: extraction wrong ({actual[vb]} != {exp})"

    diffs = [f"vb{vb}: expected {exp} got {actual[vb]}"
             for vb, exp in sorted(EXPECTED.items()) if actual[vb] != exp]
    assert not diffs, "card band differs from oracle:\n  " + "\n  ".join(diffs)
