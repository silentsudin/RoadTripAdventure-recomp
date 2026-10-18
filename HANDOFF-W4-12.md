# W4-12 handoff — SCORE/METER0 missing HUD pages

## Delivered API surface

`src/native/assets/hud_art.h`, `rt::native::assets::HudArt` now exposes:

```cpp
std::optional<preview::PreviewTexture> lapDigits;
std::optional<preview::PreviewTexture> lapDisc;
std::optional<preview::PreviewTexture> gaugePage;
```

* `lapDigits`: `SYS/SCORE.GSL` entry-1 image payload `0x8350`, 256x256
  PSMT4 decoded with its following CT16 CLUT at CSA 7 (base entry 104).
  The lap cells are `u=lap*8, v=200`, 8x16.
* `lapDisc`: same SCORE page and CLUT, CSA 8 (base entry 112).  The disc is
  `u=232, v=168`, 24x24.  It remains a normal `PreviewTexture`; the consumer
  applies its existing translucent UI blend state.
* `gaugePage`: `ITEM/METER0.GSL` gauge-subpage payload `0x8550`, 128x120
  PSMT4 decoded from its following CT16 CLUT at CSA 12 (base entry 192).
  Needle `u=3,v=53, 8x59`, hub `u=16,v=80, 12x12`, and boost arc
  `u=40,v=48, 20x60` are all present.

All three members are optional, like the pre-existing HUD art.  No consumer
was changed.  The decoder finds the image by payload offset (not a fragile
unit ordinal), validates PSM/dimensions/adjacent CT16 CLUT, and follows the
existing PSMT4 CSM1/palette conversion path.

## Coordinator integration note

When replacing W4-06 stand-ins in the coordinator-owned HUD code:

1. Replace `raceHudLapDiscFallback` with the `lapDisc` texture and source
   rectangle `(232,168,24,24)`; retain the current translucent blend mode.
2. Replace `raceHudLapDigitFallbackQuads` with `lapDigits` source cells
   `(lap*8,200,8,16)` (CSA 7 is already baked into that texture).
3. Bind `gaugePage` for `RaceHudPage::Meter0Sub`; retain the ROM geometry in
   `race_hud_layout.h`: needle `(3,53,8,59)`, hub `(16,80,12,12)`, boost arcs
   `(40,48,20,60)`.  The page itself does not supply a needle-angle law.

## Tests

Added `tests/native_w12/hud_pages_test.cpp`, registered only through the one
allowed `add_subdirectory` line.  It exits `0` with `SKIP (disc absent)`, and
with a disc verifies the two payload offsets, PSMT4/dimensions, glyph/disc/
needle/hub/arc ink, CSA-7 gold-green palette sanity, and byte-stable reloads.

Verbatim successful run transcripts:

```text
$ ./build/native-w4-12/native_w12/w12_hud_pages_test
w12_hud_pages_test passed

$ ./build/native-w4-12/native_w12/w12_hud_pages_test
w12_hud_pages_test passed

$ RT_DISC=/definitely/not/a/disc ./build/native-w4-12/native_w12/w12_hud_pages_test
SKIP (disc absent)

$ ./build/native-w4-12/native_hud_art_test
speedDial 256x128 fnv=b0f8cd2fc1570dc2
speedDigits 256x128 fnv=12e6289d587e7846
raceAtlas 256x256 fnv=92922f0c37eb35d0
native_hud_art_test passed

$ ./build/native-w4-12/w2_hud_live_test
W2-10 SCORE rect_err_px=0 timer_vectors=14/14 timer_oracle=2/2 speed_oracle=2/2 elf_pins=69/69 needle_points=4 needle_independent=3 needle_resid_deg_max=DEFERRED needle_rest_resid_deg=0.84 roi_mad=8.715 roi_hist=0.8720 deterministic=1 PASS
```

The TDD red build (before adding the new `HudArt` members) failed as intended:

```text
/Users/holzr/github/RoadTripAdventure-decomp/tests/native_w12/hud_pages_test.cpp:46:17: error: no member named 'lapDigits' in 'rt::native::assets::HudArt'
/Users/holzr/github/RoadTripAdventure-decomp/tests/native_w12/hud_pages_test.cpp:46:36: error: no member named 'lapDisc' in 'rt::native::assets::HudArt'
/Users/holzr/github/RoadTripAdventure-decomp/tests/native_w12/hud_pages_test.cpp:46:53: error: no member named 'gaugePage' in 'rt::native::assets::HudArt'
```

Configuration/build used:

```text
RT_SKIP_ANDROID_CHECK=1 RT_SKIP_VRC_PLUGIN=1 cmake -S src/native -B build/native-w4-12 -DRT_NATIVE_ASSETS_BUILD_TESTS=ON
cmake --build build/native-w4-12 --target w12_hud_pages_test native_hud_art_test -j4
```
