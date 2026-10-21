#!/usr/bin/env python3
"""Decompile the game's ELF with headless Ghidra and export pseudocode and a call graph.

    python3 scripts/ghidra/decompile.py [--elf PATH] [--only 0x2263B8,0x227048] [--reimport]

The first run imports the ELF into a Ghidra project (build/decomp/ghidra) with the Emotion Engine
add-on's r5900 language, applies our names, analyses once and exports. Later runs reuse the
project: they re-apply the names (config/game_symbols.toml and friends) and export again, with
no re-analysis. Everything written is derived from the game and stays under build/ (never commit).

Outputs (build/decomp/):
    fn/<addr>.c       pseudocode per function (8 lower-case hex digits)
    callgraph.json    {"0x...": {"name", "end", "callers", "callees"}}
    symbols.tsv       the names that were applied

Names come from config/roadtrip.toml (SDK stubs), config/game_state.toml (any address in RAM;
names gs_<table>_<key>) and config/game_symbols.toml (ours, which win). Function starts known to
the recompiler (generated/*.cpp names) seed the analysis when that directory exists.

Needs Ghidra 12 (GHIDRA_INSTALL_DIR or Homebrew's) and the ghidra-emotionengine-reloaded add-on.
"""
from __future__ import annotations

import argparse
import os
import re
import subprocess
import sys
import tomllib
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
HERE = Path(__file__).resolve().parent
LANGUAGE = "r5900:LE:32:default"
TEXT_END = 0x28E730  # .text ends here; .vutext (VU microcode) follows
PROJECT = "RoadTrip"
PROGRAM = "SLUS_203.98"


def find_ghidra(arg: str | None) -> Path:
    cands = [arg, os.environ.get("GHIDRA_INSTALL_DIR")]
    brew = Path("/opt/homebrew/Cellar/ghidra")
    if brew.is_dir():
        cands += [str(p / "libexec") for p in sorted(brew.iterdir(), reverse=True)]
    for c in cands:
        if c and (Path(c) / "support" / "analyzeHeadless").exists():
            return Path(c)
    sys.exit("Ghidra not found: set GHIDRA_INSTALL_DIR or pass --ghidra")


def find_elf(arg: str | None) -> Path:
    cands = [arg, os.environ.get("RT_ELF")]
    if os.environ.get("RT_DATA_DIR"):
        cands.append(str(Path(os.environ["RT_DATA_DIR"]) / "disc" / PROGRAM))
    cands += [str(Path.home() / "Library/Application Support/RoadTripRecomp/disc" / PROGRAM),
              str(ROOT / "build/rom" / PROGRAM)]
    for c in cands:
        if c and Path(c).is_file():
            return Path(c)
    sys.exit("boot ELF not found: pass --elf")


def main_checkout() -> Path:
    """The main working tree (generated/ lives there when this runs from a worktree)."""
    try:
        common = subprocess.run(["git", "rev-parse", "--git-common-dir"], cwd=ROOT,
                                capture_output=True, text=True, check=True).stdout.strip()
        return (ROOT / common).resolve().parent
    except Exception:
        return ROOT


def symbols() -> list[tuple[int, str, str, str]]:
    """(address, kind, name, note); later sources win for the same address."""
    out: dict[int, tuple[int, str, str, str]] = {}

    def add(addr: int, name: str, note: str = "", kind: str | None = None) -> None:
        if kind is None:
            kind = "fn" if 0x200000 <= addr < TEXT_END else "data"
        out[addr] = (addr, kind, re.sub(r"[^A-Za-z0-9_]", "_", name), note.replace("\t", " ").replace("\n", " "))

    rt = tomllib.loads((ROOT / "config/roadtrip.toml").read_text())
    for s in rt.get("general", {}).get("stubs", []):
        if "@" in s:
            name, a = s.split("@", 1)
            add(int(a, 16), name, kind="fn")

    def walk(prefix: str, node) -> None:
        if isinstance(node, dict):
            for k, v in node.items():
                walk(f"{prefix}_{k}" if prefix else k, v)
        elif isinstance(node, int) and 0x100000 <= node < 0x2000000:
            add(node, "gs_" + prefix)

    gs = ROOT / "config/game_state.toml"
    if gs.exists():
        walk("", tomllib.loads(gs.read_text()))

    ours = ROOT / "config/game_symbols.toml"
    if ours.exists():
        doc = tomllib.loads(ours.read_text())
        for table, kind in (("functions", "fn"), ("data", "data")):
            for a, v in doc.get(table, {}).items():
                if isinstance(v, str):
                    v = {"name": v}
                add(int(a, 0), v["name"], v.get("note", ""), kind)
    return sorted(out.values())


def function_starts() -> list[int]:
    if os.environ.get("RT_GHIDRA_NOSEED"):
        return []  # an independent analysis (docs/recompiler-accuracy.md compares it with the recompiler's set)
    starts = set()
    # table-called handlers the recompiler found as code pointers (e.g. the HUD
    # dispatch tables); Ghidra alone leaves them inside their neighbours.
    rt = tomllib.loads((ROOT / "config/roadtrip.toml").read_text())
    for s in rt.get("general", {}).get("entry_points", []):
        name, _, a = s.partition("@")
        if name.startswith("codeptr_"):
            starts.add(int(a, 16))
    for base in (ROOT, main_checkout()):
        gen = base / "generated"
        if gen.is_dir():
            for p in gen.glob("*.cpp"):
                m = re.search(r"_0x([0-9a-fA-F]+)\.cpp$", p.name)
                if m:
                    starts.add(int(m.group(1), 16))
            return sorted(starts)
    return sorted(starts)


def main() -> None:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--elf")
    ap.add_argument("--ghidra")
    ap.add_argument("--out", default=str(ROOT / "build/decomp"))
    ap.add_argument("--only", default="", help="comma-separated function addresses to re-export")
    ap.add_argument("--reimport", action="store_true", help="delete the project and analyse again")
    args = ap.parse_args()

    ghidra = find_ghidra(args.ghidra)
    out = Path(args.out).resolve()
    proj = out / "ghidra"
    proj.mkdir(parents=True, exist_ok=True)
    if args.reimport:
        for p in proj.glob(PROJECT + ".*"):
            subprocess.run(["rm", "-rf", str(p)], check=True)

    syms = symbols()
    sym_file = out / "symbols.tsv"
    sym_file.write_text("".join(f"0x{a:08X}\t{k}\t{n}\t{note}\n" for a, k, n, note in syms))
    fn_file = out / "function_starts.txt"
    fn_file.write_text("".join(f"0x{a:08X}\n" for a in function_starts()))

    fresh = not (proj / (PROJECT + ".gpr")).exists()
    proj_arg = proj
    if any(part.startswith(".") for part in proj.parts):
        # Ghidra refuses project paths with a dot-directory (worktrees live under .claude/): reach
        # the project through a link elsewhere; the files stay in build/.
        import hashlib
        import tempfile
        link = Path(tempfile.gettempdir()) / ("rt-decomp-" + hashlib.sha1(str(proj).encode()).hexdigest()[:10])
        if link.is_symlink() and link.resolve() != proj:
            link.unlink()
        if not link.exists():
            link.symlink_to(proj, target_is_directory=True)
        proj_arg = link
    cmd = [str(ghidra / "support/analyzeHeadless"), str(proj_arg), PROJECT]
    if fresh:
        cmd += ["-import", str(find_elf(args.elf)), "-processor", LANGUAGE]
    else:
        cmd += ["-process", PROGRAM, "-noanalysis"]
    cmd += ["-scriptPath", str(HERE),
            "-preScript", "RtApplyNames.java", str(sym_file), str(fn_file),
            "-postScript", "RtExport.java", str(out)] + ([args.only] if args.only else [])
    log = out / "ghidra.log"
    print(("importing and analysing" if fresh else "re-applying names and exporting") + f" (log: {log})")
    with open(log, "w") as f:
        r = subprocess.run(cmd, stdout=f, stderr=subprocess.STDOUT)
    for line in log.read_text(errors="replace").splitlines():
        if "RtApplyNames:" in line or "RtExport:" in line or "ERROR" in line:
            print(line.strip())
    if r.returncode != 0:
        sys.exit(f"analyzeHeadless failed ({r.returncode}); see {log}")


if __name__ == "__main__":
    main()
