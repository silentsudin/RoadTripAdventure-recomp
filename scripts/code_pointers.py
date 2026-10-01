"""Find code addresses the game builds with lui + addiu/ori pairs.

Stripped PS2 code installs callbacks by materialising a function address in a register
(`lui $v0, 0x22; addiu $v0, $v0, -0x31B8`) and storing or returning it. Nothing calls those
functions directly, so function discovery can miss them, and a later JALR to them kills the
thread with "missing-target". Every such address becomes a recompiler entry point.
"""
from __future__ import annotations

import struct
from pathlib import Path


def _sections(elf: bytes) -> dict[str, tuple[int, int, int]]:
    shoff = struct.unpack_from("<I", elf, 32)[0]
    shnum, shstrndx = struct.unpack_from("<HH", elf, 48)
    hdrs = [struct.unpack_from("<10I", elf, shoff + i * 40) for i in range(shnum)]
    strtab = hdrs[shstrndx][4]
    out = {}
    for h in hdrs:
        name = elf[strtab + h[0]: elf.index(b"\0", strtab + h[0])].decode()
        out[name] = (h[3], h[4], h[5])  # addr, offset, size
    return out


def find_code_pointers(elf_path: Path) -> list[int]:
    elf = elf_path.read_bytes()
    addr, off, size = _sections(elf)[".text"]
    lo, hi = addr, addr + size
    upper: dict[int, int] = {}
    found: set[int] = set()
    for i in range(0, size, 4):
        w = struct.unpack_from("<I", elf, off + i)[0]
        op, rs, rt, imm = w >> 26, (w >> 21) & 31, (w >> 16) & 31, w & 0xFFFF
        if op == 0x0F:  # lui
            upper[rt] = imm << 16
            continue
        if op in (0x09, 0x0D) and rs in upper:  # addiu / ori
            lo16 = imm - 0x10000 if op == 0x09 and imm & 0x8000 else imm
            value = (upper[rs] + lo16) & 0xFFFFFFFF
            if lo <= value < hi and value % 4 == 0:
                found.add(value)
        if op in (0x02, 0x03) or (op == 0 and (w & 0x3F) in (0x08, 0x09)):
            upper.clear()  # don't pair across j/jal/jr/jalr
    return sorted(found)


def entry_point_selectors(elf_path: Path) -> list[str]:
    return [f"codeptr_{a:08X}@0x{a:08X}" for a in find_code_pointers(elf_path)]
