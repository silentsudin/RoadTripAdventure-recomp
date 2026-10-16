# Map view — enhanced map / second-screen logic for the native rebuild

Updated 2026-10-16. This document is the grounding ledger for
`src/native/gameplay/map_view.{h,cpp}` and its contract test
`tests/native_assets/test_map_view.cpp`. It records addresses, formulas, and
probed *metadata* only; no disc bytes ship in the repo. The module is pure
C++ (no Vulkan/SDL/GS/disc); the GPU second-window wave consumes its
draw-list/projection contract. The second-screen UI itself is a later wave.

## What the module owns

- **Map space**: the stored fixed-point torus (`world-geography.md`, PROVEN
  scale `S = 20971.51953125` = `DAT_00332c0c`, tile span `0x02000000`, wrap
  period `0x10000000`) with the odd-row half-column shear removed:
  `mapX = wrap(X - ((Z & 0x02000000) >> 1))`, `mapZ = wrap(Z)`. The shear is
  exactly what `FUN_0021ba70` *adds* for render-local rebase, so stored odd-row
  positions carry it and map space subtracts it again — world↔map is lossless
  (worked check: Peach Town tile 43 raw origin `(0x0b000000, 0x0a000000)` →
  map `(5<<25, 5<<25)`).
- **One camera, one frame**: `MapCamera` (north-up orthographic, `[−1,1]²`
  with `+v` north) projects tiles, roads, route, markers, and the player
  through the same object per frame — the structural fix for the sibling
  app's box-drift bug class. Seam-crossing polylines project as one untorn
  run (shortest torus displacement).
- **Routes**: Dijkstra over shared-segment adjacency of the existing
  `assets::NavGraph` transitions (`gameplay/nav_path.h` centerline
  convention: node = average of the two joined segments' midpoints), with
  deterministic tie-breaks; arc-length `progressAt` clamps to `[0,1]`.
- **Markers**: save-filtered town markers (below).
- **Pages**: deterministic second-screen page state machine (below).
- **Draw list**: one ordered, camera-independent content list per map frame
  (64-row-major tile cells, roads in segment order, ≤1 route overlay,
  filtered markers, player), projected in one pass.

## Disc-graph route coverage: **PROVEN (from the user's own disc extract)**

`testDiscGraphPins` runs against the extracted `SLUS_203.98`
(`RT_DISC`, skipped when unavailable) and passed on 2026-10-16:

- Static directory pair 0 (pack-type-indexed, `scene-tile-map.md`) = the
  `COURSE/C00` race route: 35 segment records, inside one tile.
- Peach Town street graph (outer dir `0x2d98a8`, T=1): 84 segments, tile-local
  bounds `[418, 552] × [443, 1084]` game units (inside the `0..1600` tile),
  matching the scene-tile-map tables.
- Every ordered transition pair on the Peach graph builds a valid route:
  **7056/7056** (84×84, so the street graph is fully connected under
  shared-segment adjacency), each with monotone cumulative lengths and
  `progressAt` clamped to `[0,1]`. This is computed graph metadata, not
  shipped data.

## Doors/residents on the enhanced map — grounding verdict

Evidence: probe of the committed table addresses in the user's own
`SLUS_203.98` on 2026-10-16 (values below are table *metadata* — counts,
indices, identity strings — not shipped bytes).

- **PROVEN — door table shape and content** (`config [doors]`, CONFIRMED):
  `0x2C0698` holds one pointer per location (24 locations); each list is 32-byte
  quads (four `(x, z)` f32 corners) ending where the next location's list
  starts. Peach Town (location 1) has **28 door quads**, matching `byte6 = 28`
  of its `0x2BE438` scene record (`scene-tile-map.md`), and the quad
  centroids sit inside the tile's `0..1600` game-unit span.
- **PROVEN — door ↔ resident-name association** (`config [houses]`,
  `residents = 0x2C22C8`): one pointer per location to 16-byte entries with
  the name pointer at `+0xC`, one per door then one per driver-about-town
  (driver count = `byte7` of the `0x2BE438` record; Peach Town: 11). Peach
  Town's 28 door residents read, in door order: `Q's Factory Staff` (door 0),
  `Parts Shop Staff` (1), `Body Shop Staff` (2), `Paint Shop Staff` (3),
  named residents (4–16), `Entrance to the cave` (17), and ten
  `Quick-Pic Shop Staff` (doors 18–27).
- **PROVEN — save-progress filters** (`save-layout.md` / `config`):
  unvisited-house masks are 10 × u32 at save `0xBD8`, bit n = door n not yet
  visited (new-game masks at `0x2277B0`, e.g. Peach Town `0x3DFFF`: 17 of its
  28 doors start as houses-to-visit, door 13 excluded); Quick-Pic photos are
  the 16-byte bitmap at save `0x508`, photo n = bit n−1 (set by `0x23C5C0`).
- **INFERRED / AUTHORED — the P/B/C letter map**: that door 1/2/3 means
  Parts/Body/Paint is read from the resident strings above, but the *letter*
  assignment (`ShopP/ShopB/ShopC`) is a host convention, not game data.
- **UNVALIDATED — Quick-Pic door ↔ photo number n**: `config [photos]`
  describes booths as doors "whose resident is Quick-Pic Shop No.n", but the
  probed resident strings are generic (`Quick-Pic Shop Staff` ×10) — the
  photo number arrives via dialogue script op `0x11 n` / setter `0x23C5C0`,
  per the config's own note. The door-ordinal ↔ photo-number binding is
  therefore documented configuration language, **not yet ELF-verified**, and
  the module takes that ordinal from the caller rather than inventing it.
- **AUTHORED — marker set and filtering**: "Q's Factory, the shops, Quick-Pic
  Shops not yet used, houses not yet visited" (plus town exits and caller
  race gates) ports the sibling second-screen design; the in-game
  minimap itself draws only its own untextured strips + bezel sprites
  (`config [map]` GS template `0x3380B`, scissor `[45,156,158,207]`), so no
  retail ELF behaviour pins an *enhanced* marker set. The legacy town-minimap
  projection formula (`fx = 102 + (x−px)·s/200`, `fy = 183 − (z−pz)·0.47·s/200`,
  zoom table `0x2A42C0`) is CONFIRMED against the game's Quick-Pic icons to
  ≤1.5 px and is kept in the module for `RT_APP_MAP_ENHANCED` parity.

## Second-screen page machine: **AUTHORED (sibling port, no ELF evidence)**

The page rules (`map_view.h`): racing/map-up shows Map immediately; untouched
cruise auto-selects Notebook once after `notebookIdleSteps` vblanks (default
45, the sibling's 45-vblank cadence — the transition tick into cruise is not
itself an idle step); a player tab choice is sticky until the context kind
changes; the main window's map is suppressed only while the second screen
exists and shows Map. The "map is up" GS signal has no native equivalent, so
the native predicate is defined here as *map button held OR navigation
active*. The tab set/layout/visuals (`ui-parity.md` "Second screen" /
"Enhanced map": Factory > Shop > PhotoBooth > House marker priority, overlap
rules, navy/gold palette) belong to the second-screen rendering wave, not to
this logic layer.

## Native status

Logic contract complete and pinned (`native_assets_map_view_test`, all checks
pass incl. the disc pins above). Not yet wired into `rt-app`: no caller feeds
`TownMarkerSource`/`MapContentSource` from `session.cpp`, and no second
window/panel consumes `MapDrawList`/`MapProjectedFrame`. Glue needed by the
second-screen wave: town door/resident extraction from the runtime ELF
session (caller's classification), player position/heading feed, map-button
and navigation-active inputs, and the window/panel that renders the draw list
through `MapCamera::normalizedToPixels`.
