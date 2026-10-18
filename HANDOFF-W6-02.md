# W6-02 handoff: title save/load UI state

## Files

- `src/native/preview/title_ui.h`
- `src/native/preview/title_ui.cpp`
- `tests/native_w16/title_ui_test.cpp`
- `docs/native-reference/title-save-load.md`

## API and integration

`rt::native::preview::FakeCardBackend` supplies deterministic in-memory
`SaveSlotInfo` values. Construct `TitleUiState state(card)` and call `up()`,
`down()`, `confirm()`, `back()`, then `tick(frames)` once per app frame.
`confirm()` returns `StartNewGame`, `LoadContinue`, or `SaveCorrupt`; successful
launch actions transition `Idle -> FadingOut -> Complete` over 32 ticks.

Call `state.emit(viewportWidth, viewportHeight)` each frame and composite
`TitleUiDrawData::quads` with the existing 2D path and `::text` with the HUD
FONTE path. The coordinator owns all wiring in `app/main.cpp`; this worker did
not edit it.

| Element | Design-space rect / origin | Draw data |
| --- | --- | --- |
| panel | `(144,136,352,152)` | `ScreenQuad` |
| New Game highlight | `(168,160,304,40)` when selected | `ScreenQuad` |
| Continue highlight | `(168,216,304,40)` when selected | `ScreenQuad` |
| New Game label | `(192,168)` | FONTE `HudQuad`s |
| Continue label | `(192,224)` | FONTE `HudQuad`s; grey while absent |
| fade | full viewport, alpha `fadeAlpha()` | `ScreenQuad` |

Coordinates use the existing centred 640x480 FONTE mapping: the stated 640x448
design Y receives its normal +16 display-frame offset.

Register later (not performed here):

```cmake
add_subdirectory(tests/native_w16)
```

The coordinator also needs to add `preview/title_ui.cpp` to the native target
that consumes it.

## Verification run

```sh
mkdir -p build/scratch/w6-02
c++ -std=c++20 -I src/native -o build/scratch/w6-02/w16_title_ui_test \
  tests/native_w16/title_ui_test.cpp src/native/preview/title_ui.cpp
build/scratch/w6-02/w16_title_ui_test
```

Output:

```text
checks=13 failed=0 negative_control=CAUGHT
```

See `docs/native-reference/title-save-load.md` for ROM evidence and explicit
GAPs.
