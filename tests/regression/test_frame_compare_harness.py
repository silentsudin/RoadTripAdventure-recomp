"""Unit coverage for the standalone native/oracle frame comparison tool."""

from __future__ import annotations

import importlib.util
from pathlib import Path

import numpy as np
from PIL import Image


ROOT = Path(__file__).resolve().parents[2]
SPEC = importlib.util.spec_from_file_location("frame_compare", ROOT / "scripts" / "frame_compare.py")
assert SPEC is not None and SPEC.loader is not None
frame_compare = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(frame_compare)


def checker(width: int = 64, height: int = 48) -> np.ndarray:
    y, x = np.indices((height, width))
    board = ((x // 8 + y // 8) % 2 * 255).astype(np.uint8)
    return np.repeat(board[..., None], 3, axis=2)


def write_ppm(path: Path, pixels: np.ndarray) -> None:
    height, width, _ = pixels.shape
    path.write_bytes(f"P6\n{width} {height}\n255\n".encode() + pixels.tobytes())


def test_identical_images_are_close_with_exact_metrics(tmp_path: Path) -> None:
    pixels = checker()
    oracle = tmp_path / "oracle.png"
    native = tmp_path / "native.ppm"
    Image.fromarray(pixels, "RGB").save(oracle)
    write_ppm(native, pixels)

    result = frame_compare.compare_frames(oracle, native)

    assert result["mean_abs_diff_per_channel"] == [0.0, 0.0, 0.0]
    assert result["histogram_intersection"] == 1.0
    assert result["verdict"] == "CLOSE"


def test_opposite_gradients_are_far(tmp_path: Path) -> None:
    gradient = np.tile(np.arange(64, dtype=np.uint8), (48, 1))
    oracle_pixels = np.repeat(gradient[..., None], 3, axis=2)
    native_pixels = np.repeat((255 - gradient)[..., None], 3, axis=2)
    oracle = tmp_path / "oracle.png"
    native = tmp_path / "native.png"
    Image.fromarray(oracle_pixels, "RGB").save(oracle)
    Image.fromarray(native_pixels, "RGB").save(native)

    result = frame_compare.compare_frames(oracle, native)

    assert result["verdict"] == "FAR"
    assert result["histogram_intersection"] < 0.35


def test_metrics_are_deterministic_for_shifted_noisy_checker(tmp_path: Path) -> None:
    pixels = checker()
    shifted_noisy = np.roll(pixels, 3, axis=1).copy()
    shifted_noisy[::9, ::7] = 127
    oracle = tmp_path / "oracle.png"
    native = tmp_path / "native.png"
    Image.fromarray(pixels, "RGB").save(oracle)
    Image.fromarray(shifted_noisy, "RGB").save(native)

    first = frame_compare.compare_frames(oracle, native)
    second = frame_compare.compare_frames(oracle, native)

    assert first == second
