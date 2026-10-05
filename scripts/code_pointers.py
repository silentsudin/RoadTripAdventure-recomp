"""Find code addresses the game builds with lui + addiu/ori pairs.

Stripped PS2 code installs callbacks by materialising a function address in a register
(`lui $v0, 0x22; addiu $v0, $v0, -0x31B8`) and storing or returning it. Nothing calls those
functions directly, so function discovery can miss them, and a later JALR to them kills the
thread with "missing-target". Every such address becomes a recompiler entry point.

Function-pointer tables in data sections (task/state tables, vtables) are found too: an aligned
data word that points into .text counts when its target looks like a function start (a stack
frame `addiu $sp, $sp, -N`, or right after a `jr $ra` + delay slot and any alignment padding).
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
    found |= _data_table_pointers(elf, lo, hi, off, addr)
    return sorted(found)


def _looks_like_function_start(elf: bytes, text_off: int, text_addr: int, target: int) -> bool:
    w = struct.unpack_from("<I", elf, text_off + (target - text_addr))[0]
    if (w & 0xFFFF0000) == 0x27BD0000 and (w & 0x8000):  # addiu $sp, $sp, -N
        return True
    # jr $ra + delay slot right before, possibly followed by alignment padding (zero words)
    for pad in range(0, 4):
        jr = target - 8 - 4 * pad
        if jr < text_addr:
            break
        if pad and struct.unpack_from("<I", elf, text_off + (target - 4 * pad - text_addr))[0] != 0:
            break
        if struct.unpack_from("<I", elf, text_off + (jr - text_addr))[0] == 0x03E00008:
            return True
    return False


def _data_table_pointers(elf: bytes, lo: int, hi: int, text_off: int, text_addr: int) -> set[int]:
    found: set[int] = set()
    for name, (addr, off, size) in _sections(elf).items():
        if name in (".text", ".bss", ".sbss", "") or addr == 0 or name.startswith((".debug", ".mdebug", ".comment", ".DVP", ".vutext", ".vudata")):
            continue
        for i in range(0, size - 3, 4):
            v = struct.unpack_from("<I", elf, off + i)[0]
            if lo <= v < hi and v % 4 == 0 and _looks_like_function_start(elf, text_off, text_addr, v):
                found.add(v)
    return found


def entry_point_selectors(elf_path: Path) -> list[str]:
    return [f"codeptr_{a:08X}@0x{a:08X}" for a in find_code_pointers(elf_path)]
