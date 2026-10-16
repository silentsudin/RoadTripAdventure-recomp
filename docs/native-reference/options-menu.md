# Native options menu — catalog, persistence, and renderer glue

Status: **authored-for-native LOGIC** (store + catalog + navigator), 2026-10-16.
The persisted-settings store, the option catalog and the gamepad menu navigator
live in [`src/native/gameplay/settings.{h,cpp}`](../../../src/native/gameplay/settings.h);
the contract tests are [`tests/native_assets/test_settings.cpp`](../../../tests/native_assets/test_settings.cpp).
This document is the catalog table, the persistence contract, the pause
integration, the save-state row contract and the renderer glue.

> **REFERENCE PRECEDENT (CLAUDE.md:87).** The sibling recomp app has a full
> controller-navigable pause menu (Display/Graphics/Controllers/Sound pages) with
> `settings.toml` persistence, and "options a device can't use, or that aren't
> built yet, are hidden". This module reproduces that *shape* against the flags
> the native app actually has today (the `RT_APP_*` env vars). It is
> AUTHORED-for-native: no sibling code, no sibling TOML tables, no guest data.

Today the native app is configured only by `RT_APP_*` environment variables and
has no user-facing menu and no persistence (the retention audit found no
menu/save-state commands — `feature-retention.md:28`). This module adds the
logic layer; the app wiring is the follow-on glue (§4).

---

## 1. Enumerated configuration flags (source of truth)

Every `RT_APP_*` flag `src/native/app/main.cpp` (plus the one preview flag) reads,
with its default and valid values. `main.cpp` is READ-ONLY here; this table is
the inventory the catalog is mapped against.

| Flag | File:line | Default | Valid values |
|---|---|---|---|
| `RT_APP_STATE` | `app/main.cpp:761` | `TITLE` | `TITLE`, `MENU`, `PLAYING` |
| `RT_APP_FRAMES` | `app/main.cpp:354` | `0` (unlimited) | non-negative int (clamped `>= 0`) |
| `RT_APP_NORENDER` | `app/main.cpp:822` | unset (render) | present = headless |
| `RT_APP_INTERP` | `app/main.cpp:362` | `0` | unset/`0` = off; any other value = on |
| `RT_APP_INTERP_CAP` | `app/main.cpp:369` | `2` (`kDefaultCatchUpCap`) | int `>= 1` |
| `RT_APP_INTERP_RATE` | `app/main.cpp:377` | `0` (vsync) | int `>= 0` (0 = swapchain cadence) |
| `RT_APP_TERRAIN` | `app/main.cpp:620` | unset (grass quad) | present = real FLD terrain |
| `RT_APP_TEXTURES` | `app/main.cpp:621` | unset (flat) | present = decode tile uploads to RGBA8 |
| `RT_APP_TERRAIN_FILE` | `app/main.cpp:624` | unset (64-tile grid) | path to one FLD tile; ignored under `RT_APP_STREAM` (`main.cpp:828`) |
| `RT_APP_STREAM` | `app/main.cpp:827` | unset (eager load) | present (with `RT_APP_TERRAIN`) = 4-slot streaming |
| `RT_APP_STREAM_STEP_FRAMES` | `app/main.cpp:839` | `10` | int (sweep period; no clamp in app) |
| `RT_APP_STREAM_ROW` | `app/main.cpp:840` | `1` | int (id-grid row 0..7 in practice) |
| `RT_APP_STREAM_POS` | `app/main.cpp:1261` | unset (scripted sweep) | present = position-driven demand |
| `RT_APP_NAV` | `app/main.cpp:859` | unset (no overlay) | present = nav-graph overlay |
| `RT_APP_NAV_TOWN` | `app/main.cpp:864` | unset (static graph 0) | present (with `RT_APP_NAV`) = town street graph |
| `RT_APP_ROUTE` | `app/main.cpp:890` | unset | present = drive the demo car along graph 0 |
| `RT_APP_ROUTE_TOWN` | `app/main.cpp:895` | unset (static graph) | present (with `RT_APP_ROUTE`) = town graph |
| `RT_APP_HUDTEXT` | `app/main.cpp:919` | unset | present = FONTE HUD text |
| `RT_APP_SCOREHUD` | `app/main.cpp:925` | unset (off) | unset, any non-`meter` value (`score`), `meter` |
| `RT_APP_CARMODEL` | `app/main.cpp:970` | unset (demo box) | unset, empty (`0`), or int Q index |
| `RT_APP_CARMODEL_DUMP` | `app/main.cpp:1020` | unset | path (PPM livery dump) |
| `RT_APP_NOAUDIO` | `app/main.cpp:953` | unset (audio on) | present = no audio device |
| `RT_APP_BGM` | `app/main.cpp:127` | `BGM_01` | TSQ track name or a `.vag` path |
| `RT_APP_BGM_TSQ` | `app/main.cpp:128` | unset (TSQ tried) | `0` = force VAG; else try TSQ |
| `RT_APP_SNAPSHOT` | `app/main.cpp:1367` | unset | path (one PPM per frame) |
| `RT_APP_DEBUG_OVERLAY` | `preview/preview.cpp:1027` | unset | present = frustum stats overlay |
| `RT_DISC` | `app/main.cpp:131,480` | `~/Library/Application Support/RoadTripRecomp/disc` | path to a disc extract |
| `RT_MENU` | new (this module) | unset | `1` = open the pause menu (interim) |

`RT_APP_TRIGGERS` is referenced by the trigger-mapping commit message
(`docs/native-reference/feature-retention.md` / `input.{h,cpp}`) but is not yet
read by `main.cpp`; the Controllers → Analog trigger drive row stands in for it
and is flagged GPU-wave-pending.

---

## 2. Option catalog

`id` is the persisted key and the row id. `maps-to` is the env flag(s) the row
stands in for (an empty cell = no consumer yet = GPU-wave-pending). `availability`
is the `MenuContext` predicate that hides the row (`settings.h`
`OptionAvailability`). `status` is `wired-now` (a live consumer exists today) or
`gpu-wave-pending` (persisted and shown, honest that nothing reads it yet).

### Display

| id | label | maps-to env/flag | default | availability | status |
|---|---|---|---|---|---|
| `display.start_state` | Start state | `RT_APP_STATE` | `title` | always | wired-now |
| `display.menu` | Options menu (Esc/Start) | `RT_MENU` | off | always | wired-now (interim) |
| `display.interp` | Frame interpolation | `RT_APP_INTERP` | off | always | wired-now |
| `display.interp_cap` | Interp catch-up cap | `RT_APP_INTERP_CAP` | 2 | requires interp | wired-now |
| `display.interp_rate` | Interp render rate (fps) | `RT_APP_INTERP_RATE` | 0 (vsync) | requires interp | wired-now |
| `display.render` | Render to window | `RT_APP_NORENDER` (inverted) | on | always | wired-now |
| `display.snapshot_path` | Snapshot path (PPM) | `RT_APP_SNAPSHOT` | (empty) | always | wired-now |
| `display.frame_budget` | Frame budget | `RT_APP_FRAMES` | 0 (unlimited) | always | wired-now |
| `display.disc_root` | Disc extract root | `RT_DISC` | (empty → default) | always | wired-now |
| `display.debug_overlay` | Debug overlay | `RT_APP_DEBUG_OVERLAY` | off | always | wired-now |
| `display.aa` | Anti-aliasing | — | `off` | always | **gpu-wave-pending** |
| `display.upscaler` | Upscaler | — | `none` | always | **gpu-wave-pending** |
| `display.map_mode` | Map mode | — | `game` | always | **gpu-wave-pending** |
| `display.second_screen` | Second screen | — | on | requires second screen | **gpu-wave-pending** |

### Graphics

| id | label | maps-to env/flag | default | availability | status |
|---|---|---|---|---|---|
| `graphics.terrain` | Disc terrain | `RT_APP_TERRAIN` | off | always | wired-now |
| `graphics.textures` | Field textures | `RT_APP_TEXTURES` | off | requires terrain | wired-now |
| `graphics.terrain_file` | Single tile file | `RT_APP_TERRAIN_FILE` | (empty) | requires terrain | wired-now |
| `graphics.stream` | Tile streaming (4-slot) | `RT_APP_STREAM` | off | requires terrain | wired-now |
| `graphics.stream_step_frames` | Stream sweep step | `RT_APP_STREAM_STEP_FRAMES` | 10 | requires stream | wired-now |
| `graphics.stream_row` | Stream start row | `RT_APP_STREAM_ROW` | 1 | requires stream | wired-now |
| `graphics.stream_pos` | Position-driven demand | `RT_APP_STREAM_POS` | off | requires stream | wired-now |
| `graphics.nav` | Nav overlay | `RT_APP_NAV` | off | always | wired-now |
| `graphics.nav_town` | Town street graph | `RT_APP_NAV_TOWN` | off | requires nav | wired-now |
| `graphics.route` | Route follower | `RT_APP_ROUTE` | off | always | wired-now |
| `graphics.route_town` | Route on town graph | `RT_APP_ROUTE_TOWN` | off | requires route | wired-now |
| `graphics.carmodel` | Car model (Q index) | `RT_APP_CARMODEL` | 0 | always | wired-now |
| `graphics.carmodel_dump` | Car livery dump path | `RT_APP_CARMODEL_DUMP` | (empty) | always | wired-now |
| `graphics.hudtext` | HUD text (FONTE) | `RT_APP_HUDTEXT` | off | always | wired-now |
| `graphics.scorehud` | Score/Meter page | `RT_APP_SCOREHUD` | `off` | always | wired-now |

### Controllers

| id | label | maps-to env/flag | default | availability | status |
|---|---|---|---|---|---|
| `controllers.triggers` | Analog trigger drive | `RT_APP_TRIGGERS` | off | **requires gamepad** | gpu-wave-pending |
| `controllers.rumble` | Rumble | — | on | **requires gamepad** | **gpu-wave-pending** |
| `controllers.rumble_strength` | Rumble strength | — | 1.0 | **requires gamepad** | **gpu-wave-pending** |
| `controllers.deadzone` | Stick deadzone | — | 0.2 | **requires gamepad** | **gpu-wave-pending** |

`controllers.deadzone` mirrors `gameplay/input.h` `kStickDeadzone = 0.2f`
(`input.h:15`); wiring the stored value into `playerCarInput` is GPU-wave glue.

### Sound

| id | label | maps-to env/flag | default | availability | status |
|---|---|---|---|---|---|
| `sound.audio` | Audio output | `RT_APP_NOAUDIO` (inverted) | on | always | wired-now |
| `sound.bgm` | BGM track / file | `RT_APP_BGM` | (empty) | requires audio | wired-now |
| `sound.bgm_tsq` | Use TSQ BGM (else VAG) | `RT_APP_BGM_TSQ` | on | requires audio | wired-now |
| `sound.radio_station` | Radio station | — | `off` | **requires Adventure** | gpu-wave-pending |

`sound.radio_station` is grounded in the PROVEN save byte `+0x12DD`
(0 off / 1 Peach FM / 2 E-Radio, `docs/native-reference/save-layout.md:68`;
`src/native/gameplay/save.h:65` `kRadioStation`). The host BGM path
(`app/main.cpp:125-159`) does not consume the station yet — the row is honest
about that, and it appears only in an Adventure (the sibling rule, `CLAUDE.md:80`).

### Save

| id | label | maps-to | default | availability | status |
|---|---|---|---|---|---|
| `save.slot` | Save slot | (slot value) | 0 | **requires save state** | wired-now |
| `save.save_state` | Save state | `GameSession::save` | — | **requires save state** | wired-now |
| `save.load_state` | Load state | `GameSession::load` | — | **requires save state** | wired-now |

**Catalog size: 40 rows** (Display 14, Graphics 15, Controllers 4, Sound 4,
Save 3). **Env-flag coverage: all 27 enumerated flags** in §1 (26 `RT_APP_*` plus
`RT_DISC`) are mapped by at least one row, asserted by
`testCatalogMapsEveryEnumeratedEnvFlag`.

---

## 3. Persistence contract

* **Location:** `<data dir>/settings.toml`. The data directory is *injected* by
  the caller (`defaultSettingsPath(dataDirectory)`); nothing is hardcoded. The
  native app's data dir is `~/Library/Application Support/RoadTripRecomp` (the
  `RT_DISC` default mirrors it, `app/main.cpp:131-135`); tests inject a temp dir.
* **Format (flat key=value text, TOML-style file name):**

  ```toml
  # Road Trip native options (AUTHORED-for-native; see docs/native-reference/options-menu.md)
  graphics.terrain = on
  graphics.stream  = off
  sound.radio_station = peach_fm
  ```

  * Blank lines and `#` comments are preserved verbatim.
  * Each entry is `key = value`: split at the **first** `=`, both sides trimmed.
  * A line with **no `=`** or an **empty key** is corrupt: skipped for reads
    (`skippedLines()` counts them) but **preserved on save** so a re-save never
    destroys a hand-edited file.
  * Duplicate keys: the **first** wins for reads; `setRaw` updates the first.
  * **Unknown keys round-trip byte-for-byte** in order (a newer build's option, a
    hand edit).
* **Typed access:** `getBool/getInt/getFloat/getString/getEnum`, each with a
  default, validation and clamping. An unparseable value falls back to the
  (clamped) default — a corrupt value never silently becomes "on". Enum tokens
  are matched case-insensitively on input and returned canonical; an invalid
  token **keeps the current value** (`OptionsModel::setValue` returns false).
* **One writer at a time:** a `SettingsStore` owns one file and is **not**
  thread-safe. One owner (the app's main/menu thread) performs every mutation and
  save; concurrent access must be serialised by the caller. `save()` is atomic
  (temp file + `rename`) so a reader never sees a partial file.

---

## 4. Pause integration and renderer glue

**Interim toggle:** `RT_MENU=1` opens the pause menu (the `display.menu` row
stands in for it). The real wiring replaces the env check with the sibling's
button chord — Esc / Start (and Back+Start held, per `CLAUDE.md:87`) — and calls
`MenuNavigator::open()`. Everything below is renderer-agnostic: the menu is drawn
from `OptionsModel` + `MenuNavigator` state each frame.

**Gamepad-only navigation contract** (`MenuNavigator`, `settings.cpp`):

| Input | Action |
|---|---|
| up / down | move focus: skip unavailable rows, **wrap per page**; at the page list, wrap pages |
| left / right | `adjust(direction)`: move an Enum through its domain (wrap), a Toggle flips, an Integer/Float steps (clamped) |
| confirm | at the page list: `enterPage`; on a row: toggle / enum-advance / fire an Action row (`lastAction()`) |
| cancel | `back()`: page → page list → `close()` |

No pointer or mouse is required at any step. Initial state is deterministic:
`open()` puts focus on the first available page; `enterPage` lands on the row the
page was left on, clamped to the first available row when that row has since
been hidden.

**Renderer glue (pseudocode, once per frame while the menu is up):**

```cpp
// Setup (once): the data dir is injected; one writer owns the store.
SettingsStore store{defaultSettingsPath(dataDir)};
store.load();                       // absent file -> all defaults
OptionsModel model;
MenuContext ctx;                    // facts the app knows:
ctx.gamepadPresent       = padConnected();      // triggers/rumble rows
ctx.secondScreenSupported = presentationDisplayExists();
ctx.adventure            = session.state() == SessionState::EXPLORE;
ctx.race                 = session.state() == SessionState::RACE;
ctx.playerRace           = true;                 // set false for an AI race
ctx.moviePlaying         = false;                // set true in the attract demo
ctx.gsCanSaveStates      = true;                 // renderer capability
MenuNavigator nav(model, store, ctx);
// MenuContext flags (terrain/stream/nav/route/interp/audio) are refreshed from
// the store inside nav.effectiveContext() every call — no manual sync.

// Per frame:
if (menuButtonPressed()) nav.open();  // or RT_MENU for now
if (nav.isOpen()) {
    const MenuInput in = pollGamepad();       // reuse gameplay::MenuInput
    if (in.up)    nav.moveFocus(-1);
    if (in.down)  nav.moveFocus(1);
    if (in.left)  nav.adjust(-1);
    if (in.right) nav.adjust(1);
    if (in.confirm) nav.confirm();
    if (in.cancel)  nav.back();

    // The renderer draws nav.visibleRows() and highlights nav.focusedRow().
    const auto rows = nav.visibleRows();          // available rows only
    const auto focus = nav.focusedRow();          // id/label/value/type/status/domain
    drawMenu(nav.inPageList() ? pageList(model, nav) : rows, focus,
             nav.page(), focus.status == OptionStatus::GpuWavePending /* dim/preview */);

    if (const std::string& action = nav.lastAction(); !action.empty()) {
        if (action == "save.save_state")
            session.save(card, store.getInt("save.slot", 0, 0, 1));  // slot rows §5
        else if (action == "save.load_state")
            session.load(card, store.getInt("save.slot", 0, 0, 1));
    }
}
if (nav.isOpen() && !wasOpen) store.save();   // persist on close (one writer)
```

`SettingsStore::load`/`save` return `false` (never throw) on an absent/unwritable
file, so the glue can degrade to defaults silently. The env-var bridge for the
GPU wave is a one-line read per flag (`getenv` falls back to the stored value);
until then the store is the single source after `RT_MENU` opens the menu.

**Save-state rows contract** (`save.save_state` / `save.load_state`): the slot
value is `save.slot` ∈ {0,1}, the native host slot of
`gameplay/memcard.h` `MemoryCard` (`<data dir>/slot<N>.sav`, `memcard.cpp:28-30`).
The actions call `GameSession::save(MemoryCard&, int slot)` /
`GameSession::load(const MemoryCard&, int slot)` (`gameplay/session.h:31-32`;
`session.cpp:67-73`), which wrap `MemoryCard::saveSlot` / `loadSlot`. Availability
is the ported sibling rule (`CLAUDE.md:90`): shown only in an Adventure or a race
**of the player's**, never in a movie, and only when the renderer can snapshot —
see `OptionAvailability::RequiresSaveState` in `settings.h`.

---

## 5. Tests

`tests/native_assets/test_settings.cpp` (deterministic; no game data):

* store round-trip incl. **unknown-key preservation** and comment/blank survival;
* **corrupt-line skip** with `skippedLines()`, valid keys kept, corrupt lines
  preserved on save;
* typed clamps (`int`/`float`) and fallbacks for unparseable values;
* `getEnum` case-insensitivity and fallback;
* catalog: every enumerated env flag is mapped; ids are unique;
* availability predicates: **gamepad-absent hides the trigger/rumble rows**,
  second-screen hidden without a display, radio Adventure-only, save-state rule
  (Adventure / player race / no movie / GS can save);
* navigator: deterministic initial state, page-list wrap, page entry, toggle,
  back/close, **skip-unavailable + adjust-domain** sequences, enum rejects an
  invalid token and keeps the current value, and a scripted input sequence
  reproduces the identical focus path on a fresh navigator (determinism).

Build (own build dir):

```
cmake -S src/native -B build/opts -DRT_NATIVE_ASSETS_BUILD_TESTS=ON
cmake --build build/opts --target native_assets_settings_test
./build/opts/native_assets_settings_test      # -> "settings: all checks passed"
```
