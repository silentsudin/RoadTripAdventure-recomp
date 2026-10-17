#!/usr/bin/env python3
"""Compare a native rt-app frame with an oracle golden frame.

This is intentionally a measurement tool, not a release gate.  It normalizes
both inputs onto a letterboxed 320x180 RGB canvas before calculating metrics.
"""

from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path
from typing import Any

import numpy as np
from PIL import Image


CANVAS_SIZE = (320, 180)
HISTOGRAM_BINS = 32


def _ppm_tokens(data: bytes) -> tuple[list[bytes], int]:
    """Read PPM header tokens, allowing whitespace and # comments."""
    tokens: list[bytes] = []
    index = 0
    length = len(data)
    while len(tokens) < 4 and index < length:
        while index < length and data[index] in b" \t\r\n":
            index += 1
        if index < length and data[index] == ord("#"):
            while index < length and data[index] not in b"\r\n":
                index += 1
            continue
        start = index
        while index < length and data[index] not in b" \t\r\n#":
            index += 1
        if start != index:
            tokens.append(data[start:index])
    return tokens, index


def _load_p6(path: Path) -> np.ndarray:
    data = path.read_bytes()
    tokens, offset = _ppm_tokens(data)
    if len(tokens) != 4 or tokens[0] != b"P6":
        raise ValueError("not a P6 PPM")
    try:
        width, height, maximum = (int(token) for token in tokens[1:])
    except ValueError as exc:
        raise ValueError("invalid P6 dimensions or maximum value") from exc
    if width <= 0 or height <= 0 or maximum != 255:
        raise ValueError("P6 requires positive dimensions and a maximum value of 255")
    # The final header token must be followed by at least one whitespace byte.
    if offset >= len(data) or data[offset] not in b" \t\r\n":
        raise ValueError("P6 header is missing pixel-data separator")
    # Consume the mandatory separator only.  Consuming arbitrary whitespace
    # would corrupt a valid frame whose first pixel starts with 0x20 or 0x0a.
    separator = data[offset]
    offset += 1
    if separator == ord("\r") and offset < len(data) and data[offset] == ord("\n"):
        offset += 1
    expected = width * height * 3
    pixels = data[offset:]
    if len(pixels) != expected:
        raise ValueError(f"P6 has {len(pixels)} pixel bytes; expected {expected}")
    return np.frombuffer(pixels, dtype=np.uint8).reshape((height, width, 3)).copy()


def load_image(path: str | Path) -> np.ndarray:
    """Load a PNG or PPM into an HxWx3 uint8 RGB array."""
    image_path = Path(path)
    if not image_path.is_file():
        raise FileNotFoundError(f"image not found: {image_path}")
    if image_path.suffix.lower() == ".ppm":
        try:
            return _load_p6(image_path)
        except (OSError, ValueError):
            # Pillow is retained as a fallback for legal, non-P6 PPM variants.
            pass
    with Image.open(image_path) as image:
        return np.asarray(image.convert("RGB"), dtype=np.uint8).copy()


def letterbox(image: np.ndarray, size: tuple[int, int] = CANVAS_SIZE) -> np.ndarray:
    """Fit image inside an RGB canvas with black padding, preserving aspect ratio."""
    target_width, target_height = size
    height, width, channels = image.shape
    if channels != 3:
        raise ValueError("comparison images must be RGB")
    scale = min(target_width / width, target_height / height)
    resized_size = (max(1, round(width * scale)), max(1, round(height * scale)))
    resized = np.asarray(
        Image.fromarray(image, "RGB").resize(resized_size, Image.Resampling.BILINEAR),
        dtype=np.uint8,
    )
    canvas = np.zeros((target_height, target_width, 3), dtype=np.uint8)
    x = (target_width - resized_size[0]) // 2
    y = (target_height - resized_size[1]) // 2
    canvas[y:y + resized_size[1], x:x + resized_size[0]] = resized
    return canvas


def _parse_roi(roi: str | None) -> tuple[int, int, int, int] | None:
    if roi is None:
        return None
    try:
        values = tuple(int(value.strip()) for value in roi.split(","))
    except ValueError as exc:
        raise ValueError("ROI must be X,Y,W,H integers") from exc
    if len(values) != 4:
        raise ValueError("ROI must be X,Y,W,H")
    x, y, width, height = values
    if width <= 0 or height <= 0 or x < 0 or y < 0 or x + width > CANVAS_SIZE[0] or y + height > CANVAS_SIZE[1]:
        raise ValueError(f"ROI must lie within the normalized {CANVAS_SIZE[0]}x{CANVAS_SIZE[1]} canvas")
    return values


def _sobel_density(image: np.ndarray) -> float:
    gray = image.astype(np.float64).dot(np.array([0.299, 0.587, 0.114]))
    padded = np.pad(gray, 1, mode="edge")
    gx = ((padded[:-2, 2:] + 2 * padded[1:-1, 2:] + padded[2:, 2:])
          - (padded[:-2, :-2] + 2 * padded[1:-1, :-2] + padded[2:, :-2]))
    gy = ((padded[2:, :-2] + 2 * padded[2:, 1:-1] + padded[2:, 2:])
          - (padded[:-2, :-2] + 2 * padded[:-2, 1:-1] + padded[:-2, 2:]))
    return float(np.hypot(gx, gy).mean())


def _histogram_intersection(first: np.ndarray, second: np.ndarray) -> list[float]:
    intersections: list[float] = []
    for channel in range(3):
        first_hist = np.histogram(first[..., channel], bins=HISTOGRAM_BINS, range=(0, 256))[0]
        second_hist = np.histogram(second[..., channel], bins=HISTOGRAM_BINS, range=(0, 256))[0]
        intersections.append(float(np.minimum(first_hist, second_hist).sum() / first_hist.sum()))
    return intersections


def _verdict(intersection: float) -> str:
    if intersection < 0.35:
        return "FAR"
    if intersection <= 0.7:
        return "NEAR"
    return "CLOSE"


def compare_frames(oracle_path: str | Path, native_path: str | Path,
                   roi: str | tuple[int, int, int, int] | None = None,
                   label: str | None = None) -> dict[str, Any]:
    """Return deterministic calibration metrics for an oracle/native frame pair."""
    oracle = load_image(oracle_path)
    native = load_image(native_path)
    normalized_oracle = letterbox(oracle)
    normalized_native = letterbox(native)
    parsed_roi = _parse_roi(roi) if isinstance(roi, str) else roi
    if parsed_roi is not None:
        x, y, width, height = parsed_roi
        compared_oracle = normalized_oracle[y:y + height, x:x + width]
        compared_native = normalized_native[y:y + height, x:x + width]
    else:
        compared_oracle, compared_native = normalized_oracle, normalized_native

    difference = np.abs(compared_oracle.astype(np.int16) - compared_native.astype(np.int16))
    intersection_channels = _histogram_intersection(compared_oracle, compared_native)
    intersection = float(np.mean(intersection_channels))
    oracle_edge_density = _sobel_density(compared_oracle)
    native_edge_density = _sobel_density(compared_native)
    ratio = (native_edge_density / oracle_edge_density) if oracle_edge_density else None

    result: dict[str, Any] = {
        "label": label,
        "oracle": {"path": str(oracle_path), "size": [int(oracle.shape[1]), int(oracle.shape[0])]},
        "native": {"path": str(native_path), "size": [int(native.shape[1]), int(native.shape[0])]},
        "comparison_canvas": list(CANVAS_SIZE),
        "resize_mode": "letterbox-fit-bilinear",
        "roi": list(parsed_roi) if parsed_roi else None,
        "mean_abs_diff_per_channel": [float(value) for value in difference.mean(axis=(0, 1))],
        "channel_stats": {
            "oracle": {"mean": [float(value) for value in compared_oracle.mean(axis=(0, 1))],
                       "std": [float(value) for value in compared_oracle.std(axis=(0, 1))]},
            "native": {"mean": [float(value) for value in compared_native.mean(axis=(0, 1))],
                       "std": [float(value) for value in compared_native.std(axis=(0, 1))]},
        },
        "histogram_intersection_per_channel": intersection_channels,
        "histogram_intersection": intersection,
        "non_black_fraction": {
            "oracle": float(np.any(compared_oracle != 0, axis=2).mean()),
            "native": float(np.any(compared_native != 0, axis=2).mean()),
        },
        "edge_density": {"oracle": oracle_edge_density, "native": native_edge_density,
                         "native_to_oracle_ratio": ratio},
        "verdict": _verdict(intersection),
    }
    return result


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--oracle", required=True, help="oracle golden PNG or PPM")
    parser.add_argument("--native", required=True, help="native rt-app PNG or PPM")
    parser.add_argument("--roi", help="comparison-space ROI: X,Y,W,H on the 320x180 letterboxed canvas")
    parser.add_argument("--json", dest="json_path", help="write full metrics JSON to this path")
    parser.add_argument("--label", help="optional label included in the JSON")
    args = parser.parse_args(argv)
    try:
        result = compare_frames(args.oracle, args.native, args.roi, args.label)
    except (OSError, ValueError) as exc:
        parser.error(str(exc))
    output = json.dumps(result, indent=2, sort_keys=True)
    if args.json_path:
        destination = Path(args.json_path)
        destination.parent.mkdir(parents=True, exist_ok=True)
        destination.write_text(output + "\n", encoding="utf-8")
    print(output)
    return 0


if __name__ == "__main__":
    sys.exit(main())
