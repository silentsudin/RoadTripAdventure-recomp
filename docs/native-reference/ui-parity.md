# Native UI-parity specification

**Status:** implementation contract for `re/native-complete`. The reference is the **recomp application's replacement UI**, not an unmodified PS2 menu. An updater must not see a UI difference unless it is explicitly in the deviations ledger. Sources and line pins below are from this branch on 2026-10-16.

## Non-negotiable rendering contract

- Render replacement UI in the recomp vocabulary: chunky outlined type, orange/olive dialogs, sky-blue lists, blue selection capsule, animated gold horn. Never expose OS, ImGui, or native diagnostic UI.
- Recomp design scale is `S=max(.5,H/1080*handheldBoost)` (`src/ui/Theme.cpp:68-101`); use it for all listed design pixels.
- FONTE/game UI coordinates are 640x448, top-left. Native maps them with one uniform factor and letterbox centring: `s=min(W/640,H/480)`, `x'=(W-640s)/2+x*s`, `y'=(H-480s)/2+(y+16)*s` (`src/native/preview/race_hud.h:87-112`, `docs/native-reference/hud-layout.md:11-17`). This keeps a centered 4:3 field: at 1080p the factor is 2.25. Do not stretch.
- This branch contains no `output-modes.md`; the preceding mapping is the available source of truth. Pause/setup chrome is viewport-relative and scales by `S`.

## Screen inventory

### Title and attract

Keep the recomp title/attract treatment clean: no replacement title card and no native labels such as `Road Trip authored scene`, `Start state`, or debug controls (`src/native/app/main.cpp:915-920`, `src/native/gameplay/settings.cpp:408-443`). First-run setup appears only when required. Attract/demo has neither state rows nor Radio station (`CLAUDE.md:90`, `src/game/GameOptions.cpp:70-74`).

### Pause root — `Menu`

Open with **Guide**, Back+Start held 0.3 s, Esc, F3, Android Back, or second-screen Menu; pause and block game input (`src/ui/PauseMenu.h:4,24`). Slide in over 0.18 s, quadratic ease; close with 0.2 s scrim fade (`src/ui/PauseMenu.cpp:238-265,1817-1824`). Exact order:

1. `Resume`
2. `Save state` — only when save-state predicate passes.
3. `Load state` — same predicate.
4. `Options`
5. `Controllers`
6. `Quit`

Root width is 520*S; subpages 880*S, each capped at 92% viewport. Standard rows are 80*S, state cards 136*S; panel padding 30*S, inner side inset 30*S, safe margin 24*S, name tab 44*S, prompt zone 90*S (`PauseMenu.cpp:1838-1887`). Panel geometry does not change with focus; picture-preview rows reduce scrim to 55%. Root Resume/Quit hints format player summary as `Player 1: ...     Player 2: ...`, including ` NN%` battery (`PauseMenu.cpp:107-121`).

### Pause Options — one headed recomp page

Native `MenuNavigator` currently has separate pages; the parity renderer must adapt it into this **single** recomp page. Heading rows are not focusable; selectable rows wrap on Up/Down. Left/Right cycles values, Confirm opens/acts, and movement/change/confirm/cancel use the recomp menu cues. `test_settings` is the navigation pin, not the presentation order.

**Display** (`src/ui/PauseMenu.cpp:397-525`), exact order:

| Row | Exact values | Visible only when |
|---|---|---|
| `Display mode` | `Window`; `Borderless fullscreen`; `Fullscreen` | not Android |
| `Window size` | `Small (640x448)`; `Medium (960x672)`; `Large (1280x896)`; `Largest (WxH)`; `Custom (WxH)` | non-Android + Window |
| `Aspect ratio` | `4:3 (original)`; `16:10`; `16:9`; `21:9`; `32:9` | 16:9 capability |
| `HUD position` | `4:3 centred`; `Screen edges` | Aspect row visible |
| `Map` | `Enhanced`; `Original` | hardware GS, and not second display + enabled |
| `Frame rate` | `60 fps (original)`; `120 fps (generated)`; `240 fps (generated)`; `60 fps (with Arm ASR)` | 120 Hz capability |
| `Frame generation` | `Re-rendered (no added latency)`; `Warped, interpolated (+1 refresh latency)`; `Warped, extrapolated (edge artefacts)` | >60 fps and `RT_FRAME_GEN=1` |
| `Frame skip` | `Auto`; `Off` | always |
| `Performance overlay` | `Off`; `Frame rate`; `Detailed` | always |
| `Second screen` | `On`; `Off` | presentation display exists |

**Sound** (`PauseMenu.cpp:526-542`): `Volume` is `Off` or `NN%` in 10% increments; `Speaker` is `Mono`/`Stereo`; Adventure-only `Radio station` is `Off`/`PEACH FM`/`E-RADIO` (`GameOptions.cpp:59-67`).

**Graphics** (`PauseMenu.cpp:543-766`), exact order after filtering: `Renderer` (`Hardware`, `Accurate (paraLLEl-GS)`; GPU GS + reference renderers); `Supersampling (SSAA)` (exact strings at lines 563-597); `Mipmaps` (`On (original)`, `Off (sharpest)`, accurate only); `Fix steering bias`; `High detail cars`; `Draw distance`; `Race car shadows` (each `On`/`Off`); `Texture pack` (`None installed`, `None (original)`, pack name / ` (missing)` / `  NN%`); `Interlacing` (`Off (progressive fields)`, `On (original)`); available `Anti-aliasing` (`Off`, `FXAA`, `SMAA`, `TAA (temporal)`); available `Upscaling` (`Bilinear`, `AMD FSR 1`, `Snapdragon GSR 1`, `Snapdragon GSR 2`, `Arm ASR`, `AMD FSR 3`, `NVIDIA DLSS`, `Intel XeSS`, `MetalFX spatial`, `MetalFX temporal`); `Sharpening` (`NN%`) only for FSR1/GSR2/ASR/FSR3/DLSS/XeSS.

**Pending native GPU features:** hide them. Do not show current model-preview `Anti-aliasing`, `Upscaler`, `Map mode`, `Second screen`, `Rumble`, or `Radio station` rows (`settings.cpp:432-443,478-500`) before their effects work. Recomp hides unavailable/not-built rows (`PauseMenu.cpp:390-392`, `CLAUDE.md:87`); it does not advertise previews. Also hide native-only Start state/interpolation/snapshot/disc/terrain/stream/nav/route/car test/HUD test/BGM-path/debug rows in retail UI.

### Controllers and children

`Controllers` exact order (`PauseMenu.cpp:813-872`): heading `Devices`; each detected `<name>  NN%` valued `Player 1`/`Player 2`/`Not playing`; with no pad, note `Connect a controller and it appears here.`; heading `Driving`; `Controls` = `Modern`/`Classic`/`Custom`; if a gamepad, heading `Vibration`, `Vibration` = `Dynamic`/`Classic`/`Off`, `Strength` = Off or 0/25/50/75/100%. Strength disables when Off and a vibration change samples feedback. Modern defaults except keyboard-only Classic.

Device details: `Plays as`, `Buttons`, then pads get `Left stick deadzone`, `Right stick deadzone` (0–50%, 5% increments), `Vibration`, `Test vibration` (`PauseMenu.cpp:875-908`). Buttons page heading is `Game button` / `Your key` or `Your control`; exact order is `Up`, `Down`, `Left`, `Right`, `Cross`, `Circle`, `Square`, `Triangle`, `L1`, `R1`, `L2`, `R2`, `Start`, `Select`, `L3`, `R3` (`911-934`). Driving: heading `Layout`, `Controls`, heading `Action`/`Your control`, recomp drive actions, then (with pad) heading `Feel`, `Analogue triggers` (`979-1048`). Render binding glyphs from the controller family at draw time (`src/platform/Controllers.h:18-78`, `glyph-repaint.md:100-136`).

### Quit and save states

Retain recomp confirmation copy and safe default focus (Quit opens on cancel/back; `PauseMenu.cpp:302,1160-1215`). State rows show only in Adventure or player's race, never movie, and only if renderer can snapshot (`src/native/gameplay/settings.cpp:367-371`; recomp `CLAUDE.md:90`). Recomp has four numbered thumbnail cards, empty/unreadable states, overwrite/load confirmations, and notices such as `Saved in slot N.` (`PauseMenu.cpp:324-365,1725-1780`). Native's current two `slot<N>.sav` slots (`options-menu.md:265-273`) are a model gap: implement four visible one-based cards and thumbnails before enabling the rows.

### First run / setup

Use the sunny animated-road background; centered `Road Trip` dialog with 56*S pad and width `min(viewportW-80*S,viewportH*1.2)` (`SetupScreen.cpp:36-62,164-212`). Welcome exact copy: `Welcome!`; `Road Trip needs your own copy of the game. Choose the disc image you made from it (.bin, .iso or .chd).`; `Choose disc image` (`353-370`). Preserve all image-warning text/button ordering at `373-404`, with safe choice focused. Buttons clamp left/right, Enter/Start/A confirms, gold rim + horn (`83-136`).

Progress defaults to `Getting Road Trip ready`; phase `Step N of M: <phase>`; 44*S eased, non-decreasing gold bar; left `NN%` or `M:SS`; right ETA or `Working...`; footer `This only happens once. You can switch to other apps meanwhile.` (`264-350`). Native omits only recomp's compile phase, using `Preparing Road Trip`; ledgered below.

### Second screen

Own sky-blue output with `Map`, `Notebook`, `Radio` tabs and green `Menu`/`Close` pill. Panel margin 24*S, tab row 84*S, 30*S content inset, max 240*S tabs, 168*S Menu (`SecondScreen.cpp:731-765`). Active tab is orange/gold with cyan rivets, inactive blue. It auto-selects Map while map/racing, Notebook otherwise; user override lasts to context change; a new stamp opens Notebook except when map is visible (`697-710`). Throttle to 30 fps except touch, retaining last image (`848-888`).

- **Map:** map left, 250*S stats column; town now-playing strip 64*S. Race column includes course, `Get ready`, `Lap`/`Finished`, `Last lap`, `Best lap`/`New best lap!`; town column has Time, Photos here, Houses to visit, Coins found (`174-245,775-824`).
- **Notebook:** Adventure player/miles header; Licence/Money/Coins/Towns/Photos chips; `Got N` / `To get N`; stamp cards and drag scrolling; retain empty strings and 3.5-second stamp animation (`323-480`).
- **Radio:** `PEACH FM`, `E-RADIO`, `Off` plates, song strip/title/time; unavailable message `The radio plays in town, in an Adventure game.` (`558-623`).

### Enhanced map

Gold rim `E8C830` / highlight `FFF0A0`, navy ground `0B1240`. Mark in strict priority Factory, Shop, PhotoBooth, House; omit completed PhotoBooth/House. Shop overlaps Factory: move 2.3 radii; PhotoBooth/House overlap <1.6 radii: drop. Markers over car shrink .7 and use alpha `59` (`MapView.cpp:79-191`). All have navy stroke: orange two-ring `Q`, cyan letter shop, pink camera, white unvisited-house dot. This is the recomp Enhanced map, not a generic minimap.

### Performance overlay

Recomp has a left-bottom rounded navy `0B1240B8` pill, x=16*S / bottom=12*S, `2C6CE8` 2*S rim (`PerfOverlay.cpp:19-96`). Copy/value order: `<N> fps`, optional `game <N>`; detailed `worst N.N ms`, `VU1 N%  GS N%`, `CPU N%`, optional `GPU N% @ N MHz`, `Battery N deg C`. Native implements the row bottom-edge on the FONTE path from `gameplay/perfstats` with that copy/order and follows `display.perf_overlay` (Off default), but only draws what it can measure: `N fps`, `game N`, `worst N.N ms`, `CPU N%`. VU1/GS busy, GPU and battery segments have no native counter and are never drawn (their templates are in the code; the numbers are not invented). Still non-parity: the pill is a square-cornered box (no rounded rim, segment dividers or text outline). The native `[perf] fps=... game=...` log stays diagnostic only. Omit unknown fields; never print `N/A`.

## Text-parity contract

**String inventory: 184 static/user-visible templates**, plus data substitutions (device/pack/slot/caption/song/path/numeric values). The implementation must generate a checked manifest from these exact source ranges, then render every manifest string with identical case and punctuation:

| Source | Lines | Scope |
|---|---:|---|
| `src/ui/PauseMenu.cpp` | 293-303, 324-365, 397-768, 813-1048, 1796-2089 | root/options/controllers/state strings and hints |
| `src/game/GameOptions.cpp` | 59-67 | station values |
| `src/ui/SetupScreen.cpp` | 264-405 | welcome/progress/warnings/errors/buttons |
| `src/ui/SecondScreen.cpp` | 174-245, 323-623, 626-824 | tabs/columns/notebook/radio/map messages |
| `src/ui/MapView.cpp` | 40-191 | map marker labels |
| `src/ui/PerfOverlay.cpp` | 19-96 | overlay labels/units |
| `src/platform/Controllers.h` | 3-78 | public controller language/glyph semantics |

All text uses `fonteTextQuads`/the proven ASCII slot mapping (`race_hud.h:87-112`, `hud-layout.md:142-149`): `!`–`~` emit; space advances. Never silently drop another byte. Required native ASCII substitutions for currently unrenderable recomp punctuation are: `Press…` → `Press...` (`PauseMenu.cpp:2002`), `…` in pack names → `...` (`686-693`), em dash `—` → ` - ` (`SecondScreen.cpp:491`), `Battery N °C` → `Battery N deg C` (`PerfOverlay.cpp:59`). Controller face symbols are vector Shapes, not ungrounded FONTE codes (`glyph-repaint.md:9-35,100-113`).

## Color / geometry contract

| Recomp value/source | Native value |
|---|---|
| olive `485800F2`; frame outer/frame/light `C05B1E`/`F86018`/`F8BF84` | identical RGBA (`Theme.h:17-21`; construction `Theme.cpp:162-200`) |
| list `0098C8`/`8ED4F0`/`0F5E92`; text `F6E68F`, selected `ECECEC` | identical (`Theme.h:35-41`) |
| selection `2C6CE8`/`0838B0`/`F8C838`, horn `F2C230` | identical (`Theme.h:25-31`, `Theme.cpp:212-248`) |
| prompt `304090F2`, rims `91D2E8`/`1CA1E6`, outlines `1A1A10`/`000820`, scrim `000A2899` | identical (`Theme.h:44-50`) |
| frame strokes 3/6/3*S; standard roundings 18/14/12*S | identical (`Theme.cpp:116-127,162-200`) |
| Pause 520/880*S, rows 80*S, pad 30*S, margin 24*S | identical (`PauseMenu.cpp:1838-1887`) |
| game UI 640x448 | exact 640/480 transform in Non-negotiable contract |

## Acceptable-deviations ledger

Only these are allowed. **Anything not here is a defect.**

1. **Renderer-backend pixels — visible-inevitable.** Native Vulkan versus recomp GS/ImGui can differ in AA coverage, texture sampling and scaling filter. Geometry, colors, draw order, copy and controls still match.
2. **Progressive fields — visible-inevitable.** Recomp can use original interlaced or progressive fields; native currently presents progressive. Keep `Interlacing` hidden until native behavior matches.
3. **Widescreen policy — visible-inevitable.** Recomp widens 3D while retaining 4:3 UI (or moving HUD to edges); native currently has arbitrary swapchain output. Apply centered 640/480 mapping; stretched UI is never allowed.
4. **No compile setup step — visible-inevitable.** Native does not compile a recompiled game. Omit only that phase and use `Preparing Road Trip`; shared setup layout/bar/ETA/copy remains identical.
5. **FONTE punctuation substitutions — invisible.** Only the four Text-parity substitutions are allowed until glyph evidence exists.

## Verification protocol

### Structural checks runnable now

The executable structural gate is `tests/native_ui/w14_ui_parity.cpp` (target `w14_ui_parity`): it pins the recomp literal manifest per owning surface, the adapter row order/visibility, FONTE renderability and the theme/geometry numbers. It proves **literal/adapter presence, not that a surface is rendered**; a group going green is necessary, not sufficient.

Current status (2026-10-28): 18/18 green. The last red group, `manifest/second-screen-backlog`, closed with a real surface: `src/native/preview/second_screen.{h,cpp}` builds the Map / Notebook / Radio pages (tabs, Menu pill, race and town columns, notebook chips, `Got`/`To get`, stamp cards, radio plates) from native state (`gameplay/save.h`, the race snapshot, `gameplay/radio`), drawn on the FONTE path; `RT_APP_MENU_SCREEN=second-map|second-notebook|second-radio` renders a page to `RT_APP_SNAPSHOT` for review, and `RT_SECOND_DEBUG=1` logs the recomp's `[second]` page-decision line. Still missing there: the **Android presentation display** (no Presentation/JNI seam yet — the page is not on the Thor's second display), a real `MapDrawList` feed (the Map page shows its empty-state message), stamp art from `SYS/STAMP.GSL`, course/place/photos/now-playing values, and tap handling. `manifest/setup-backlog` is green on wording/layout literals, and the first-run setup screen now renders in a window (progress bar, step, ETA, and the failure screen's Close / Try again / Use it buttons, keyboard focus); headless runs keep the old instant path. Still missing on setup: Welcome and Warning screens (native has no disc picker so they are not produced), mouse/touch/gamepad hit-testing (buttons are keyboard-only), the horn and road background, the eased bar, and FONTE text before the font is extracted from the disc. The Options panel and Controllers page render (headless shot: `RT_APP_STATE=MENU RT_APP_MENU_SCREEN=options|controllers`). Not yet parity: the temporary "Controllers" link row inside Options (the recomp opens it from the Pause root, which native lacks), device names/battery, device-detail and Driving-controls pages, Quit confirmation, state cards, and panel chrome rivets/hints/cues (rounding and the eight-way text outline are in; the second screen has the recomp's rounded panel/tabs/Menu/cards/plates, its sky gradient and its radio plate art), and rows marked `HostGluePending` (persisted and shown, no consumer yet: `Second screen`, `Radio station`).

1. Build a source-literal → native-manifest diff (including approved ASCII normalization); fail on missing, extra, case, or punctuation change.
2. Add adapter tests for rich/Android/no-pad/no-display/non-Adventure/movie contexts: assert this row order and assert pending GPU rows are **hidden**.
3. Assert adapter mappings against `OptionsModel`, keeping its IDs internal. Extend `tests/native_assets/test_settings.cpp`; catalog/availability/wrap/skip/deterministic focus are its existing pin (`options-menu.md:277-300`, `test_settings.cpp:323-468`).
4. Test every manifest string for ASCII-or-approved-Shape and a FONTE glyph/space advance per character.
5. Numerically test theme/rect constants at 1080p and 2160p, 640x448 mapping, tab/panel bounds, and overlay-HUD clearance.

### Dual-run pixels (user machine and ROM)

Fix resolution, aspect, settings, locale, controller family, state, and vblank. Recomp capture: `RT_MENU_SHOT` (`root|display|graphics|controllers|device|buttons|quit|reset`), `RT_MENU_AT`, and `RT_FRAME_DUMP` (`CLAUDE.md:87-88`). Native capture: `RT_APP_SNAPSHOT` after the equivalent deterministic state, or headless `RT_APP_STATE=MENU RT_APP_MENU_SCREEN=options|controllers RT_APP_HEADLESS=1 RT_APP_SNAPSHOT=…`. Convert only for comparison. Commit **hashes and metadata, never PNG goldens**.

Tolerance: **T0** exact manifest/row/order/coordinates/flat colors; **T1** one-pixel AA fringe and <=2 channel levels; **T2** bounded AA/filter variance only in renderer-marked image regions; **T3** masks only ledgered progressive/widescreen/setup-compile regions. New hash requires a linked ledger rationale and reviewed metric.

Failure triage order: (1) screen/state and visibility predicate; (2) literal manifest/FONTE fallback; (3) 640/448 or `S` transform; (4) z-order/scrim/panel/theme literal; (5) backend tolerance or ledger only last.

## Model-layer gaps

Before claiming parity, add backing for Pause root/Quit confirmation; one headed Options page; recomp display/frame/renderer/quality/pack/interlace settings; device assignment/rebinding/driving controls; four thumbnail save slots; setup state machine; second display/tabs/map/notebook/radio; and recomp overlay presentation. `OptionsModel` is a useful persistence/navigation core but today exposes native-only diagnostics and GPU-wave-pending rows rather than the recomp catalog.
