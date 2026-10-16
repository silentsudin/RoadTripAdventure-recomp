# Output modes — standard progressive resolution policy

Updated 2026-10-16. Module: `src/native/app/output_modes.{h,cpp}`; contract
test: `tests/native_assets/test_output_modes.cpp` (`native_assets_output_modes_test`,
166 checks, all passing in `build/outmodes`). Pure C++ (no SDL/Vulkan) — the
app glue below is written for `src/native/app/main.cpp`, which this pass does
**not** touch (coordinator integrates). Line numbers below are from this
checkout during the 2026-10-16 audit; concurrent module dispatch is shifting
`main.cpp`/`preview.cpp`, so anchor by symbol where they disagree.

Requirement this module pins: the native app presents natively (no GS
emulation), so the original's interlaced CRT output is ours to **shed, but
faithfully**: standard output resolutions — 1280x720, 1920x1080, 2560x1440,
3840x2160 (+ custom `WxH` and the native desktop sentinel) — and **every
presented frame is a full progressive frame, never interlaced**.

---

## 1. Original display-mode ledger

Everything in this table is cross-cited from [`frame-timing.md`](frame-timing.md)
(ELF evidence, SLUS-203.98 EE vaddrs) unless another doc is named.

| Constant / vaddr | Fact | Status |
|---|---|---|
| `0x002000d4..0x0020018c` | Display setup: `sceGsResetGraph(0,1,2,1)` then `sceGsSetDefDBuffDc` twice — both double-buffered, alternatives `0x280x0e0` = **640x224** and `0x200x0x100` = **512x256** | CONFIRMED |
| `0x0026e328..0x0026e348` | `sceGsResetGraph` masks arguments into `SetGsCrt(1&1, 2&0xff, 1&1)`; syscall 2 at `0x00284ba0..0x00284ba4` | CONFIRMED |
| `SetGsCrt(1,2,1)` | SDK tuple: **NTSC, interlaced, frame mode**, nominal 59.94-Hz field cadence | INFERRED (tuple semantics; the numeric call is CONFIRMED) |
| `0x00200528..0x002005a8` | Root loop: task walker, DMA kick, then the `sceGsSyncV` VBlank wait each pass | CONFIRMED |
| `0x0020067c..0x002006d8` | `sceGsSwapDBuffDc` per VBlank; XORs the buffer index every wait — one field buffer per parity | CONFIRMED |
| `0x0026e494..0x0026e4a4` | `sceGsSyncV` reads GS CSR (`0x1200_1000`) bit 13 = **field parity**, used to select the buffer/display env | CONFIRMED |
| `0x002005b0..0x002005bc`; `0x26F340/0x26F3C8` | The parity flag feeds the display environments; the recomp proves the rest of the CRT service: 640x224 **fields** drawn, every other one moved down half a line (`sceGsSetHalfOffset`, `halfoff` = parity) | parity path CONFIRMED in-ELF; half-offset semantics from the recomp hook evidence (`CLAUDE.md` "Progressive fields") |
| 640x448 UI space; `+16` centring of the 448-line field in the 480-line frame | HUD authored space; `A+D` vertex formulas; the `+16` is the single INFERRED mapping step | CONFIRMED / centring INFERRED ([`hud-layout.md`](hud-layout.md) §Coordinate space) |
| `0x200x0x100` = 512x256 = PAL side | PAL scales x by 0.8 to 512 — the 512x256 DBuff is the PAL alternative, not the NTSC scanout | INFERRED |
| 640x448 = 10:7 square pixels | 640 px span the full 4:3 display width; 448 of 480 lines are lit, so the active area's physical aspect is (4/3)·(480/448) = 10/7 with square pixels | INFERRED (pure geometry from the CONFIRMED rows) |

**Recomp precedent (reference, not native code):** the sibling recomp defeats
the CRT service at three layers — the `overrides.cpp` hook forces `halfoff =
0`, the pgs backend hides INT/FFMD from paraLLEl-GS, and
`VSyncInfo::progressive_field_scanout` combines both fields into
a **640x448** progressive double-strike picture (8x/16x super-sampling:
**1280x896**) — `CLAUDE.md` "Progressive fields". The native rebuild has no
GS at all, so it does not defeat interlacing; it never has it.

## 2. Progressive by construction (Q1)

The native audit (grep over `src/native/preview/`, `src/native/app/`,
`src/native/gameplay/`) finds **zero** field-parity / half-offset / interlace
logic: no `parity`, no `halfoff` in any form, no buffer-parity-tied y
offsets, and no 224-line render targets. Concretely:

* Every render pass spans the full window extent — scene `renderGeometry`
  (`preview.cpp:618`), 2D quads (`preview.cpp:1157-1180`), HUD glyphs
  (`preview.cpp:1226+`); render area `{0, 0, W, H}`.
* The presenter's `present()` is one full-window RGBA buffer → swapchain blit
  (`app/main.cpp:317-345`): a single `vkCmdCopyBufferToImage` at
  `{extent.width, extent.height}`. There is no per-present field split, no
  odd/even line selection, no half-line shift anywhere downstream.
* All HUD geometry is authored once per frame in the full 640x448 space
  (`gameplay/hud_layout.h:35-41`); nothing renders "a field" and nothing
  shifts by parity.

Therefore **every native presented frame is already a full progressive
frame** — the original's interlace is a CRT *service* (field buffers + CSR
bit-13 parity + `sceGsSetHalfOffset`) with no analogue on a host surface.
Nothing had to change in the renderer for progressiveness. What this pass
adds is policy and enforcement, because "correct by accident" is not
a contract:

1. Standard resolution ladder + parser (§3, §6) — presentation sizes become
   deliberate, not the accident of a 4:3 initial window.
2. Aspect-fit/letterbox math — 3D and HUD each fill what they should and
   nothing stretches.
3. `progressiveContractSatisfied()` (§5) — the checkable invariant.

**The "overlay y=6" story, resolved:** `preview.cpp:986` lifts nav debug
ribbons to `y = 6` *render units* (comment at 983-986: at ground level the
ribbon is sub-pixel against the grazing spawn camera). That is a world-space
lift for readability, **not** a display/scanout parity offset — no change
needed for progressiveness, and it must not be cited as interlace residue.
Likewise `hud_layout.h:36`'s comment ("448 = 224-line field, doubled") is
provenance documentation of the authored space, not runtime field math.

**What still has to change (coordinator glue, not renderer rework):** the
widescreen/HUD placements that today *stretch* (the Q3 inventory in §7)
and the missing window-size contract (§6). The `+16`-in-480 centring used
by `preview/race_hud.h:99-102` is the *display-frame fiction mapping* — an
INFERRED convention for re-anchoring analysis, kept for the 4:3-frame option
(`fitDisplayFrame`), and **not** a parity artifact.

## 3. Scanout math (Q2) and the resolution ladder

**Presentation model.** The authored picture is the combined 640x448 raster
(square pixels, 10:7 active area inside the 4:3 480-line frame; §1). The
native presents that whole raster once per frame — the same combined picture
the recomp's progressive scanout emits (640x448 at 1x, 1280x896 at 2x). Only
1280x896 is an exact integer (x2) multiple of it, so integer-scaling the HUD
inside 720p/1080p/1440p/4K requires a snap choice:

* **3D** fills the whole window. Today's widening, honestly: the rt-app
  in-game path uses the GameLoop projection with `aspect` fixed at
  **800/600 = 4:3** (`gameplay/game_loop.h:43`; `s.proj` passes straight
  through `previewRenderScene`, `preview.cpp:1054/1119`), so at a non-4:3
  window the 3D *stretches* — there is no resize hook. Only the model-preview
  auto-fit camera adapts (`preview.cpp:599-600`, ortho extents widen with
  aspect). **Policy (AUTHORED, recomp-precedent):** the game's projection
  widens horizontally with the window — `GameLoopParams::aspect = extent.w /
  extent.h` with `fovY` pinned — the same intent as the recomp's "narrow the
  projection X to widen" hook (`CLAUDE.md` Widescreen; original X/Y
  projection scales 0.47 / 0.80 at `camera-constants.md:17-18`). Zero
  vertical crop, full-bleed 3D, square pixels at every window shape.
* **HUD / authored-space overlays** never stretch: a single uniform scale
  `s = min(W/640, H/448)` maps the 640x448 raster into the window as a
  centred 10:7 column (`hudPlacement(Exact)`); `s` is one number, so
  x-scale == y-scale by construction. The integer option floors `s`
  (`hudPlacement(IntegerSnapped)`, clamped up to 1x).

**Resolution ladder** (HUD scale factor from the 640x448 authored space, as
computed by `hudPlacement`; "column" = the 10:7 HUD rect inside the window):

| Resolution | 3D fill | HUD scale factor (exact / integer snap) | Filter policy | Open risks |
|---|---|---|---|---|
| **720p** 1280x720 | full 16:9, projection aspect = 16:9 | 720/448 = **45/28 ≈ 1.6071** (column 1028.57x720, pillar bars 125.71 px); snap = x1 (640x448 at (320,136)) — unusable: HUD would occupy half the window height | **linear, exact-ratio for everything** — x1 snaps waste 272 rows; no integer option exists in-window | Text is the softest rung (1.6x linear); acceptable, and the original's HUD batches were linear-sampled anyway (below) |
| **1080p** 1920x1080 | full 16:9 | 1080/448 = **135/56 ≈ 2.4107** (column 1542.86x1080, bars 188.57 px); snap = **x2** → authored raster at (320, 92) **1280x896**, side bars 320 px | **linear exact default**; text/glyph elements may opt into nearest x2 (crisp at exactly the recomp's 2x scanout) | Mixed-scale compositing (x2 text next to 2.41-scale panels) drifts relative element sizes ~17% — pick one mode per composite, not per sprite |
| **1440p** 2560x1440 | full 16:9 | 1440/448 = **45/14 ≈ 3.2143** (column 2057.14x1440, bars 251.43 px); snap = **x3** → (320, 48) **1920x1344**; the 48 px bars are exactly the original 16-line frame borders at x3 (16·3 = 48; 3.33% of frame height) | **linear exact default**; x3 nearest is the best crispness/size trade on the ladder | 48 px black bars inside the HUD column under x3 (visible over 3D on OLED); choose exact-fill if that reads as a defect |
| **4K** 3840x2160 | full 16:9 | 2160/448 = **135/28 ≈ 4.8214** (column 3085.71x2160, bars 377.14 px); snap = **x4** → (640, 184) **2560x1792** | **linear exact**; x4 nearest opt-in for text | x4 bars grow to 184 px t/b — at 4.8x linear is visually clean; recommend against whole-HUD x4 |
| **custom** `WxH` | full window (aspect = W/H) | `min(W/640, H/448)` exact; `floor ≥ 1` snapped — e.g. **1280x896 = exactly x2**, zero bars, the integer-perfect configuration (matches the recomp's 8x/16x super-sampled scanout size) | linear; nearest only when the exact factor is an integer | Windows < 640x448 downscale the authored picture (allowed, deterministic; snapped targets then clip centred) |
| **native** sentinel | desktop display mode at app start | resolved size feeds the same math | as resolved | Fullscreen mode switching (SDL exact mode vs borderless) untested headless |

**Filter policy per element type (AUTHORED):**

* **Flat panels/bars/plates** (race lap panel, gear bar, dialogue frame,
  9-slices): exact-ratio always. Pure solid geometry — letterboxing them for
  an integer snap buys nothing.
* **Atlas glyph/sprite elements** (FONTE text, time/lap digits, speed
  segments): linear at exact ratio by default. This is *faithful*, not
  a compromise: the original's race-HUD arena batches sampled with
  **TEX1 0x60 linear** (`hud-layout.md` §1.1). Integer snap (nearest) is an
  optional sharpness upgrade where the factor ≥ 2 (1080p and up); at 720p
  there is no integer option (45/28), and linear is both original and
  required.
* **Full-screen 2D-backed pages** (title picture, fades): exact-ratio fill of
  the 4:3 display frame (`fitDisplayFrame`), matching the recomp's verdict
  that 2D-backed screens are shown at 4:3 (`CLAUDE.md` Widescreen); fades are
  geometry, unfilled edges blend over the 3D as originally.

**Why the 448 basis (not 480):** the recomp's progressive scanout picture is
640x448 / 1280x896 — the borders are not emitted (the 480-line frame with
its 16-line overscan borders is CRT fiction); the HUD safe area is authored
*inside* 640x448, so a 448-fill presents every authored pixel without
inventing border geometry; and x2 lands exactly at 1280x896. The 4:3-frame
variant (`fitDisplayFrame` + `kAuthoredFrameYOffset`) stays available for
recomp-parity HUD narrowing and 2D screens — note it makes **1440p the
integer-perfect frame** (frame scale 1920x1440 → 3.0: the full 640x480 frame
at x3 with 48 px borders inside, every authored texel exactly x3), versus
720p 1.5 / 1080p 2.25 / 4K 4.5.

## 4. Aspect policy (widescreen stance)

Matches the recomp stance and closes the Widescreen row's "defined policy"
gap in [`feature-retention.md`](feature-retention.md) for output modes:

* **Wide-fill (default):** 3D renders to the full window (widened
  projection, square pixels — never a horizontal stretch of a 4:3 render).
  HUD/authored overlays composite into the centred **10:7 authored column**
  (`hudPlacement`), preserving original proportions; the module also
  provides the recomp-parity centred **4:3 frame** anchor (`fitDisplayFrame`)
  for HUD-narrowing parity and 2D-backed screens.
* **Original-shape pillarbox:** any window narrower than the content stands
  unscaled instead of stretching — `fitContentToWindow` returns viewport +
  bars for any content aspect vs window (16:9 content on a 4:3 window and
  4:3 content in a 16:9 window are the tested rows).
* **Never stretch** is enforced structurally: HUD scale is a single uniform
  factor; the letterbox math only ever produces margins, never differential
  x/y scaling. (Today's `race_hud.h:99-102` places text with independent
  `W/640`, `H/480` factors — a 16:9 window stretches glyphs 33% horizontally;
  the §6 glue replaces it.)

Pixel-exact FOV parity for the widened 3D is harder to claim than
"authored": the original projection's X/Y scale pair (0.4699999988 /
0.8000000119, `camera-constants.md:17-18` CONFIRMED) needs
a capture-comparison against the recovered projection before the wide 3D is
called authentic — flagged in §8.

## 5. The `progressiveContractSatisfied` invariant

`output_modes.h` documents and tests the invariant directly:

```cpp
static_assert(progressiveContractSatisfied({}));            // 448 lines, offset 0
static_assert(!progressiveContractSatisfied({224, 0.f}));   // a lone field
static_assert(!progressiveContractSatisfied({448, 0.5f}));  // the retail CRT half-offset
```

*Any* host present that composes both fields (≥ 448 authored lines) with
zero parity offset passes; field-only presents and any nonzero odd-line
shift (the retail CRT service, `halfoff`) fail. The PAL-shape DBuff
alternative (512x256 fields) is handled by the `fieldLines` parameter. The
app asserts it once at present (§6) so a future regression that splits or
shifts fields fails loudly in debug builds.

## 6. Env contract and the main.cpp glue (sketch — main.cpp untouched here)

```
RT_APP_RESOLUTION=<preset|WxH>
    preset: native | desktop | 720p | 1080p | 1440p | 2160p | 4k | uhd
            (case-insensitive; native/desktop = desktop display mode at start)
    WxH:    exact size, integers 1..32768, e.g. 1920x1080, 1280x896
    invalid values log an "rt-app resolution:" line and fall back to the
    desktop mode — never abort; unset keeps today's 800x600 windowed default
    byte-for-byte (matrix-regression quiet).
RT_APP_FULLSCREEN=<0|1>
    1 = SDL borderless fullscreen at the resolved resolution (default 0).
```

Glue, matching the existing env-helper conventions (`envFrames()`,
`envInterpEnabled()`, `main.cpp:355-373`):

```cpp
#include "app/output_modes.h"          // beside the other app/preview includes

rt::native::app::Resolution envResolution() {
    const char* text = std::getenv("RT_APP_RESOLUTION");
    if (text == nullptr || *text == '\0') return {};   // sentinel: native
    const rt::native::app::Resolution parsed = rt::native::app::parseResolution(text);
    if (!rt::native::app::valid(parsed) &&
        !rt::native::app::isPresetName(text)) {        // sentinel vs garbage
        std::cout << "rt-app resolution: invalid RT_APP_RESOLUTION='" << text
                  << "' (expected <preset|WxH>), using desktop\n";
        return {};
    }
    return parsed;
}

bool envFullscreen() {
    const char* text = std::getenv("RT_APP_FULLSCREEN");
    return text != nullptr && std::strcmp(text, "1") == 0;
}
```

Window creation (replaces the fixed `kInitialWidth = 800, kInitialHeight =
600` at the top of `main()`):

```cpp
int main() {
    const rt::native::app::Resolution want = envResolution();
    const bool fullscreen = envFullscreen();
    int width = want.width, height = want.height;
    if (rt::native::app::isNativeDesktop(want)) {
        SDL_DisplayMode desktop{};
        SDL_GetDesktopDisplayMode(SDL_GetPrimaryDisplay(), &desktop);
        width = desktop.w; height = desktop.h;
    }
    const SDL_WindowFlags flags = SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE |
                                  (fullscreen ? SDL_WINDOW_FULLSCREEN : 0);
    SDL_Window* window = SDL_CreateWindow("Road Trip authored scene",
                                          width, height, flags);
    if (fullscreen && window) (void)SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    if (std::getenv("RT_APP_RESOLUTION") || std::getenv("RT_APP_FULLSCREEN"))
        std::cout << "rt-app resolution: " << width << 'x' << height
                  << (fullscreen ? " fullscreen" : " windowed") << '\n';
    // ...
```

3D aspect on init and resize (inside `recreateSwapchain`, after the
preview re-init; and once at session creation):

```cpp
loopParams.aspect = static_cast<float>(presenter.extent.width) /
                    static_cast<float>(presenter.extent.height);
```

HUD composite (replaces the per-axis stretch in `fonteTextQuads`,
`race_hud.h:99-102`; uniform scale + offset from the module):

```cpp
const rt::native::app::HudPlacement hud =
    rt::native::app::hudPlacement(width, height,
                                  rt::native::app::HudScaleMode::Exact);
// authored-space point (uiX, uiY) -> window px (never stretch):
const float px = hud.target.x + uiX * hud.scale;
const float py = hud.target.y + uiY * hud.scale;
```

The `hudPlacement` call is one line per composite; the `+16` display-frame
variant (for 2D-backed pages / recomp HUD-narrowing parity) is
`fitDisplayFrame(width, height).viewport` +
`(uiY + kAuthoredFrameYOffset) * (viewport.height / 480)`.

Present-time assertion (debug builds):

```cpp
assert(rt::native::app::progressiveContractSatisfied({}) &&
       "native present must be one full progressive frame, no parity offset");
```

## 7. Native site inventory (Q3): what assumes which line counts today

All from this checkout's audit; `main.cpp`/`preview.cpp` line numbers move
under concurrent integration — anchor by symbol.

| Site | Assumption | Policy for it |
|---|---|---|
| `gameplay/hud_layout.h:35-41` | Authored-space constants `kHudUiWidth 640`, `kHudUiHeight 448` ("224-line field, doubled"), `kFrameWidth/Height 640/480`, `kFrameYOffset 16` | Keep: the authored contract this module's constants mirror (`kAuthored*`) |
| `gameplay/hud_layout.h:372-380` | `normalizedFrame640x480()` maps authored rects into the 640x480 frame (y +16); currently has no callers | Keep as the recomp-parity analysis helper; the module's `fitDisplayFrame` is its geometry counterpart |
| `preview/race_hud.h:36-37` | `raceHudQuads` authored indicator panel space **800x600**, uniform `max(1, min(W/800, H/600))` | Native stand-in panels (not ROM geometry); optionally migrate denominators to `hudPlacement`, or keep as its own authored mini-space |
| `preview/race_hud.h:94-113` | `fonteTextQuads` maps authentic 640x448 anchors with independent factors `sx = W/640`, `sy = H/480` and `y = (uiY + 16) * sy` — **stretches text at non-4:3 windows** | The one window↔original-line-count coupling in the tree (480 frame / 448 authored); migrate to the §6 uniform placement |
| `gameplay/game_loop.h:43` | `GameLoopParams::aspect = 800/600` — the 3D projection is a fixed 4:3 and nothing on resize updates it (`main.cpp` has no aspect write) | Glue: `aspect = extent.w / extent.h` at init + resize |
| `app/main.cpp:795-798` | `kInitialWidth = 800, kInitialHeight = 600` + `SDL_WINDOW_RESIZABLE`; all later sizes derive from `presenter.extent` | Replaced by the §6 env-resolved creation; resize path stays |
| `app/main.cpp:menuQuads` (768-776 in this audit) | Menu authored space **1024x720**, scaled `min(W/1024, H/720)` into the window | Own authored space (its aspect 1.422 ≈ 10:7 already); optionally unify on `hudPlacement` |
| `app/main.cpp:recreateSwapchain` (954-962) | On out-of-date: rebuild swapchain + re-init preview at the new extent | Add the aspect + placement recompute hook here |
| `preview/preview.cpp:599-600` | Model-preview auto-fit camera adapts ortho extents to the current render aspect | Unchanged (tool framing, not the game camera) |
| `preview/preview.cpp:979-986` | Nav debug ribbons lifted to `y = 6` render units (world-space) | Unchanged; explicitly **not** a parity artifact (§2) |
| `app/main.cpp:317-345` | `present()`: full-window RGBA→swapchain blit, one image per frame | The host-present half of the progressive contract; assert §5 |

No site in `src/native` divides a window dimension by 224 or renders
half-frames; the only original-line-count couplings are the authored-space
constants (by design) and the `race_hud.h` 640/480 mapping (migrate).

## 8. Open risks / ledger

* **CONFIRMED:** Ledgers §1 (via `frame-timing.md`/`hud-layout.md` rows),
  the recomp progressive precedent, the native no-parity audit (§2), and the
  module's arithmetic contracts (166 test checks in `build/outmodes`).
* **AUTHORED (this pass):** the resolution ladder, the 10:7 authored-column
  HUD anchor with the 448 basis, the wide-fill projection policy, the filter
  per-element defaults, the env contract, and the `progressiveContractSatisfied`
  boundary semantics (≥ 2x field lines, offset exactly 0).
* **Open:**
  * Pixel-exact FOV parity for widened 3D — needs a capture comparison
    against the recovered 0.47/0.80 projection pair (§4).
  * 2D-backed screen verdicts (recomp classifier parity, title/loading
    pages at 4:3) are policy hooks here; no native classifier exists yet
    (feature-retention Second-screen/Widescreen rows).
  * Fullscreen mode switching (exact vs borderless) and HiDPI scales
    (`SDL_GetWindowSizeInPixels` vs logical points) untested headless.
  * Snapped-HUD bars over live 3D (1440p x3's 48 px) need a visual pass;
    the module treats bars as geometry the app can clear or leave
    transparent.
  * `raceHudQuads` (800x600) and `menuQuads` (1024x720) authored spaces are
    native stand-ins; unifying them on the module is cosmetic, left to the
    coordinator.
