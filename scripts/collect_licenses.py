#!/usr/bin/env python3
"""collect_licenses.py <cmake build dir> <out dir> [--llvm <llvm source dir>] [--ndk <ndk dir>]

Copies the licence of every third-party component the app ships (linked in, bundled or, for
Android, the on-device compiler) into <out dir>, one file each, plus THIRD_PARTY_NOTICES.txt with
all of them. The Mac bundle puts them in Resources/licenses, the APK in assets/licenses.
Fails if a component's licence can't be found, so a new dependency can't ship without one.
"""
import argparse
import sys
from pathlib import Path

REPO = Path(__file__).resolve().parent.parent
RUNTIME = REPO / "third_party/PS2Recomp/ps2xRuntime"

# (name, what it is, licence files relative to its root). Roots: "deps:<name>" is
# <build>/_deps/<name>-src; others are repo paths.
COMPONENTS = [
    ("PS2Recomp", "the recompiler and runtime", "repo:third_party/PS2Recomp", ["LICENSE"]),
    ("paraLLEl-GS", "the GPU GS; LGPL 3", "repo:third_party/PS2Recomp/ps2xRuntime/third_party/parallel-gs", ["COPYING.LGPLv3"]),
    ("Granite", "Vulkan backend of paraLLEl-GS", "repo:third_party/PS2Recomp/ps2xRuntime/third_party/parallel-gs/Granite", ["LICENSE"]),
    ("volk", "Vulkan loader (in Granite)", "repo:third_party/PS2Recomp/ps2xRuntime/third_party/parallel-gs/Granite/third_party/volk", ["LICENSE.md"]),
    ("SDL", "windows, input, audio", "deps:sdl3", ["LICENSE.txt"]),
    ("raylib", "host platform layer", "deps:raylib", ["LICENSE"]),
    ("Dear ImGui", "menus", "deps:imgui", ["LICENSE.txt"]),
    ("rlImGui", "ImGui on raylib", "deps:rlimgui", ["LICENSE"]),
    ("toml11", "settings files", "deps:toml11", ["LICENSE"]),
    ("sse2neon", "SSE on ARM", "deps:sse2neon", ["LICENSE"]),
    ("fmt", "formatting (recompiler)", "deps:fmt", ["LICENSE"]),
    ("ELFIO", "ELF reading (recompiler)", "deps:elfio", ["LICENSE.txt"]),
    ("rabbitizer", "MIPS decoding (recompiler)", "deps:rabbitizer", ["LICENSE"]),
    ("libdwarf", "debug info reading (recompiler); LGPL 2.1", "deps:libdwarf",
     ["COPYING", "src/lib/libdwarf/LIBDWARFCOPYRIGHT", "src/lib/libdwarf/LGPL.txt"]),
    ("libchdr", "CHD disc images (with LZMA SDK, miniz, Zstandard)", "build:libchdr-LICENSES.txt", []),
    ("Zstandard", "save state compression (BSD)", "deps:zstd", ["LICENSE"]),
    ("Fredoka", "the UI font (SIL Open Font License)", "repo:resources/fonts", ["OFL.txt"]),
    ("AMD FidelityFX FSR 1", "upscaling shaders (MIT)", "repo:third_party/PS2Recomp/ps2xRuntime/src/lib/gs/post/ffx", ["LICENSE.txt"]),
    ("SMAA", "anti-aliasing shaders and tables (MIT)", "repo:third_party/PS2Recomp/ps2xRuntime/src/lib/gs/post/smaa", ["LICENSE.txt"]),
    ("Snapdragon GSR", "upscaling shader (BSD-3-Clause)", "repo:third_party/PS2Recomp/ps2xRuntime/src/lib/gs/post/sgsr", ["LICENSE"]),
    ("Arm ASR", "temporal upscaler, from AMD FSR 2 (MIT)", "deps:arm_asr", ["LICENSES/MIT.txt"]),
    ("Arm ASTC Encoder", "texture pack compression (Apache-2.0)", "deps:astcenc", ["LICENSE.txt"]),
]


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("build")
    ap.add_argument("out")
    ap.add_argument("--llvm", help="LLVM source dir (Android: the on-device compiler, libc++)")
    ap.add_argument("--moltenvk", help="MoltenVK's LICENSE (Mac)")
    args = ap.parse_args()
    build, out = Path(args.build), Path(args.out)
    out.mkdir(parents=True, exist_ok=True)

    comps = list(COMPONENTS)
    if args.llvm:
        comps.append(("LLVM", "the on-device compiler (clang, lld) and libc++", f"abs:{args.llvm}", ["LICENSE.TXT"]))
    if args.moltenvk:
        comps.append(("MoltenVK", "Vulkan on Metal", f"absfile:{args.moltenvk}", []))

    notices, missing = [], []
    for name, what, root, files in comps:
        kind, _, where = root.partition(":")
        if kind == "build" or kind == "absfile":
            paths = [build / where if kind == "build" else Path(where)]
        else:
            base = {"repo": REPO / where, "deps": build / "_deps" / f"{where}-src", "abs": Path(where)}[kind]
            paths = [base / f for f in files]
        texts = []
        for p in paths:
            if not p.is_file():
                missing.append(f"{name}: {p}")
                continue
            texts.append(p.read_text(errors="replace"))
        if not texts:
            continue
        body = "\n\n".join(texts)
        (out / f"{name.replace(' ', '')}-LICENSE.txt").write_text(body)
        notices.append(f"{'=' * 78}\n{name} ({what})\n{'=' * 78}\n\n{body.strip()}\n")
    (out / "THIRD_PARTY_NOTICES.txt").write_text(
        "Road Trip recomp ships the following third-party software under their own licences.\n"
        "The app's source, including how these are built and linked, is public.\n\n" + "\n\n".join(notices))
    if missing:
        sys.exit("collect_licenses: missing licence files:\n  " + "\n  ".join(missing))
    print(f"collect_licenses: {len(notices)} components -> {out}")


if __name__ == "__main__":
    main()
