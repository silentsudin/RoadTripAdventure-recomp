# Wave 3 — Visual depth + gameplay wiring (goal: GAME-LOOK 75 -> 90)

Ground rules (identical to wave 2, `.qoder/wave-board.md` §Ground rules):
- No ROM, no runtime-extracted art in git. Docs cite ELF vaddrs.
- `cmake -S src/native -B build/w-W3-<n> -DCMAKE_BUILD_TYPE=Release -DRT_NATIVE_ASSETS_BUILD_TESTS=ON`;
  test target `w3_<stem>_test`; SCORE line; NO_ELF => exit 0.
- Do NOT run `git add`, `git commit`, `git checkout`, `git restore`, `git stash`,
  `git clean`, or any destructive git command. Read-only git only.
- Only touch your owned files. `git status` first; foreign uncommitted edits exist
  in src/native/app/main.cpp — leave them.
- Headless smoke if you touch app-facing code:
  `RT_APP_HEADLESS=1 RT_APP_NO_AUDIO=1 RT_APP_STATE=town RT_APP_MODE=explore RT_APP_FRAMES=60 RT_APP_SNAPSHOT=/tmp/w3n.json RT_ELF="$HOME/Library/Application Support/RoadTripRecomp/disc/SLUS_203.98" RT_PAK=/tmp/w3n.pak ./build/native/rt-app`
  (build into YOUR build dir's rt-app if build/native is locked).
- Verify against ROM with python struct at offset = vaddr - 0x1FF000,
  ROM = "$HOME/Library/Application Support/RoadTripRecomp/disc/SLUS_203.98" (1,280,576 B).

## W3-01 — Proportional text: lowercase glyph cells + advance table
File: `src/native/preview/text_metrics.h` (new).
Problem: the text renderer maps every char to METER10/FONTE uppercase cells;
lowercase renders as uppercase, spacing is fixed.
ROM ground truth: advance table `DAT_0029a880` at vaddr 0x29A880 (256 x u8?
verify element size/type by reading and sanity-checking plausible widths 4..24);
character->cell mapping in the FONTE VAB block (lbn 339/340/341).
Deliverables:
1. `struct Glyph {float advance; int cellX, cellY, w, h;};` + `GlyphTable`
   built at runtime from the advance table when ROM available, else from an
   authored fallback table measured from the FONTE atlas layout (document
   derivation; fallback constant tables are OK to commit — they are numeric).
2. `measure(text, pxPerUnit)` + `layoutRun(text)` returning per-char advance.
3. Upper/lowercase cell selection per ROM mapping (verify in ELF which handler
   walks DAT_0029a880: functions.tsv `Find functions using DAT_0029a880`).
Test: `tests/native_w2/text_metrics_test.cpp` — ROM pins (advance bytes for
'A','a',' ', '.', digits), monotonic sanity, width vs sum-of-advances,
determinism.

## W3-02 — Race-circuit minimap polyline + yawed car arrow
Files: `src/native/gameplay/minimap_track.h` (new), test
`tests/native_w2/minimap_track_test.cpp`.
W2-11 (in `src/native/gameplay/minimap.h/.cpp`) proved the projection law
(race=90, y-sign -0.47 vs town -0.8) but native draws no track polyline.
Deliverables:
1. Per-track polyline: derive circuit path from `NavRouteTables::trackGraph(n)`
   (src/native/assets/nav_routes.{h,cpp}: 12 graphs; race track n used at
   runtime — find via ELF who indexes the graphs with race id, functions.tsv;
   if indeterminate, map tracks 0..11 and document). Convert node positions
   (world units, same coord frame as physics 0x90 block) into minimap map-space
   via the W2-11 law (s=1, centered on player).
2. `PolylineResult buildTrackPolyline(graph, px, pz, zoomLevel)` -> screen-space
   segments clipped to the scissor (W2-11 scissor table).
3. Car arrow: position from law, heading from physics yaw; verify oracle arrow
   scale 0.62 vs naive (wave-board W2-11 row) — fold the 0.62 empirical scale.
Test: polyline continuity (consecutive node distance bounds), clip determinism,
zoom monotonicity (1020/765/612/459/306), arrow screen pos vs car (102,183).

## W3-03 — Collision against nav occupancy grid
Files: `src/native/physics/car_physics.cpp` (`resolveCollision` stub line ~274)
+ `src/native/physics/collision_grid.h` (new).
`NavRouteTables::townGraph(1)` has 84 road segments; off-road = slowdown
already exists (roadAffinity). Missing: buildings/trees actually block.
Deliverables:
1. Build a coarse occupancy grid (e.g. 2m cells over town bounds) from road
   segments + a hard-boundary box; mark blocked = outside road corridor AND
   inside town bounds.
2. `resolveCollision(pos, yaw)` -> push car back to last free cell along
   travel, damp velocity (restitution ~0.3), return contact flag.
3. Wire the stub to call it when `townGraph` loaded (keep it header-light;
   do not break determinism: grid built once, integer/half-float math stable).
Test: `tests/native_w2/collision_grid_test.cpp` — car starting on road node
never penetrates blocked cells across randomized drives; deterministic replay
hash; frame-rate independence (fixed 8 substeps).

## W3-04 — Save/load file slots wired to game state
Files: `src/native/gameplay/save_slots.{h,cpp}` (new; W2-08 module in
`save.{h,cpp}` + `save_v2` fixtures).
Deliverables:
1. `SlotStore` mapping 2 host slots -> files under app-support dir
   (`~/Library/Application Support/RoadTripNative/slot<N>.sav`),
   v2 container from W2-08; `save(state)`, `load(n, state)`, `list()`.
2. Field coverage: money, owned cars (car_flags from W2-06 catalog), mileage,
   best laps (race_rules LapState), radio station, options (RT_ENHANCE bits).
3. `RT_SAVE_DIR` env override for tests.
Test: round-trip 200 randomized states (money/car_flags/lap records), corrupt
byte => recover or reject deterministically, empty-slot sentinel 0xFF semantics
(W2-08 open item: race-results default 0xFFx78 vs save.h 35 — resolve against
ROM 0xFF0 block read if ELF available).

## W3-05 — Script VM -> gameplay event wiring (countdown, radio, camera)
Files: `src/native/gameplay/event_bridge.{h,cpp}` (new; W2-07 ScriptVM in
`text_script.{h,cpp}`? locate via grep ScriptVM/srcs).
Deliverables:
1. Map script trigger opcodes (0x8A radio, 0x8B cutscene, 0x80-0x84 rumble —
   W2-09/audio-map.md) to GameLoop callbacks: `onRadio(station)`,
   `onCountdown(start)`, `onCameraPreset(k)` (camera presets exist at 0x29CE00
   4x vec3 — W2-04 found them).
2. Countdown ElementA: race-intro 3-2-1-GO element — author timing law from
   ELF 0x21CE48 gate region (race-entry handler; functions.tsv + capstone with
   the word-swap recipe in wave-board Lessons).
Test: `tests/native_w2/event_bridge_test.cpp` — scripted byte streams produce
expected callback sequences (oracle vectors from W2-07 harness outputs in
docs/native-reference/evidence/w2/ if present, else authored from ROM decode).

## W3-06 — Enhancement registry wiring
Files: `src/native/app/enhancements_app.{h,cpp}` (new; registry in
`src/native/enhancements/*.h` exists unwired).
Deliverables: RT_ENHANCE_* env -> registry singleton; expose to main.cpp
(ssaa, target_hz, log_fps, fps_display, physics_hz); `describeAll()` string
for the log line; defaults preserve current behavior exactly (golden: current
headless output unchanged when no env set).
Test: defaults-on == legacy constants; each toggle flips exactly its knob.

## Integration order for coordinator
W3-01 -> W3-06 -> W3-04 -> W3-02 -> W3-03 -> W3-05 (riskiest last).
