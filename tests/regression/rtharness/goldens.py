"""Golden frames: exact picture hashes are committed (goldens.json); the images themselves are game
output and stay in the local test-data directory."""

from __future__ import annotations

import hashlib
import json
from pathlib import Path

from . import Frame

GOLDENS_FILE = Path(__file__).resolve().parents[1] / "goldens.json"


class GoldenMismatch(AssertionError):
    pass


def _load() -> dict:
    return json.loads(GOLDENS_FILE.read_text()) if GOLDENS_FILE.exists() else {}


def _save(data: dict):
    GOLDENS_FILE.write_text(json.dumps(data, indent=2, sort_keys=True) + "\n")


def check(name: str, frame: Frame, test_data: Path, update: bool = False):
    digest = hashlib.sha256(frame.rgba).hexdigest()
    images = Path(test_data) / "goldens"
    images.mkdir(parents=True, exist_ok=True)
    goldens = _load()
    entry = goldens.get(name)
    if update or entry is None:
        goldens[name] = {"sha256": digest, "width": frame.width, "height": frame.height}
        _save(goldens)
        frame.image().save(images / f"{name}.png")
        return
    if entry["sha256"] == digest:
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
