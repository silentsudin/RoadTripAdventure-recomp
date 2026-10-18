# W2-12 Report — Input mapping (device -> PS2 action masks, remap, menu nav)

**Batch owner:** subagent (autonomous per wave2 rules)  
**Date:** 2026-10-18  
**Branch:** re/native-complete (working dir /Users/holzr/github/RoadTripAdventure-decomp)  
**Private build:** build/w-W2-12 only

## Summary

Implemented `src/native/gameplay/input_map.{h,cpp}` (SDL-agnostic):

- Default remap table (10 u16) exactly matching ELF stores at FUN_00229F20 (0x229F20) / save +0x12E8.
- Keyboard + gamepad profiles each covering all 16 physical button bits with 0 conflicts.
- Deterministic per-tick collector (last-write-wins, shuffle-proof).
- MenuNav resolver with grounded bit semantics + explicit GAPs.
- ELF reader helper (read-only, offset = vaddr-0x1FF000) for test verification.
- Pure numeric fixtures + repro in test.

All owned files created. No other files touched. No git state changes. ROM bytes never written.

## Owned files delivered

- `src/native/gameplay/input_map.h`
- `src/native/gameplay/input_map.cpp`
- `tests/native_w2/input_map_test.cpp`
- `docs/native-reference/input-map.md`
- `.qoder/wave2/W2-12-report.md`
- (no data/ fixtures needed beyond in-test vectors)

## Key numbers + reproducibility

Acceptance (from wave2-plan-sonnet.md §W2-12):

```
W2-12 SCORE default_remap=10/10 conflicts=0 bits_mapped=16/16 roundtrip_save=1000/1000 order_deterministic=1
```

Observed (with ELF):

```
W2-12 SCORE default_remap=10/10 conflicts=0 bits_mapped=16/16 roundtrip_save=1000/1000 order_deterministic=1
```

**Repro commands (exact):**

```sh
# 1. Build only this batch (private dir)
cmake -S . -B build/w-W2-12 -DRT_NATIVE_ASSETS_BUILD_TESTS=ON -DCMAKE_BUILD_TYPE=Release
cmake --build build/w-W2-12 --target w2_input_map_test -j

# 2. With ROM (full verification + ELF decode of the 10 addiu immediates)
RT_ELF=/Users/holzr/Library/Application\ Support/RoadTripRecomp/disc/SLUS_203.98 \
  build/w-W2-12/w2_input_map_test --elf "$RT_ELF"
# prints exactly one SCORE line; $? == 0

# 3. Without ROM (pure paths + NO_ELF)
build/w-W2-12/w2_input_map_test
# prints "NO_ELF" then the SCORE line (still 0 exit)

# 4. Repro the 10 constants from ELF (snippet in test + report)
python3 -c '
import struct, sys
path = sys.argv[1]
off = 0x229f20 - 0x1ff000
with open(path, "rb") as f:
    f.seek(off)
    for i in range(10):
        instr = struct.unpack("<I", f.read(4))[0]
        imm = instr & 0xffff
        print(f"{i}: 0x{imm:04x}")
' "$RT_ELF"
# 0: 0x0040 ... exactly the default table
```

Every number (10/10, 16/16, 1000/1000, 1) is produced by code inside the test binary (no external oracle except optional ELF for the default_remap pin).

## Evidence grounding (no fabrication)

- Remap values: direct decode of addiu $vN, imm ; sh $vN, 0x12E8+... at 0x229F20 (generated decomp + manual capstone walk in prior passes). Matches authored ControllerConfig::resetDefaults().
- 16-bit layout: PhysicalButton in menu.h (cross-checked against scePadRead usage in ReadPadState decomp 0020a8a8.c and TestPad active-low contract).
- Menu bits (0x1000/0x4000/0x0040): verbatim from HandleNumberSpinner 0023a320.c and menu flow decomp.
- Action order: car-physics.md + game_state.toml + functions.tsv.
- Determinism + roundtrip: authored per rules; exercised 10k/1000 in test.

## Confidence

**High (85).** All numeric thresholds met on first clean build. ELF decode path exercised when RT_ELF present. Menu nav grounded on the two strongest cited sites; remaining nav paths marked GAP as required. No blockers hit. The module is ready for the documented coordinator integration step (route SDL events + test-socket through the collector).

## Open questions (per plan)

- Analog axis to steering/jet/wing continuous values (AcquireVehicleInput side) — outside this batch scope.
- Per-device family remaps (Xbox vs PS layout) — future wave.
- Exact start-button semantics in attract + name entry — partial GAP, sufficient for current menu nav contract.
- Second player port stride (0x3448) usage in action_map — noted but not exercised here.

## Build / test log (sanitized)

```
... cmake ... w-W2-12 ...
[100%] Built target w2_input_map_test
W2-12 SCORE default_remap=10/10 conflicts=0 bits_mapped=16/16 roundtrip_save=1000/1000 order_deterministic=1
```

(Full output captured in session; one SCORE line, exit 0.)

**End of W2-12 deliverable.**
