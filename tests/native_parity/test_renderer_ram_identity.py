"""W16 renderer-independence pin: Vulkan and GL presenters produce
byte-identical guest RAM over the same deterministic attract run.

The GL backend must never touch guest state; if this ever diverges, the GL
draw path has started feeding (or reading) anything the EE sees.
Verified first by hand 2026-10-20 (60-frame attract, w16t build):
"IDENTITY: vulkan==gl RAM 60f" with "renderer=gl: all draw classes on GL,
no Vulkan device (W16.4)" in the gl run's log.
"""
import filecmp
import os
import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
RT_APP = Path(os.environ.get("RT_APP_BIN", ROOT / "build/w-G0-verify/rt-app"))
VBLANK = 60  # dump after 60 frames; RT_APP_FRAMES one higher so the dump flushes


def _run(renderer: str, dump: Path, tmp_path: Path) -> str:
    env = dict(
        os.environ,
        RT_APP_NATIVE_BOOT="1", RT_APP_HEADLESS="1", RT_APP_NOAUDIO="1",
        RT_TIME="virtual", RT_APP_FRAMES=str(VBLANK + 1),
        RT_APP_RENDERER=renderer, RT_RAM_DUMP=f"{VBLANK}:{dump}",
    )
    log = tmp_path / f"{renderer}.log"
    with log.open("wb") as output:
        result = subprocess.run([str(RT_APP)], cwd=ROOT, env=env, timeout=1800,
                                stdout=output, stderr=subprocess.STDOUT)
    tail = log.read_text(errors="replace")
    assert result.returncode == 0, f"{renderer} run exited {result.returncode}:\n{tail[-4000:]}"
    assert dump.exists(), f"{renderer} run produced no RAM dump:\n{tail[-4000:]}"
    return tail


def test_attract_ram_identical_vulkan_vs_gl(tmp_path):
    vk = tmp_path / "ram_vk.bin"
    gl = tmp_path / "ram_gl.bin"
    vk_log = _run("vulkan", vk, tmp_path)
    gl_log = _run("gl", gl, tmp_path)
    assert "renderer=gl: all draw classes on GL, no Vulkan device" in gl_log, (
        "RT_APP_RENDERER=gl did not select the GL backend:\n" + gl_log[-4000:])
    assert vk.read_bytes() == gl.read_bytes(), (
        "guest RAM diverged between vulkan and gl attract runs; "
        "first diff at "
        + hex(next(i for i, (a, b) in enumerate(zip(vk.read_bytes(), gl.read_bytes())) if a != b)))
    print(f"\nRAM identity pinned: vulkan==gl over 32 MiB at vblank {VBLANK}")
