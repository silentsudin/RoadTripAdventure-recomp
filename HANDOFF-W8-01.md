# W8-01 handoff: title save/load UI app wiring

## Files touched

- `src/native/app/main.cpp`
- `HANDOFF-W8-01.md`

## Behaviour

- `RT_APP_STATE=LOAD` starts directly on the New Game / Continue screen.
- Selecting **Adventure** from the main menu opens that screen. Up/down/Enter
  feed `TitleUiState`; Escape returns to the main menu while idle.
- A valid Continue selection loads slot 0 after its 32-frame fade. New Game
  resets session progress before starting the same race path. Missing or bad
  saves leave Continue disabled, as supplied by `TitleUiState`.
- Unset `RT_APP_STATE` still starts at TITLE; `MENU` and `PLAYING` parsing is
  unchanged.

## Main.cpp edit ranges

- Includes and app-state parsing/render helpers: lines 39, 1309-1409.
- Slot-0 backend setup: lines 1818-1832.
- State initialisation and keyboard routing: lines 1944-1947, 2125-2132.
- Menu-to-load transition, back, fade completion, and launch: lines 2174-2212.
- Save/load quads and FONTE composition: lines 3018, 3082-3115.

## Required coordinator CMake registration

`title_ui.cpp` is registered for the w16 test but is **not** a source of
`rt_native_assets` in the current tree. Add this exact line beside
`preview/ui_menu.cpp` in `RT_NATIVE_ASSET_SOURCES` in `src/native/CMakeLists.txt`:

```cmake
    preview/title_ui.cpp         # title New Game / Continue UI state
```

Do not add it separately to `rt-app`: that target already links
`rt_native_assets`, and this single registration also covers Android.

## Verification evidence

Configured in the isolated build directory:

```text
cmake -S src/native -B build/w-W8-1 -DRT_NATIVE_ASSETS_BUILD_TESTS=ON -DCMAKE_BUILD_TYPE=Release
```

`w16_title_ui_test` (rc 0):

```text
checks=13 failed=0 negative_control=CAUGHT
```

For the runtime build only, I inserted a locally compiled `title_ui.cpp`
object into `build/w-W8-1/librt_native_assets.a`; this isolated build artifact
does not change source or CMake and compensates for the missing registration
above. `cmake --build build/w-W8-1 --target rt-app w16_title_ui_test -j4`
then returned rc 0.

Headless LOAD smoke (rc 0):

```text
RT_TIME=virtual RT_APP_NOAUDIO=1 RT_APP_HEADLESS=1 RT_APP_STATE=LOAD RT_APP_FRAMES=30 RT_APP_SNAPSHOT=build/w-W8-1/w8-01-load.ppm RT_APP_SNAPSHOT_AT=0 ./build/w-W8-1/rt-app
rt-app headless: dummy video, null presenter 800x600
rt-app state=LOAD
rt-app snapshot: build/w-W8-1/w8-01-load.ppm (800x600)
rt-app frames=30 simulation_frame=0
```

The PPM is nonempty (1,440,015 bytes). MENU regression used the same command
with `RT_APP_STATE=MENU` and `w8-01-menu.ppm` (rc 0):

```text
rt-app state=MENU
rt-app snapshot: build/w-W8-1/w8-01-menu.ppm (800x600)
rt-app frames=30 simulation_frame=0
```

## Coordinator re-check

After adding the one CMake source line, delete/reconfigure the isolated build
or rebuild `rt_native_assets` normally; no manual archive object should be
needed. Exercise the interactive Adventure -> New Game / Continue path with a
real SDL event source if end-to-end controller routing needs coverage.
