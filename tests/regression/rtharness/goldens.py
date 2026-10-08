"""Golden frames: exact picture hashes are committed (goldens.json); the images themselves are game
output and stay in the local test-data directory."""

from __future__ import annotations

import hashlib
import json
import os
from pathlib import Path

from . import Frame

GOLDENS_FILE = Path(__file__).resolve().parents[1] / "goldens.json"
# The default test data (conftest's --test-data): its goldens/ holds the paraLLEl-GS pictures.
REPO_TEST_DATA = Path(__file__).resolve().parents[3] / "build/regression"


class GoldenMismatch(AssertionError):
    pass


def _load() -> dict:
    return json.loads(GOLDENS_FILE.read_text()) if GOLDENS_FILE.exists() else {}


def _save(data: dict):
    GOLDENS_FILE.write_text(json.dumps(data, indent=2, sort_keys=True) + "\n")


def check_audio(name: str, audio: dict, update: bool = False):
    """Sound over a stretch of play is bit-exact between runs; compare its hash."""
    key = f"audio:{name}"
    goldens = _load()
    entry = goldens.get(key)
    actual = {"hash": audio["hash"], "frames": audio["frames"]}
    if update or entry is None:
        goldens[key] = actual
        _save(goldens)
        return
    if entry != actual:
        raise GoldenMismatch(f"sound '{name}' differs from the golden: {actual} (golden {entry}), "
                             f"rms {audio['rms']}, peak {audio['peak']}")


# ---------------------------------------------------------------------------------------- hardware GS
# The goldens are paraLLEl-GS pictures. The hardware GS (RT_GS_BACKEND=hw) draws the same scenes
# with the GPU's rasteriser and filtering at its render scale, so its pictures never hash the same;
# they are compared with the paraLLEl-GS image instead (the golden's PNG, kept locally beside the
# hashes), tolerating what upscaling and filtering move: each pixel is compared with the other
# picture's 3x3 neighbourhood (both ways), and a difference counts when a channel is off by more
# than HW_LEVEL. The picture fails when too many pixels differ overall, or when one block of the
# screen differs a lot (a missing or wrong object, not spread-out filtering noise).
HW_LEVEL = 48
HW_MAX_PERCENT = 3.0      # % of all pixels (spread-out filtering noise reaches 2)
HW_BLOCK = 32             # device pixels
HW_BLOCK_MAX_PERCENT = 30.0  # % of one block's pixels


def hardware_gs() -> bool:
    return os.environ.get("RT_GS_BACKEND", "") == "hw"


def _reference(name: str, test_data: Path, entry: dict) -> Path | None:
    """The paraLLEl-GS picture of this golden: this run's test data, else the default test data
    (build/regression), if its pixels still hash to the committed golden."""
    import numpy as np
    from PIL import Image

    for d in (Path(test_data) / "goldens", REPO_TEST_DATA / "goldens"):
        p = d / f"{name}.png"
        if p.exists():
            rgba = np.asarray(Image.open(p).convert("RGBA")).tobytes()
            if hashlib.sha256(rgba).hexdigest() == entry["sha256"]:
                return p
    return None


def tolerant_difference(a, b, radius: int = 1):
    """Per pixel: the largest channel difference to the best match within `radius` in the other
    picture, both ways (the larger of the two)."""
    import numpy as np

    h, w = a.shape[:2]
    a = a.astype(np.int16)
    b = b.astype(np.int16)
    pa = np.pad(a, ((radius, radius), (radius, radius), (0, 0)), mode="edge")
    pb = np.pad(b, ((radius, radius), (radius, radius), (0, 0)), mode="edge")
    da = np.full((h, w), 32767, np.int16)
    db = np.full((h, w), 32767, np.int16)
    for dy in range(-radius, radius + 1):
        for dx in range(-radius, radius + 1):
            sb = pb[radius + dy:radius + dy + h, radius + dx:radius + dx + w]
            sa = pa[radius + dy:radius + dy + h, radius + dx:radius + dx + w]
            da = np.minimum(da, np.abs(a - sb).max(axis=2))
            db = np.minimum(db, np.abs(b - sa).max(axis=2))
    return np.maximum(da, db)


def hw_verdict(expected, actual) -> tuple[bool, str, object]:
    """(passes, detail, difference) for a hardware-GS picture against the paraLLEl-GS one."""
    import numpy as np

    d = tolerant_difference(expected, actual)
    over = d > HW_LEVEL
    percent = float(over.mean()) * 100
    h, w = over.shape
    bh, bw = h // HW_BLOCK, w // HW_BLOCK
    blocks = over[:bh * HW_BLOCK, :bw * HW_BLOCK].reshape(bh, HW_BLOCK, bw, HW_BLOCK).mean(axis=(1, 3)) * 100
    worst = float(blocks.max()) if blocks.size else 0.0
    by, bx = np.unravel_index(int(blocks.argmax()), blocks.shape) if blocks.size else (0, 0)
    ok = percent <= HW_MAX_PERCENT and worst <= HW_BLOCK_MAX_PERCENT
    detail = (f"hardware GS vs paraLLEl-GS: {percent:.2f}% of pixels off by more than {HW_LEVEL} "
              f"(limit {HW_MAX_PERCENT}%), worst {HW_BLOCK}px block {worst:.0f}% at "
              f"({bx * HW_BLOCK},{by * HW_BLOCK}) (limit {HW_BLOCK_MAX_PERCENT:.0f}%)")
    return ok, detail, d


def _check_hw(name: str, frame: Frame, test_data: Path, entry: dict):
    import numpy as np
    from PIL import Image

    failures = Path(test_data) / "failures"
    reference = _reference(name, test_data, entry)
    if reference is None:
        raise GoldenMismatch(f"frame '{name}': no paraLLEl-GS picture of this golden to compare the hardware GS "
                             f"with (run the suite once with the default GS to make {name}.png)")
    expected = np.asarray(Image.open(reference).convert("RGB"))
    actual = frame.array()
    eh, ew = expected.shape[:2]
    ah, aw = actual.shape[:2]
    if (ah, aw) != (eh, ew) and ah % eh == 0 and aw % ew == 0:
        # A larger picture of the same scanout (progressive fields: the hardware GS keeps its
        # render scale's lines, paraLLEl-GS scans 2x and 4x out at 640x448): box-filtered down.
        fy, fx = ah // eh, aw // ew
        actual = actual.reshape(eh, fy, ew, fx, 3).mean(axis=(1, 3)).round().astype(np.uint8)
    if expected.shape != actual.shape:
        failures.mkdir(parents=True, exist_ok=True)
        frame.image().save(failures / f"{name}.actual.png")
        raise GoldenMismatch(f"frame '{name}': size {frame.width}x{frame.height} (paraLLEl-GS "
                             f"{expected.shape[1]}x{expected.shape[0]}); see {failures}")
    ok, detail, d = hw_verdict(expected, actual)
    if ok:
        return
    failures.mkdir(parents=True, exist_ok=True)
    frame.image().save(failures / f"{name}.actual.png")
    Image.fromarray(np.clip(d.astype(np.int32) * 4, 0, 255).astype("uint8")).save(failures / f"{name}.diff.png")
    raise GoldenMismatch(f"frame '{name}': {detail}; see {failures}")


def check(name: str, frame: Frame, test_data: Path, update: bool = False):
    digest = hashlib.sha256(frame.rgba).hexdigest()
    images = Path(test_data) / "goldens"
    images.mkdir(parents=True, exist_ok=True)
    goldens = _load()
    entry = goldens.get(name)
    if (update or entry is None) and hardware_gs():
        raise GoldenMismatch(f"frame '{name}': goldens are paraLLEl-GS pictures; make or update them without "
                             "RT_GS_BACKEND=hw")
    if update or entry is None:
        goldens[name] = {"sha256": digest, "width": frame.width, "height": frame.height}
        _save(goldens)
        frame.image().save(images / f"{name}.png")
        return
    if entry["sha256"] == digest:
        return
    if hardware_gs():
        _check_hw(name, frame, test_data, entry)
        return
    failures = Path(test_data) / "failures"
    failures.mkdir(parents=True, exist_ok=True)
    frame.image().save(failures / f"{name}.actual.png")
    detail = f"size {frame.width}x{frame.height} (golden {entry['width']}x{entry['height']})"
    reference = images / f"{name}.png"
    if reference.exists() and (frame.width, frame.height) == (entry["width"], entry["height"]):
        import numpy as np
        from PIL import Image

        expected = np.asarray(Image.open(reference).convert("RGB"), dtype=np.int16)
        actual = frame.array().astype(np.int16)
        diff = np.abs(actual - expected).max(axis=2)
        changed = float((diff > 8).mean()) * 100
        Image.fromarray((np.clip(diff * 4, 0, 255)).astype("uint8")).save(failures / f"{name}.diff.png")
        detail = f"{changed:.2f}% of pixels differ, max difference {int(diff.max())}"
    raise GoldenMismatch(f"frame '{name}' differs from the golden: {detail}; see {failures}")
