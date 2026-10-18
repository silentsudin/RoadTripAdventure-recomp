# HANDOFF W4-10: speed-display law harmonization

Branch `re/native-complete`. Worker W4-10. Ruling applied: **SPEED LAW** (coordinator, 2026-10-18).

    V = floor(speed_word / 55.07), clamped to 0..186
    zero-suppressed digits (V = 0 draws one "0"), right-aligned
    ones digit top-left at UI (573,388); step -17 per higher digit
    16x23 cells on METER0; u = 140 + 16*(d%7), v = 23*(d/7); drawn uniform white
    "mph" is baked into METER0 (never drawn separately); badge box x 556..604, y 388..416
    speedometer needle: UNRESOLVED, no needle constants changed

## Changed (my files only)

- `src/native/gameplay/hud_live.h`
  - `kDefaultSpeedDivisor` 100 -> `kSpeedDivisorMph = 55.07f` (`kDefaultSpeedDivisor` kept as alias, so existing callers compile).
  - New `kSpeedMax = 186`.
  - New METER0 digit constants: `kSpeedOnesX=573`, `kSpeedOnesY=388`, `kSpeedDigitStep=-17`, `kSpeedDigitW=16`, `kSpeedDigitH=23`, `kSpeedAtlasU0=140`, `kSpeedAtlasVStep=23`, `kSpeedAtlasCols=7`.
  - New badge constants: `kSpeedBadgeX=556`, `kSpeedBadgeY=388`, `kSpeedBadgeW=48`, `kSpeedBadgeH=28`.
  - `kAltSpeedDivisor` (20000/186) retained but marked retired. It is only referenced by the informational lines of `hud_live_test.cpp`.
  - Comments updated: the GAP notes now describe the law.
- `src/native/gameplay/hud_layout.h`
  - `HudElement::DriveSpeedDigits` rect 532,385,58x26 -> **539,388,50x23** (bbox of up to 3 digits; right edge 589 = ones right edge).
  - `kHudGlyphSets`: the existing 20x26 METER10 set at index 10 is **kept unchanged** (it is the 2P handler-10 set; `test_hud_layout.cpp` pins it). Two METER0 rows added at the end (indices 16, 17): row 0 = digits 0-6 (`140,0,16x23,uPitch 16,count 7`), row 1 = digits 7-9 (`140,23,16x23,uPitch 16,count 3`). Two rows, not one, because the grid model in the existing tests wraps at atlas width / uPitch.
- `tests/native_w2/hud_layout_test.cpp` (**new file**; the task named it as existing, but it did not exist. CMake's `tests/native_w2/*_test.cpp` glob registers it as `w2_hud_layout_test` when the standalone `src/native` project is configured with `RT_NATIVE_ASSETS_BUILD_TESTS=ON`).
  - Checks: law constants, badge/digit-extent geometry, layout bbox and METER0 glyph cells, all fixtures through `hostMph` and through `buildGaugeHud` quads (quad count, x/u/v per digit), `speedCells` digit-count semantics.
  - Fixtures: 0->0, 55->0, 55.07->1, 5506.999->99, 5507->100, 10243->185, 10244->186, 999999->186.

`hud_live.cpp` already draws the 1P digits per the law (x=0x59, y=0x5e, step 0x11, 16x23, METER0), so it needed no change for the digit path.

## Test results (no ROM needed for w2_hud_layout_test)

Configure and build (standalone `src/native` project; see note 1):

    RT_SKIP_ANDROID_CHECK=1 RT_SKIP_VRC_PLUGIN=1 cmake -S src/native -B build/native-w410-assets -DRT_NATIVE_ASSETS_BUILD_TESTS=ON
    cmake --build build/native-w410-assets -j8 --target w2_hud_layout_test w2_hud_live_test native_assets_hud_layout_test

Current tree, run from repo root:

| binary | result |
|---|---|
| `w2_hud_layout_test` | **FAIL** 47 checks, 2 failed: `999999 -> got 18158, want 186` (hostMph clamp; **needs item A**) |
| `native_assets_hud_layout_test` | **FAIL** 3/463 (checks #4, #5 pin old DriveSpeedDigits rect; #460 pins old meter text origin; **needs item C**) |
| `w2_hud_live_test --elf .../SLUS_203.98` | **FAIL** `speed_oracle=0/2` (the `d100` check; **needs items A + B**). Everything else in the line unchanged |

Verified with scratch builds (repo files NOT edited; patched copies in the scratch dir `/private/var/folders/66/v2t7gtq135l3g5jlkbrp72_80000gn/T/opencode/w410.xNHK/`):

- `w2_hud_layout_test` + patched `hud_live.cpp` (item A): **PASS, 47 checks, 0 failed**.
- `w2_hud_live_test` + item A + item B, real ELF: **`W2-10 SCORE ... speed_oracle=2/2 ... elf_pins=69/69 ... PASS`**, exit 0.
- `native_assets_hud_layout_test` + item C: **463 checks passed**, exit 0.

## Required coordinator changes (files outside my edit list)

### A. `src/native/gameplay/hud_live.cpp` lines 41-44: clamp hostMph to the law

Replace:

```cpp
int hostMph(float hostSpeed, float divisor) {
    if (!(divisor > 0.f)) return 0;
    return static_cast<int>(std::fabs(hostSpeed) / divisor);
}
```

with:

```cpp
int hostMph(float hostSpeed, float divisor) {
    if (!(divisor > 0.f)) return 0;
    const float q = std::fabs(hostSpeed) / divisor;
    if (!(q > 0.f)) return 0;
    if (q >= static_cast<float>(kSpeedMax)) return kSpeedMax;
    return static_cast<int>(q);
}
```

Without it, `999999` renders `160`, because `speedCells` only draws 3 cells of the unclamped 18158.

### B. `tests/native_w2/hud_live_test.cpp` line 327: divisor expectation

Replace:

```cpp
        const bool d100 = hl::hostMph(static_cast<float>(want) * 100.f + 50.f, hl::kDefaultSpeedDivisor) == want;
```

with:

```cpp
        const bool d100 = hl::hostMph(static_cast<float>(want) * hl::kSpeedDivisorMph + 0.5f) == want;
```

Informational lines 368-376 still print "/100"; that text is cosmetic and can be updated later.

### C. `tests/native_assets/test_hud_layout.cpp`

Lines 37-39, replace:

```cpp
    check(speed != nullptr && rectEq(speed->ui, 532.f, 385.f, 58.f, 26.f));
    // Rightmost 20px digit starts at anchor+78: right edge 492+78+20 = 590.
    check(speed != nullptr && speed->ui.x + speed->ui.w == 590.f);
```

with:

```cpp
    check(speed != nullptr && rectEq(speed->ui, 539.f, 388.f, 50.f, 23.f));
    // SPEED LAW: ones digit (573,388) 16px wide -> right edge 589.
    check(speed != nullptr && speed->ui.x + speed->ui.w == 589.f);
```

Line 209, replace `check(meter[0].x == 532.f && meter[0].y == 401.f);` with `check(meter[0].x == 539.f && meter[0].y == 404.f);`. The FONTE text origin is the DriveSpeedDigits bbox origin, so it moves with the element.

### D. `src/native/preview/race_hud.h` lines 206-209 (comment only)

The comment still reads "CONFIRMED: DriveSpeedDigits is (532,385), emitted by 0x24E1A0 at the 1P dial anchor". That is wrong: 0x24E1A0 is the 2P handler. The 1P speed digits are at UI (573,388) on METER0 (SPEED LAW). Update the comment. Also note the preview's `meterPageTextQuads` draws FONTE text at this origin, not METER0 digits. That is a preview placeholder, not the law's rendering.

## Open / not changed (decision needed)

1. **Shipped chain conflicts with the law.** `shippedMphFromRaw` / `kShippedMphDivisor` (1.609, applied to `|v_f15| * 216.0`) and the `kShippedSpeedometerCap` comment describe the shipped mph chain. `216/1.609 ≈ 134.2` while the law uses `55.07` on `speed_word`. These cannot both be the same quantity. I left the shipped-chain helpers unchanged, and they are not the displayed path. Needs reconciliation before anyone reuses them.
2. **2P handler 10 is not covered by the law.** `buildDriveHud2P` (0x24E1A0, METER10) still draws 20x26 digits at v=102 with `speedCells` defaults (`firstDx 0x4E`, `step 0x13`). It does get the law divisor via `hostMph`. The law covers only METER0, so the 2P geometry is unverified. The METER10 glyph set (index 10) is intentionally kept.
3. **`kAltSpeedDivisor`** (20000/186) can be deleted once `hud_live_test.cpp` lines 328, 352-376 stop using it.
4. **Needle** is untouched, per the instructions.

## Build notes

1. The `w2_*` test executables come from the standalone project `src/native/CMakeLists.txt`. Configuring the top-level `cmake -S . -B build/native-w410` does not include them. For the top-level configure, `cmake --build build/native-w410 -j8` built everything I could reach, then failed at link time in `third_party/PS2Recomp/ps2xRuntime/ps2_vu1_recomp` (undefined `_g_ps2RecompiledFunctionTable*`). That target is unrelated to these files. I did not investigate it.
2. My files compiled with `-Wall -Wextra` and no warnings.
3. No git add, commit, stash, checkout, reset or push was run.
