# Oracle golden frames

Ground-truth pictures from the PS2Recomp desktop oracle (the real recompiled Road Trip game), captured 2026-10-17 on this Mac. Native rebuild waves should calibrate against these pixels, not against guesses.

Images are **not in git**. They live under `build/scratch/oracle-golden/` (`/build/` is gitignored). Do not `git add -f` them. Recapture with `scripts/oracle_golden_capture.sh`. Per-frame harness words are also in `build/scratch/oracle-golden/manifest.json` (untracked).

## Route

Desktop oracle. Device fallback was not used. The release app id was not touched.

| | |
|---|---|
| Binary | `build/macos-release/RoadTrip.app/Contents/MacOS/RoadTrip` (prebuilt, mtime 2026-10-14 19:37, not rebuilt this session) |
| SDK / game `build_id` | `39c18412f00187ff25cef4e3fb81319bfb362b0e` (matched; no on-launch rebuild) |
| Disc | `~/Library/Application Support/RoadTripRecomp/disc` (`SLUS_203.98`, 1,280,576 bytes) |
| Game library | `~/Library/Application Support/RoadTripRecomp/game/libroadtrip_game.dylib` |
| Renderer | `RT_GS_BACKEND=pgs` (paraLLEl-GS), `RT_GS_SSAA=4`, `RT_PROGRESSIVE_FIELDS=0` |
| Time | `RT_TIME=virtual`, `RT_SPEED=max`, `RT_FAKE_CLOCK=1000000000` |
| Picture source | test-socket `frame` command (GS scanout, tightly packed RGBA, before presenter post-process) |
| Socket | harness-relative Unix socket `rt.sock` in each scene's work dir. Not `tcp:9876`. |
| Wall clock | menus 2.4 s, attract 3.3 s, race 7.3 s, town 10.6 s |

`RT_REFERENCE_RENDERERS=ON` was already set in that macos-release cache, so paraLLEl-GS was available. Current `src/` was not compiled into this binary.

## What "visible" means

Every frame below was opened and checked, not only measured. Average RGB and non-black fraction are from the full image (a pixel is non-black when R+G+B > 12). None are a black or empty present. Title, main menu, and Options match the game's own art; Options matches the regression golden `build/regression/goldens/options_menu.png`, including the keyboard prompt text.

| Frame | Avg RGB | Non-black | Channel std | Visually |
|---|---|---|---|---|
| `title.png` | 120.04, 132.95, 119.68 | 1.000 | 75.80 | Yes. License-plate title, "PRESS START BUTTON", three cars. |
| `main_menu.png` | 115.96, 130.42, 121.66 | 1.000 | 75.22 | Yes. Adventure / Quick Race / 2 Player / Options / Results. |
| `options.png` | 108.46, 125.07, 124.61 | 1.000 | 74.24 | Yes. Vibration / Speaker / Sound Volume / Exit. |
| `attract.png` | 102.01, 101.56, 100.15 | 1.000 | 36.96 | Yes, but a cinematic roof-level shot. Cars and asphalt, no HUD. |
| `race_hud.png` | 106.70, 131.01, 139.27 | 0.9997 | 70.11 | Yes. Peach Raceway, full HUD, 24th, 39 mph, 00'40"76. |
| `race_hud_driving.png` | 100.55, 118.22, 124.78 | 0.9994 | 63.94 | Yes. Same race later, 63 mph, 00'52"95, chase cam. |
| `factory_menu.png` | 126.57, 145.14, 118.22 | 0.9999 | 78.41 | Yes. Q's Factory, "Drive around town" on the list. |
| `town_driving.png` | 102.37, 111.83, 122.31 | 0.9997 | 55.38 | Yes. Peach Town square, minimap, 0 mph, clock 09:13. |
| `town_driving_street.png` | 120.67, 110.03, 94.28 | 0.9918 | 50.25 | Yes. Same car against the Paint Shop, clock 09:15. |

## Frames

Addresses are `config/game_state.toml`. `progress.scene` is the u32 at `0x0177F760+0x538`. `game_mode` is the u8 at `0x33590A` (0 Adventure, 1 Quick Race, 8 attract). `scene3d_handler` is `0x3356D8` (`0x21CE48` race, `0x21CF80` town). `drive_flags` is `0x335928`. `location` is the u8 at `0x335923` (1 = Peach Town). Clock is `0x335914` vblanks since midnight, 150 per game minute.

| File (untracked) | Screen | Resolution | How | Harness state at the grab |
|---|---|---|---|---|
| `build/scratch/oracle-golden/title.png` | Title / press-start | 1280×447 | Boot, run 12 guest seconds, socket `frame`. Same timing as `test_boot_to_title`. | vblank 733, flips 723, `ui_context=0x29AFA0` (title), `game_mode=0`, `progress.scene=0`, car (0, 0, 0), clock 0. Scene word is stale here. |
| `build/scratch/oracle-golden/main_menu.png` | Main menu, cursor on Adventure | 1280×447 | Title, Start, 3 guest seconds. `boot_to_main_menu`, except Start is pressed at 12 s rather than 10 s. | vblank 941, still `ui_context=0x29AFA0`, `game_mode=0`, car (0, 0, 0). |
| `build/scratch/oracle-golden/options.png` | Game Options (not the recomp replacement) | 1280×447 | Main menu, Down ×3, Cross, 2 s. `RT_GAME_OPTIONS=1`. | vblank 1137, still title UI context, `game_mode=0`. |
| `build/scratch/oracle-golden/attract.png` | Attract demo, 45 guest seconds, no input | 1280×447 | `test_attract_demo` timing. | vblank 2711, `game_mode=8`, `scene3d_active=2`, handler `0x21CE48` (race), `drive_flags=0x10104` (includes `0x10000`, HUD hidden), car (892.488, 1.021, 1101.804), clock 144400 vblanks (minute 962). `progress.scene` still 0. |
| `build/scratch/oracle-golden/race_hud.png` | Quick Race HUD, Peach Raceway, already rolling | 1280×447 | Main menu → Quick Race → Cross ×8 with 4 s between (`test_quick_race`), then wait until `progress.scene==10` or 20 s. Scene never became 10, so this is ~41 s into the race (HUD `00'40"76`). | vblank 4193, `game_mode=1`, handler race, `scene3d_active=2`, `drive_flags=0x124`, car (616.684, 1.100, 568.756), place−1 = 23 (24th, matches HUD), countdown 0, total-time frames 2447 (2447/60 = 40.78 s, matches `00'40"76`). `progress.scene` stayed 0. |
| `build/scratch/oracle-golden/race_hud_driving.png` | Same race, gas held, chase view | 1280×447 | Previous pose, Cross held 12 guest seconds. | vblank 4924, car (747.502, 1.100, 479.340), place−1 23, total-time frames 3178 (52.97 s, HUD `00'52"95`), clock still 108000 (12:00). |
| `build/scratch/oracle-golden/factory_menu.png` | Q's Factory after Continue | 1280×447 | Checkpoint `build/regression/checkpoints/adventure_first_save`, `continue_to_factory`. | vblank 2224, `progress.scene=6` (factory), `game_mode=0`, `location=1`, clock 81877 (minute 545). Car position word still 0 (not driving). |
| `build/scratch/oracle-golden/town_driving.png` | Peach Town, just outside the factory | 1280×447 | Factory menu, Down ×4, Cross, Cross ("Come again!"), 12 s, `finish_dialogue`. No town path-follower. | vblank 3323, `progress.scene=2`, handler `0x21CF80` (town), `drive_flags=4` (can drive), `location=1`, car (506.283, 30.000, 442.230), clock 82974 (minute 553 = 09:13, matches the HUD clock). |
| `build/scratch/oracle-golden/town_driving_street.png` | Peach Town street, Paint Shop | 1280×447 | Previous pose, Cross held 5 s. Car x moved +12.274. | vblank 3695, car (518.557, 30.098, 481.538), clock 83346 (minute 555 = 09:15, matches HUD). |

Required set for calibration: `title.png` (title/attract; `attract.png` is the demo), `main_menu.png` + `options.png`, `town_driving.png` (in-game driving), `race_hud.png` (race HUD). The other files are the same runs a few seconds later, so a wave can see motion as well as a static pose.

## Recapture

```sh
scripts/oracle_golden_capture.sh
```

One scene group:

```sh
RT_ONLY=title,main_menu,options scripts/oracle_golden_capture.sh
RT_ONLY=attract scripts/oracle_golden_capture.sh
RT_ONLY=race_hud,race_hud_driving scripts/oracle_golden_capture.sh
RT_ONLY=factory_menu,town_driving,town_driving_street scripts/oracle_golden_capture.sh
```

Every variable the script reads is commented at the top of `scripts/oracle_golden_capture.sh`. The ones that change the picture:

| Variable | This capture | Effect |
|---|---|---|
| `RT_ORACLE_APP` | `build/macos-release/RoadTrip.app/Contents/MacOS/RoadTrip` | Oracle executable. Not the native `rt-app`. |
| `RT_BASE_DATA` | `~/Library/Application Support/RoadTripRecomp` | Must contain `disc/` and `game/`. Symlinked into a private `RT_DATA_DIR`. |
| `RT_GOLDEN_OUT` | `build/scratch/oracle-golden` | PNG + `manifest.json` + `work/<scene>/app.log`. |
| `RT_CHECKPOINT` | `build/regression/checkpoints/adventure_first_save` | mc0 used for factory/town. Empty skips town. |
| `RT_GS_BACKEND` | `pgs` | `hw` or `cpu` will not match these pixels. |
| `RT_GS_SSAA` | `4` | With progressive fields off, scanout is 1280×447. |
| `RT_PROGRESSIVE_FIELDS` | `0` | `1` changes the field combine. |
| `RT_GAME_OPTIONS` | `1` | `0` replaces Options with the recomp menu. |
| `RT_FAKE_CLOCK` | `1000000000` | Guest date. Do not change if frames must match. |
| `RT_SPEED` | `max` | Guest pace. Virtual time, so the pad sequence still hits the same vblanks. |
| `RT_FRAME_WARMUP` | `12` | Rendered vblanks before each grab. A grab with non-black fraction < 0.05 is retried once. None of these needed the retry. |
| `RT_ONLY` | empty | Comma-separated scene ids. |

Pinned by the harness, not by the caller: `RT_HEADLESS=1`, `RT_TIME=virtual`, `RT_RENDER=0` except during the grab, `RT_TEST_SOCKET=rt.sock`. The script unsets inherited `RT_DATA_DIR`, `RT_TEST_SOCKET`, `RT_FRAME_DUMP*`, and `RT_MENU_SHOT*` so a leftover dump or menu-shot cannot quit the oracle or write into the user's real save dir.

`RT_PYTHON` defaults to `build/regress-venv/bin/python` (Pillow). Numpy is used for the color stats when importable.

## UNDOCUMENTED

Things this capture showed that `config/game_state.toml` does not say, or that a later wave will get wrong if it assumes the obvious.

1. **Quick Race does not set `progress.scene` to 10.** After the countdown the word stayed 0 while `game_mode` was 1, the 3D handler was the race handler, the HUD was live, and the car moved. The scene enum (2 town, 6 factory, 10 racing) is the Adventure progress block. The script's "wait until scene==10" therefore always burns the 20 s timeout on Quick Race, which is why `race_hud.png` is `00'40"76` rather than the start lights. Identify a Quick Race with `game_mode==1` and `scene3d_handler==0x21CE48`, not with `progress.scene`.

2. **Race live words are garbage in town.** `place`, `countdown`, and `total_time` (`0x015B0088`, `0x15AFB8C`, `0x15AFC1C`) read as `1144128939` / `1106771968` / `1065353216` while driving in Peach Town. The toml already says the work area is reused; these captures confirm the values are not a place or a time. Do not assert them unless `game_mode==1` (or the race handler is current).

3. **`drive_flags` bit `0x100` is set in a race and in the attract demo and is not named.** Observed: race `0x124` = `0x100|0x20|0x04`, attract `0x10104` = `0x10000|0x100|0x04`. Named bits in the toml are `0x04` can drive, `0x20` racing, `0x08` finished, `0x10000` hides the HUD. The attract picture has no HUD, which matches `0x10000`. Meaning of `0x100` was not determined.

4. **Town clock advances; the Quick Race clock word did not.** Town `0x335914` went 82974 → 83346 across 5 guest seconds of throttle, and the HUD clock went 09:13 → 09:15 (150 vblanks/minute). Both race grabs sat at 108000 (exactly 12:00) across 12 guest seconds. Not investigated further.

5. **"button Enter" / "button Exit" are the oracle's headless keyboard prompt, not missing game art.** They appear on `main_menu.png` and `options.png` and on the regression golden `options_menu.png`. Title itself draws the game's "PRESS START BUTTON" texture, with no recomp overlay. Do not treat those two words as a texture-pack failure.

6. **Resolution is the GS scanout, 1280×447, not 1280×720.** That is 4× SSAA of the interlaced field, the same size as `build/regression/goldens/*.png`. No presenter/window capture was taken. `RT_FRAME_DUMP` needs a window and was left unset on purpose (`RT_HEADLESS=1`).

7. **`RT_DISC` is not how this oracle finds the disc.** The recomp app serves `cdrom0:` from `<RT_DATA_DIR>/disc`. The harness symlinks `RT_BASE_DATA/disc`. Setting `RT_DISC` alone does nothing for `RoadTrip.app`.

8. **No field-area frame** (the open world outside a town). Driving view is Peach Town only (`location=1`). Attract and race are circuit cameras, not the field map.

9. **Attract is a poor driving-camera reference.** `game_mode=8` and the race handler are set, but the camera is a roof-level cinematic and the HUD-hide bit is set. Use `race_hud.png` / `town_driving.png` for chase-cam and HUD calibration.

10. **Title menu state is stale.** While `ui_context==0x29AFA0`, car position, clock, and `progress.scene` read as zero even though the picture is the title or the main menu. Do not wait on those words to detect the title.

11. **This oracle is the 2026-10-14 prebuilt app**, not a build of the current tree. A wave that needs pixels from a newer runtime has to rebuild `build/macos-release` (or another build dir) with `-DRT_REFERENCE_RENDERERS=ON` and rerun the script. `src/native/` was not used and was not modified.

12. **No savestate shortcut was required.** Town came from the memory-card checkpoint above, not from an `.rtstate`. The disc tree has no savestates directory; `.rtstate` files elsewhere under `build/` were left alone.
