# Open recomp issues — native status (2026-10-28)

The goal requires the open issues of https://github.com/silentsudin/RoadTripAdventure-recomp
to be fixed or evidenced. Many are **recomp-renderer defects** (the paraLLEl-GS / hardware-GS
front end, the second screen's GS-layer routing): the native app has no GS and no second-screen
GS layer, so those issues cannot exist there and the recomp tree keeps its own fixes. The rest are
**feature requests**; since native must have feature parity, each one is a native task.

Status legend: `absent` — the native app has no such feature; `partial`; `present`.

## Renderer defects (recomp-only, cannot occur natively)

| Issue | Subject | Native status |
|---|---|---|
| #2 | Second screen glitches inside the Body Shop | not a native symptom (no GS map layer to glitch; the native map is reconstructed in `gameplay/map_view`), and the native second screen is not on the Android display yet |
| #4 | Hardware GS: River Raceway 2P road decal covers more ground than paraLLEl-GS | not a native symptom (no GS batches; native draws terrain meshes directly) |
| #7 | GS thread 25-35 ms on frames while a new area loads | not a native symptom (no GS thread); native streams tiles in `gameplay/tile_streamer` — its own load cost still needs a device measurement |
| #8 | ~5 ms GPU busy unaccounted in town | not a native symptom (native has no GS pipeline; its GPU cost is the W15 ledger) |
| #19 | High GS cost on Windows | recomp renderer; native's cost is the W15 ledger |
| #20 | Crashes at the Conspiracy Entertainment screen on Intel HD 630 | native now **installs** on GL-only devices: the Android manifest no longer requires Vulkan (`android.hardware.vulkan.version` 1.1, `required="false"`), and `RT_APP_RENDERER=auto` falls back to GL; whether the native GL path runs on that GPU is unverified (no such machine here) |
| #11 | Test harness can't drive the game through the SDL pad once the test pad was used | tooling, recomp harness; pull request 39/40 await review |
| #12 | `RT_MENU_SHOT` doesn't work headless or on Android | tooling, recomp; native's twin is `RT_APP_MENU_SCREEN` + `RT_APP_SNAPSHOT`, which works headless |

## Feature requests (native parity tasks)

| Issue | Subject | Native status | What it needs |
|---|---|---|---|
| #9 | Course-select carousel dips below 60 fps on the Thor | recomp VU1 entry4 cost; native has no VU1 microcode run (translated code) — measure on device when the Thor is available | device measurement |
| #21 | CRT shader option | `absent` | a post-process pass in `preview/` (scanline/mask + the recomp's option row) |
| #22 | Modding framework | `absent` | design decision (what a mod may replace: assets, tables, scripts) |
| #24 | Optional steering-bias fix (cars steer sharper left) | **implemented** | `UpdateSteeringDamping` 0x21a088: the descending arm's closeness test is the ascending one mirrored the wrong way (`iVar3 - cmd < 9` is already true while descending), so a left command snapped to lock in one tick. Options > Graphics > `Fix steering bias` (off by default = retail) flips it to `cmd - iVar3 < 9`; `rtguest::gSteerBiasFix`, pins in `tests/native_guest/test_steering_bias.cpp`. The same TU's `param_3`/`param_5` were translated unsigned, which made the whole negative arm dead — now signed (a fidelity fix, retail hash-neutral in the race@110 pair, town 2680 census 4,982 -> 4,848 B) |
| #25 | Import a PCSX2 save file | **logic present, no UI** | `gameplay/save.{h,cpp}` already parses the 13,384-byte progress block and maps a public-P2 image (`scanP2RoadTripSaves`, `migrateFromP2Image`, `migrateFromSaveFile`, with `migrateToNative` normalising). What is missing is only the Options row plus a file picker — and that needs action dispatch first: `optionsPanelConfirm` records `lastAction()` and `app/main.cpp` never consumes it, so no Save-page action (Save/Load state either) fires yet |
| #26 | Force high-quality LOD for cars and wheels | **not applicable to the native renderer** | The native car is one merged mesh: `preview/car_mesh` keeps model 0 of the CAR file only (`car_mesh.h:68-72`), so there is no level of detail to force. The game's own selection is in `drawCar` 0x221788: body far when ratio >= `carBodyFarRatio` (0x332CC0, ELF float 0.03) or after five near bodies (`carNearBodyCount` 0x3356E8), wheels near at ratio 0 / mid below `carWheelsFarRatio` (0x332CCC, 0.05) / else far. No native render path calls `drawCars*` yet (their only callers are the guest scene draw TUs 0x220938/0x220b20/0x220d40/0x220fe8), so a row would do nothing; the recomp app's hook (`src/game/overrides.cpp:430-438`, distance 0 + counter 0) is the wiring to port when the guest car draw path lands |
| #27 | Right thumbstick / analog camera control | **implemented** | Options > Controllers > `Analog camera` (`controllers.analog_camera`, off by default) feeds the right stick (SDL `RIGHTX/RIGHTY`, or the test pad's `rx`/`ry`) to `GameSession::setCameraLook`: the look target orbits its eye by up to a quarter turn of yaw and 30 degrees of pitch behind a 0.2 deadzone, after `updateCamera`, so the car keeps its own axes. Zero deflection runs no rotation at all — headless lockstep runs at 110 frames are byte-identical with the row off or on with a resting stick, and full right stick changes 580,657 of 1,440,015 frame bytes |
| #28 | Increased draw / render distance | **implemented (two steps)** | Options > Graphics > `Draw distance` (`graphics.draw_distance`) maps to the terrain neighbourhood radius (`RT_APP_TILES_RADIUS`), applied at the next start; an explicit env value still wins. The scene's far plane follows the radius (`GameSession`'s `farPlane`, 500 render units x radius) so the wider neighbourhood's 679-unit far corner is not clipped; the default stays exactly 500. Headless, terrain scene: Default loads `tiles=9` (radius=1), Wide loads `tiles=23` (radius=2, `farZ=1000`) and changes 738,544 of 1,440,015 frame bytes. Radius 3 would need a far plane past 1,000 units; the row deliberately has no third step |
| #30 | Custom race / opponents grid | `absent` | race setup (`gameplay/race_rules`, `session`) plus a grid editor UI |

## Notes

- The Options catalog lists `Fix steering bias` and `Draw distance` as wired rows
  (`gameplay/settings.cpp` retail catalog). "High detail cars" (#26) has no counterpart
  because the native car mesh has one LOD; the parity rule is to hide what does not work.
- The second screen is implemented natively (Map/Notebook/Radio, `preview/second_screen`); what is
  still missing is the **Android presentation display** and its JNI seam, which is the native side
  of the feature those issues were filed against.
