#!/usr/bin/env python3
"""Decode Road Trip's Adventure progress (a save file or the live RAM block) using
config/game_state.toml.

    python3 tools/save_parser.py <saves>/mc0/BASLUS-20398/BASLUS-20398
"""

from __future__ import annotations

import struct
import sys
import tomllib
from pathlib import Path

MAP_FILE = Path(__file__).resolve().parents[1] / "config/game_state.toml"


def load_map() -> dict:
    return tomllib.loads(MAP_FILE.read_text())


def decode(block: bytes, game_map: dict | None = None) -> dict:
    """Field name -> value for a progress block (save file contents or RAM)."""
    game_map = game_map or load_map()
    out = {}
    for name, f in game_map["progress"]["fields"].items():
        off = f["offset"]
        kind = f["type"]
        if kind == "u32":
            out[name] = struct.unpack_from("<I", block, off)[0]
        elif kind == "u16":
            out[name] = struct.unpack_from("<H", block, off)[0]
        elif kind == "u8":
            out[name] = block[off]
        elif kind == "f32":
            out[name] = struct.unpack_from("<f", block, off)[0]
        elif kind == "str":
            raw = block[off:off + f["length"]]
            out[name] = raw.split(b"\0", 1)[0].decode("latin-1")
        elif kind == "bytes":
            out[name] = block[off:off + f["length"]]
    return out


def main():
    if len(sys.argv) != 2:
        sys.exit(__doc__)
    block = Path(sys.argv[1]).read_bytes()
    expected = load_map()["progress"]["size"]
    if len(block) != expected:
        sys.exit(f"expected {expected} bytes, got {len(block)}")
    for name, value in decode(block).items():
        if isinstance(value, bytes):
            value = f"{len(value)} bytes, {sum(bin(b).count('1') for b in value)} bits set"
        print(f"{name:14} {value}")


if __name__ == "__main__":
    main()
