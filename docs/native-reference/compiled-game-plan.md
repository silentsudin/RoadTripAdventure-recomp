# Compiled-Game Plan — Ghidra pseudocode → bit-perfect native game (W10+)

Goal (user mandate 2026-10-18): take the Ghidra pseudocode (`build/decomp/fn/<addr>.c`,
1542 functions) and get it into a fully working game, **bit perfect to the game running
on a PS2** as measured by our emulator (rt-app-oracle / goldens / state hashes).

## Architecture decision (recorded)

1. **Guest image, not hand-typed structs (at first).** The translated EE code runs
   against a host-owned RAM image: 32 MB arena in `src/native/guest/`, initialized from
   the user's boot ELF exactly like the emulator's loader (.text/.data/.rodata/.lit4/
   .vutext loaded, .bss/.sbss zeroed). Guest pointers stay 32-bit addresses; every
   dereference in translated code goes through one typed accessor `G(addr)`.
   Consequence: the translated game builds the **same EE-RAM byte state** as the
   emulator, so bit-perfection is proven by per-vblank RAM-hash diff vs the oracle —
   the exact mechanism `RT_STATE_HASH_FULL` / `state_hash` already exposes.
   Named structs (guest_vehicle style) layer ON TOP via typed global aliases as
   subsystems get verified; they never change the bytes.
2. **Direct calls where Ghidra resolved them; a host registry for the rest.** The
   game's task/function-pointer tables are data in the image; indirect-call sites go
   through `rtCallGuest(addr, …)` backed by a compile-time-registered table of OUR
   translated functions. This is our own dispatch in src/native — NOT PS2Recomp
   machinery, no generated/*.cpp, no dlopen, no guest execution of EE code. HARD RULE
   respected: every EE game routine shipped = translation of Ghidra pseudocode.
3. **Hardware models stay host-side.** GS (hardware backend + paraLLEl goldens),
   IOP+SPU2 (their modules were never part of the 1542 denominator), VU0/VU1
   interpreters (microcode is .vutext DATA, not .text functions), GIF/VIF1. The
   denominator of "compiled game" = the 1542 ELF functions; the VU microcode programs
   keep their vu1hle reference validation.
4. **Float/int semantics**: game thread runs with EE float mode (RZ+FZ, MXCSR RC+FTZ+DAZ,
   ps2_ee_fpu_mode trick) so translated FPU ops round exactly as the EE; Ghidra's
   truncating-div/mul idioms map to the ee_math helpers (already proven in W9-03).

## Bit-perfect ladder (gates, in order — each one measurable)

- G0 **compile**: transliterated TU builds (`rt_guest` static lib). Denominator 1542.
- G1 **link**: registry closes — every call target resolves to a translated fn or an
  explicit HLE/hardware stub (stub list is public, shrinking).
- G2 **pure-fn parity**: transliterated fn vs oracle guest `fn_call` probe on identical
  RAM slices — per-batch unit tests (the W9-03 template, automated).
- G3 **boot parity**: native EE image + hardware models boots the game to attract;
  EE-RAM state hashes match oracle at fixed vblank checkpoints.
- G4 **scene parity**: race/town/menus state-hash-clean + scanout within the
  accepted-deviation ledger (same as today's hw-GS-vs-pgs tolerance).
- G5 **cutover**: rt-app drives the ported game; authored subsystems retire as their
  ported replacements hit G4; perf ledger native-vs-emulator published.

## Waves

- **W10 (now)**: infra — shim (`guest_shim.h`), transliterator
  (`scripts/ghidra/transliterate.py`), `rt_guest` CMake target, token inventory,
  dependency-ordered work packages (`build/scratch/port/packages.json`), pilot
  translation of the task walker (0x204F98) + gameMainLoop (0x2003B8) → G0/G2 for
  the pilot.
  - G0 met 2026-10-19: all 1463 corpus TUs compile clean (`clang++ -std=c++17
    -fsyntax-only`, sweep13) + `rt_guest` static lib; 53 TUs held for hand work
    (array_cast 38, packed_vec 10, wide_field 2, arrow_field 2, named_field 1,
    varargs 1 — ledger in `build/scratch/g0/report.json`).
- **W11**: mass translate in dependency order (leaf-first). Parallel agents per
  package (≤40 fns), write only into `src/native/guest/gen/` (+ their package test);
  coordinator compiles, spot-verifies against pseudocode, merges names into
  game_symbols.toml. Progress = fns compiling + passing G2.
- **W12**: link closure + boot to attract (G1+G3); fix loop by state-hash diff.
- **W13**: scene parity race → town → menus (G4), state-hash chasing.
- **W14**: IOP-side HLE completion (loaders/snd/pad/mc), audio + save bit-parity.
- **W15**: cutover + perf ledger + authored-subsystem retirement (G5).

## Agent discipline (amended for this program)

Agents may write ONLY: `src/native/guest/gen/` (transliterated TUs),
`tests/native_guest/` (package tests), `build/scratch/port/` (reports).
They must not touch: shim/CMakeLists/config/game_symbols/AGENTS.md/git.
Coordinator (me) owns: shim, CMake, merges, symbol registry, verification commits.

## Progress ledger

`scripts/ported_count.py`: counts `// guest:0xXXXXXXXX` markers in `src/native/guest/`
against the 1542 denominator; per-gate column maintained in `docs/native-reference/
wave-board` rows W10+.
