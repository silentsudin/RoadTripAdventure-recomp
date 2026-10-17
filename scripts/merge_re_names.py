#!/usr/bin/env python3
"""Merge verified agent naming proposals into config/game_symbols.toml.

Input: build/scratch/re-loop/named_batch*.jsonl (one JSON object per line:
addr, name, note), produced by the naming-loop subagents and spot-verified by
the coordinator (ELF string checks, pseudocode reads, pointer dereferences).

Validation (anything failing is skipped with a reason):
  * addr is a function in build/decomp/callgraph.json
  * addr has no name yet in config/game_symbols.toml
  * name is lowerCamel-ish and unique (collisions get an _0xADDR suffix)
  * note is non-empty

Run scripts/ghidra/decompile.py afterwards to apply the names to Ghidra.
"""
import glob
import json
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
TOML = ROOT / "config" / "game_symbols.toml"
BATCHES = sorted(ROOT.glob("build/scratch/re-loop/named_batch*.jsonl"))
# Valid C identifier (SDK/libgcc names like DIntr, _InitSys, __udivdi3 are correct
# as-is; Ghidra accepts them verbatim).
NAME_RE = re.compile(r"^[A-Za-z_][A-Za-z0-9_]*$")


def existing_addrs_and_names(text: str) -> tuple[set[str], set[str]]:
    addrs, names = set(), set()
    section = ""
    for line in text.splitlines():
        s = line.strip()
        if s.startswith("["):
            section = s
            continue
        if section != "[functions]":
            continue  # [data] entries must not block naming the same address as a function
        m = re.match(r"(0x[0-9A-Fa-f]{6,8})\s*=\s*\{\s*name\s*=\s*\"([^\"]+)\"", s)
        if m:
            addrs.add(m.group(1).lower())
            names.add(m.group(2))
    return addrs, names


def esc(s: str) -> str:
    return s.replace("\\", "\\\\").replace('"', "'").replace("\n", " ").strip()


def main() -> int:
    cg = json.loads((ROOT / "build/decomp" / "callgraph.json").read_text())
    cg_addrs = {a.lower() for a in cg}
    # Ghidra-keyed variants: callgraph keys may lack the 0x00 padding; normalise both.
    cg_norm = {a[-8:] for a in cg_addrs}
    text = TOML.read_text()
    have_addrs, have_names = existing_addrs_and_names(text)
    merged: list[str] = []
    skipped: list[str] = []
    for path in BATCHES:
        for ln, line in enumerate(path.read_text().splitlines(), 1):
            if not line.strip():
                continue
            try:
                obj = json.loads(line)
            except json.JSONDecodeError as e:
                skipped.append(f"{path.name}:{ln} bad json ({e})")
                continue
            addr = str(obj.get("addr", "")).lower()
            name = str(obj.get("name", "")).strip()
            note = esc(str(obj.get("note", "")))
            if not (len(addr) >= 10 and addr.startswith("0x")) or addr[-8:] not in cg_norm:
                skipped.append(f"{path.name}:{ln} {addr} not in callgraph")
                continue
            key = "0x" + addr[-8:].upper()
            if key.lower() in have_addrs:
                skipped.append(f"{path.name}:{ln} {key} already named")
                continue
            if not NAME_RE.match(name):
                skipped.append(f"{path.name}:{ln} {addr} bad name {name!r}")
                continue
            if not note:
                skipped.append(f"{path.name}:{ln} {addr} empty note")
                continue
            if name in have_names:
                name = f"{name}_0x{addr[-8:]}"
            have_addrs.add(key.lower())
            have_names.add(name)
            merged.append(f"{key} = {{ name = \"{name}\", note = \"{note}\" }}")
    if not merged:
        print("nothing to merge")
        for s in skipped:
            print("SKIP", s)
        return 0
    marker = "\n# --- Agent naming loop, verified wave 1 (2026-10-17): DeepSeek batch " \
            "proposals; coordinator spot-verified (ELF string reads, pseudocode reads, " \
            "pointer dereferences). [unclear] notes mark structural-only ids. ---\n"
    # Insert at the end of the [functions] section, not EOF (appending at EOF would
    # land under [data] and Ghidra would apply the names as data labels).
    lines = text.splitlines()
    insert_at = len(lines)
    started = False
    for i, ln in enumerate(lines):
        if ln.strip() == "[functions]":
            started = True
        elif started and ln.strip().startswith("["):
            insert_at = i
            break
    lines.insert(insert_at, marker + "\n".join(merged) + "\n")
    TOML.write_text("\n".join(lines) + "\n")
    print(f"merged {len(merged)} functions; skipped {len(skipped)}")
    for s in skipped:
        print("SKIP", s)
    return 0


if __name__ == "__main__":
    sys.exit(main())
