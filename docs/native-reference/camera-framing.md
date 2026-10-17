# Chase-camera framing target sheet

This is the numeric target for the native driving/observation camera. It separates recovered game values from useful host conventions. Do **not** treat a host vertical FOV or look-at point as a recovered ROM constant: the PS2 camera build is a GS/VU screen-space construction, not a conventional clip matrix ([`camera-constants.md:39-42`](camera-constants.md#L39-L42)).

All image measurements use the unpresented oracle scanout, 1280 x 447, not a 1280 x 720 window capture ([`oracle-golden-frames.md:17-20,109`](oracle-golden-frames.md#L17-L20)). Coordinates start at the upper left. `Horizon y` is the estimated *road-plane vanishing/effective horizon* in the drive direction, not the building/sky silhouette: town geometry occludes that silhouette. Car bounds exclude blob shadows and HUD. Measurements are manually bounded to the nearest two pixels, so fractions carry roughly `+/-0.005` (bbox) or `+/-0.02` (occluded horizon) uncertainty.

## Oracle measurements

| Oracle frame | View classification | Effective horizon y / 447 | Horizon fraction | Car bbox `(x0,y0)-(x1,y1)` | Car height fraction | Car centre-x fraction | Ground below horizon fraction | Interpretation |
|---|---|---:|---:|---|---:|---:|---:|---|
| `town_driving.png` | Peach Town, rear chase | `~184` | `0.412` | `(485,250)-(797,377)` | `128/447 = 0.286` | `641/1280 = 0.501` | `263/447 = 0.588` | Primary town reference. The distant road through the centre/arch gives the estimate; rooflines must not be used. |
| `town_driving_street.png` | Peach Town, rear chase | `~184` (road direction occluded) | `0.412` | `(486,250)-(795,377)` | `128/447 = 0.286` | `641/1280 = 0.501` | `263/447 = 0.588` | Same framing scale as the first town grab. The Paint Shop facade occludes the vanishing point, so this carries forward the road-plane estimate rather than treating a roof edge as horizon. |
| `race_hud_driving.png` | Peach Raceway, rear chase | `~176` | `0.394` | `(486,249)-(796,377)` | `129/447 = 0.289` | `641/1280 = 0.501` | `271/447 = 0.606` | Primary race reference. The 8-pixel higher effective horizon is consistent with the local rising/curving track; car scale changes only `+0.003`. |
| `attract.png` | Cinematic overhead/roof-level | `0` (no visible sky/road horizon) | `0.000` | foreground car `(476,172)-(821,321)` | `150/447 = 0.336` | `649/1280 = 0.507` | `1.000` | **Non-target.** A cinematic roof-level camera, HUD-hidden, rather than a driving reference ([`oracle-golden-frames.md:50,113-115`](oracle-golden-frames.md#L50)). |

The two driving car heights agree to one pixel after rounding and both centres are effectively scanout centre. The evidence therefore does **not** support a town-vs-race change in follow distance, height, or lateral anchor. The `0.018` horizon-fraction difference is scene/road geometry evidence, not a separate camera-preset measurement. The oracle identifies these as a town capture (`drive_flags=4`) and a live Quick Race capture ([`oracle-golden-frames.md:52-55`](oracle-golden-frames.md#L52-L55)); it explicitly warns that attract is unsuitable for driving-camera calibration ([`oracle-golden-frames.md:113-115`](oracle-golden-frames.md#L113-L115)).

## Numeric target

`RU` means native render units. Native game positions are converted from world/game units by `0.1` before camera evaluation ([`src/native/gameplay/game_loop.cpp:66-70`](../../src/native/gameplay/game_loop.cpp#L66-L70)). For scale context, a town tile is 1600 terrain/game units ([`world-geography.md:13-20`](world-geography.md#L13-L20)), hence 160 RU in the native scene; neither tile width nor the scale conversion changes the camera tuple.

| Parameter | Target | Basis (cite / measured / UNDOCUMENTED-assumed) |
|---|---:|---|
| Backward distance | **7.000 RU** along `-carForward` before the orbit-pitch component | **ROM CONFIRMED:** preset-0 offset is `(0,2,-7)` at `0x0029ce00..0f` ([`camera-constants.md:11`](camera-constants.md#L11)). The native orbit maps distance to the backward component ([`camera.cpp:103-113`](../../src/native/gameplay/camera.cpp#L103-L113)). This is consistent with the matched `0.286/0.289` measured car-height fractions. |
| Base camera height above car origin | **2.000 RU** | **ROM CONFIRMED:** Y component of that preset ([`camera-constants.md:11`](camera-constants.md#L11)). |
| Actual flat-road eye height above car origin | **2.601 RU** | **Derived:** `2 + 7*sin(4.921875 deg)`. This is what the host orbit emits, rather than merely the preset base-Y value ([`camera.cpp:109-114`](../../src/native/gameplay/camera.cpp#L109-L114)). |
| Orbit pitch | **+4.921875 deg** (`0.08590293 rad`) | **ROM CONFIRMED:** default pitch target `0x0380` turns ([`camera-constants.md:13`](camera-constants.md#L13)). Positive here raises the eye behind the car; it is not the final optical look-down angle. |
| Look-at anchoring | **Car centre + `(0, 1.500, 0)` RU**; no lateral offset | **UNDOCUMENTED host convention:** the ELF builds basis vectors, so it has no recovered host target-height scalar ([`camera-constants.md:34-37`](camera-constants.md#L34-L37)). This is the present native target assignment ([`camera.cpp:113-116`](../../src/native/gameplay/camera.cpp#L113-L116)); the centred `0.501` oracle car centres support zero lateral offset. |
| Optical look-down angle on level ground | **8.97 deg down** | **Derived, not ROM:** `atan((2.601 - 1.500) / (7*cos(4.921875 deg)))`. It distinguishes the 4.92-degree orbit parameter from the final eye-to-target ray. |
| Flat-road ground strike | **16.47 RU ahead of car origin** | **Derived, not ROM:** extend eye-to-target to `y=0`: `7*cos(pitch) * 2.601 / (2.601 - 1.500)`. Use as a regression diagnostic, not a fixed road target; terrain collision can change pitch in the original ([`functions/chase-camera.md:8`](functions/chase-camera.md#L8)). |
| Vertical FOV | **1.03 rad = 59.0 deg** | **UNDOCUMENTED-assumed host projection.** ROM equivalents are focal `500` plus X/Y scales `0.47/0.80`, not FOV ([`camera-constants.md:12,17-18,39-42`](camera-constants.md#L12-L18)). The oracle runtime labels `1.03` as its Road Trip vertical-FOV estimate ([`gs_pgs_presenter.cpp:1858-1861`](../../third_party/PS2Recomp/ps2xRuntime/src/lib/gs/gs_pgs_presenter.cpp#L1858-L1861)); it gives `tan(horizontalFOV/2)=0.75`, about 74 degrees across at 4:3 ([`gs_pgs_presenter.cpp:1580-1585`](../../third_party/PS2Recomp/ps2xRuntime/src/lib/gs/gs_pgs_presenter.cpp#L1580-L1585)). |
| Town chase versus race chase | **Same tuple below**; accept effective-horizon bands `0.412` town and `0.394` race | **Measured + constrained:** vehicle scale/centre match. No inspected normal-chase branch derives offset, pitch, or focal from speed ([`camera-constants.md:26-28`](camera-constants.md#L26-L28)); no distinct race preset is recovered. |

### Copy/paste default

```cpp
// Render-space; heading 0 is +Z. FOV is a host assumption, not a ROM scalar.
ChaseCameraTarget{.backDistance = 7.000f, .baseHeight = 2.000f,
                  .orbitPitchRad = 0.08590293f, .lookAtHeight = 1.500f,
                  .lateralOffset = 0.000f, .fovYRad = 1.030f};
```

With the current native convention:

```text
eye    = carPosition - carForward * (7*cos(0.08590293))
       + up * (2 + 7*sin(0.08590293))
target = carPosition + up * 1.5
```

Preserve this pose path: `CarPose.position` and `heading` feed `updateCamera`; it produces eye/target; the view matrix is built from those values ([`game_loop.cpp:66-75`](../../src/native/gameplay/game_loop.cpp#L66-L75)). The snapshot projection then uses `1/tan(fovY/2)` and `t/aspect` ([`game_loop.cpp:76-81`](../../src/native/gameplay/game_loop.cpp#L76-L81)). The preview renderer consumes supplied view/projection matrices directly; its orthographic auto-fit branch runs only when either is absent ([`preview.cpp:576-603`](../../src/native/preview/preview.cpp#L576-L603)).

## 16:9 handling

The 447-line oracle is a scanout capture, not a 16:9 render target ([`oracle-golden-frames.md:109`](oracle-golden-frames.md#L109)). Title/menu screens are 2D-backed original-display material, so the recomp presenter fits them as 4:3 rather than treating their scanout width as evidence for a wide camera ([`raylib_presenter.cpp:255-263,289-294`](../../third_party/PS2Recomp/ps2xRuntime/src/lib/host/raylib_presenter.cpp#L255-L263)). They therefore say nothing about widening chase FOV.

For native **3D driving** output, keep `fovY=1.03` pinned and set projection aspect to actual output width/height. This widens horizontally at 16:9; it does not stretch the 4:3 picture. The established policy says full-window 3D, pinned vertical FOV, square pixels, no vertical crop ([`output-modes.md:100-111,163-175`](output-modes.md#L100-L111)); the app reapplies `extent.width/extent.height` each frame ([`main.cpp:1666-1671`](../../src/native/app/main.cpp#L1666-L1671)). HUD and 2D/title/menu retain a uniform-scale centred authored column or 4:3 frame, never differential X/Y scaling ([`output-modes.md:112-116,142-145`](output-modes.md#L112-L116)).

## Known gaps / do not overclaim

* Exact conventional ROM vertical FOV, host near/far planes, and a host target height are **UNDOCUMENTED**. `1.03` rad and `1.5 RU` are host choices grounded by oracle/recomp practice, not guest scalars.
* There is one town standstill and one moving race reference, not a sweep over all 24 preset records, reverse camera, free camera, or collision states. Preset selection/cycling exists ([`functions/chase-camera.md:9-15`](functions/chase-camera.md#L9-L15)), but their values remain unmeasured.
* The native model does not implement the original terrain-collision response; the recovered routine tests terrain and changes pitch ([`functions/chase-camera.md:8`](functions/chase-camera.md#L8)). A collision shot should preserve the screen-space car target while allowing the derived 16.47-RU flat-road strike to move.
* Attract is recorded above only to prevent its 0.336-height, all-ground composition from being used as the chase target.
