"""Minimal ISO9660 reader for PS2 disc images (.cue/.bin MODE2/2352, MODE1/2352, or .iso 2048).

Mirrors src/rom/IsoReader.cpp. Used by the build pipeline to pull the boot ELF out of
the user's own disc image; never writes anything into the repository.
"""
from __future__ import annotations

import dataclasses
import os
import re
import struct
from pathlib import Path
from typing import BinaryIO, Iterator

SECTOR = 2048


@dataclasses.dataclass
class Entry:
    path: str          # e.g. "SYS/FONT.GSL" (version suffix stripped)
    lba: int
    size: int
    is_dir: bool


def resolve_image(path: str | os.PathLike) -> tuple[Path, int, int]:
    """Return (data file, raw sector size, user-data offset) for a .cue/.bin/.iso path."""
    p = Path(path).expanduser()
    if p.suffix.lower() == ".cue":
        text = p.read_text(errors="replace")
        m = re.search(r'FILE\s+"([^"]+)"', text) or re.search(r"FILE\s+(\S+)", text)
        if not m:
            raise ValueError(f"no FILE entry in {p}")
        p = p.parent / m.group(1)
    if not p.exists():
        raise FileNotFoundError(p)
    if _looks_raw(p):
        with p.open("rb") as f:
            f.seek(16 * 2352 + 15)
            mode = f.read(1)[0]
        return p, 2352, 24 if mode == 2 else 16
    return p, 2048, 0


def _looks_raw(p: Path) -> bool:
    with p.open("rb") as f:
        return f.read(12) == b"\x00" + b"\xff" * 10 + b"\x00"


class IsoImage:
    def __init__(self, path: str | os.PathLike):
        self.file, self.raw, self.offset = resolve_image(path)
        self.f: BinaryIO = self.file.open("rb")
        pvd = self.read_sectors(16, 1)
        if pvd[1:6] != b"CD001":
            raise ValueError("not an ISO9660 image (missing CD001 at sector 16)")
        self.volume_id = pvd[40:72].decode(errors="replace").strip()
        root = pvd[156:156 + 34]
        self.root = Entry("", struct.unpack_from("<I", root, 2)[0],
                          struct.unpack_from("<I", root, 10)[0], True)

    def close(self) -> None:
        self.f.close()

    def __enter__(self):
        return self

    def __exit__(self, *exc):
        self.close()

    def read_sectors(self, lba: int, count: int) -> bytes:
        if self.raw == SECTOR:
            self.f.seek(lba * SECTOR)
            return self.f.read(count * SECTOR)
        out = bytearray()
        for i in range(count):
            self.f.seek((lba + i) * self.raw + self.offset)
            out += self.f.read(SECTOR)
        return bytes(out)

    def read_file(self, e: Entry) -> bytes:
        return self.read_sectors(e.lba, (e.size + SECTOR - 1) // SECTOR)[: e.size]

    def iter_file(self, e: Entry, chunk_sectors: int = 512) -> Iterator[bytes]:
        remaining, lba = e.size, e.lba
        while remaining > 0:
            n = min(chunk_sectors, (remaining + SECTOR - 1) // SECTOR)
            data = self.read_sectors(lba, n)[:remaining]
            yield data
            remaining -= len(data)
            lba += n

    def list_dir(self, d: Entry) -> list[Entry]:
        data = self.read_sectors(d.lba, (d.size + SECTOR - 1) // SECTOR)
        out, i = [], 0
        while i < len(data):
            length = data[i]
            if length == 0:
                i = (i // SECTOR + 1) * SECTOR
                continue
            rec = data[i:i + length]
            name_len = rec[32]
            name = rec[33:33 + name_len]
            if name not in (b"\x00", b"\x01"):
                n = name.decode(errors="replace").split(";")[0]
                out.append(Entry(f"{d.path}/{n}" if d.path else n,
                                 struct.unpack_from("<I", rec, 2)[0],
                                 struct.unpack_from("<I", rec, 10)[0],
                                 bool(rec[25] & 2)))
            i += length
        return out

    def walk(self) -> Iterator[Entry]:
        stack = [self.root]
        while stack:
            for e in self.list_dir(stack.pop()):
                yield e
                if e.is_dir:
                    stack.append(e)

    def find(self, path: str) -> Entry:
        want = path.strip("/").upper()
        for e in self.walk():
            if e.path.upper() == want:
                return e
        raise FileNotFoundError(path)


if __name__ == "__main__":
    import sys

    with IsoImage(sys.argv[1]) as iso:
        print(f"volume: {iso.volume_id}")
        for e in iso.walk():
            print(f"{'D' if e.is_dir else 'F'} {e.lba:8d} {e.size:10d} {e.path}")
