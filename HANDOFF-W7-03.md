# HANDOFF — W7-03: town collision grid → car physics

## Deliverables

| path | what |
|---|---|
| `tests/native_w20/collision_grid_test.cpp` | new test (only new source file) |
| `tests/native_w20/CMakeLists.txt` | new, mirrors `tests/native_w16/CMakeLists.txt` style |
| `build/scratch/w7-03/` | probes + built binary + captured output |

No existing file was edited. `src/native/CMakeLists.txt` is untouched (coordinator registers the dir).

## Build & run (narrow file list)

The suggested `find src/native/gameplay -name '*.cpp'` pulls ~100 TUs; the test needs **one**:
`collision_grid.h` is header-only and `NavRouteTables` lives in `src/native/assets/nav_routes.cpp`.

```
mkdir -p build/scratch/w7-03
clang++ -std=c++20 -Wall -Wextra -I src/native \
    src/native/assets/nav_routes.cpp \
    tests/native_w20/collision_grid_test.cpp \
    -o build/scratch/w7-03/collision_test
RT_DISC="$HOME/Library/Application Support/RoadTripRecomp/disc" ./build/scratch/w7-03/collision_test
```

Clean compile, no warnings. The CMakeLists was also configured+build+run standalone (scratch project) — configure 0, build 0.

## Test output (RT_DISC set)

```
evidence outer[1]=0x002d9310 pair={0x002c5ff0,0x002c67d0} raw_segments=84
evidence static[1] pair={0x002aca80,0x002ace88} raw_segments=43
evidence town_segments=84 static1_segments=43
evidence grid=84x84 cellUnits=19.0476 freeCells=95
evidence corridor_cells=43 extent=114.3x266.7
evidence interior_samples=548 blocked=548
evidence rays_tested=24 mostly_free=24 worst_free_fraction=1.000
checks=22 failed=0
```

SKIP path verified by running the binary from a temp dir with `HOME=/nonexistent` and `RT_DISC`/`RT_ELF` unset:

```
SKIP collision_grid: no SLUS_203.98 (set RT_ELF or RT_DISC)
checks=0 failed=0
exit=0
```

## Exact API signatures used

```cpp
// src/native/assets/nav_routes.h
expected<NavRouteTables, NavRouteError> NavRouteTables::open(const std::filesystem::path&);
expected<NavGraph, NavRouteError>       NavRouteTables::townGraph(int sceneId) const;
bool                                    NavRouteTables::staticPair(std::size_t, uint32_t&, uint32_t&) const;
expected<NavGraph, NavRouteError>       NavRouteTables::staticGraph(std::size_t index) const;
std::uint32_t                           NavRouteTables::outerEntry(std::size_t t) const;
// struct NavGraph { uint32_t segmentVaddr, transitionVaddr; vector<NavSegment> segments; vector<NavTransition> transitions; };
// struct NavSegment { float ax, az, bx, bz, auxA, auxB; };
// struct NavTransition { uint8_t segmentA, segmentB, sideACandidates[2], sideBCandidates[2], followerOutput, reserved; };

// src/native/gameplay/collision_grid.h   (namespace rt::native::gameplay::collision)
static OccupancyGrid OccupancyGrid::fromTownGraph(const assets::NavGraph&);  // header-only
std::size_t width() const; std::size_t height() const; float cellUnits() const;
int cellX(Point) const; int cellZ(Point) const; bool blocked(Point) const;
CollisionResult resolve(Point previous, Point desired, Point velocity, float restitution) const;
// struct Point { float x, z; };  struct CollisionResult { Point position, velocity; bool contact; };
```

The test adds no second grid builder — it reuses `OccupancyGrid::fromTownGraph` exactly (the W3-03 grid is the only one in the tree; `grep -rln collision_grid src/ tests/` returns only its header, its CMake entry and the two tests).

## Evidence on the 84-vs-43 discrepancy

**Verdict: the two numbers come from TWO DIFFERENT ROM DIRECTORIES. Both are real. 84 is the town street graph; 43 is the static-directory entry 1, which is not Peach Town.** Neither is a miscount.

Raw structure reads (independent of `NavRouteTables`, `fileOffset = probe reads PT_LOAD`):

| source | location | pair | span / 24 |
|---|---|---|---|
| outer directory @ `0x002d98a8`, entry **1** | `outer[1] = 0x002d9310` (file off `0xda310`) | `{0x002c5ff0, 0x002c67d0}` | `(0x2c67d0 − 0x2c5ff0)/24 = **84**` |
| static directory @ `0x002bd578`, entry **1** | file off `0xada80` | `{0x002aca80, 0x002ace88}` | `(0x2ace88 − 0x2aca80)/24 = **43**` |

Byte proof of the segment record (`struct.unpack('<6f', …)` at `0x002c5ff0`): `(484.0, 486.0, 480.0, 482.0, 0.0, 0.0)` — i.e. `484/486 → 480/482`, the segment-0 record the reader decodes. Spans are exact multiples of 24, so the divisor is proven, not assumed.

Two independent lines agree that 43 belongs to the **static** directory:

1. `docs/native-reference/nav-tables.md:85` tabulates the *static* directory and lists `index 1 = 43`.
2. `docs/native-reference/nav-index-binding.md:87` tabulates the *outer* directory: `T=1 Peach Town dir 0x002d9310 car0 0x002c5ff0/0x002c67d0` — the 84-segment pair.

Distribution census (all populated rows, from the raw directory walk) — no other index yields 43 in the outer directory, and no other yields 84 in the static directory:

* outer: `0→30, 1→84, 3→35, 4→33, 6→14, 7→30, 9→30, 13→33, 18→26, 22→9, 23→86, 24→51, 25→12, 26→39, 27→34, 28→16, 29→78, 30→15, 31→14`
* static: `0→35, 1→43, 2→120, 3→143, 4→66, 5→163, 6→90, 7→128, 8→42, 9→37, 10→143, 11→90, 12→143, 13→224, 14→197, 15→220, 16→220, 17→106, 19→23, 20→86, 22→15`

Footprints differ, so they are not the same corpus under two widths:
town graph bbox `x[418,552] z[443,1084]` (…×641 long corridor), static-1 bbox `x[358,822] z[340,572]`.

**What the python survey most likely did:** read the *static* directory at `0x2bd578 + 8*1` (the "directory idx1" name matches its table row) and reported 43 — correct for that directory, wrong for Peach Town streets. The C++ `townGraph(1)` reads the *outer* directory and gets 84 — correct for Peach Town. The node count equals the segment count in both (`transitions.size() == segments.size()`, proven by the `transitionVaddr − segmentVaddr` span law), so the "43 nodes" figure is static-graph-1's node count, not Peach Town's.

The test pins this: it asserts `townGraph(1).segments.size() == 84`, `staticGraph(1).segments.size() == 43`, and that the reader's town count equals the raw `(trans−seg)/24` — so if anyone "fixes" the reader to 43 the test fails on the raw-structure read, not just on a constant.

## PROPOSED integration patch (proposal only — NOT applied)

Target: `src/native/gameplay/game_loop.cpp`, `GameLoop::integrateCar` (currently lines 119–131).

Two new members in `game_loop.h` (private):
```cpp
gameplay::collision::OccupancyGrid collisionGrid_;
bool collisionReady_ = false;
```
Built **once** where the town graph is loaded (in `reset()`, or when the town scene is entered), never per tick:
```cpp
// Town-only: the graph is Peach Town streets, not the race course.
if (auto tables = assets::NavRouteTables::open(townElfPath); tables) {
    if (auto g = tables->townGraph(1); g) {          // scene 1 = Peach Town
        collisionGrid_ = collision::OccupancyGrid::fromTownGraph(*g);
        collisionReady_ = true;
    }
}
```

`integrateCar` — replace the two position lines (`game_loop.cpp:129-130`) with:
```diff
@@ void GameLoop::integrateCar(const CarInput& input) {
     ai_.heading = wrapAngle(ai_.heading + steer * (sp / params_.speedRef) * params_.turnRate);
-    ai_.position.x += std::sin(ai_.heading) * sp * params_.unitsPerSpeedTick;
-    ai_.position.z += std::cos(ai_.heading) * sp * params_.unitsPerSpeedTick;
+    // World units, tile-local; the nav graph and ai_.position share the frame,
+    // so no unit conversion (WORLD-SCALE RISK in handoff).
+    const collision::Point previous{ai_.position.x, ai_.position.z};
+    const collision::Point desired{previous.x + std::sin(ai_.heading) * sp * params_.unitsPerSpeedTick,
+                                   previous.z + std::cos(ai_.heading) * sp * params_.unitsPerSpeedTick};
+    if (collisionReady_) {
+        const collision::CollisionResult hit =
+            collisionGrid_.resolve(previous, desired, {desired.x - previous.x, desired.z - previous.z}, 0.3f);
+        ai_.position = {hit.position.x, hit.position.z};
+        if (hit.contact) car_.speed = static_cast<std::int32_t>(static_cast<float>(car_.speed) * 0.3f);
+    } else {
+        ai_.position = {desired.x, desired.z};
+    }
 }
```

Call signature the test pins: `grid.resolve(Point{prev}, Point{desired}, Point{delta}, 0.3f)` → `{position, velocity, contact}`, then `blocked(position) == false` every step (the test's corridor/substep checks are the contract).

## Risks

1. **Cell size vs. real road width — the blocker for shipping as-is.** `cellUnits = 1600/84 = 19.05` units, and a free cell is at most **18.9 units** from a route segment (measured). The game's own road half-width is **60** (`game_loop.h:34`, `docs/native-reference/nav-tables.md`-adjacent course tuning). The grid's driveable ribbon is therefore ~⅓ the width of the real road: a car driving normally would hit invisible walls while still on tarmac. Only 95 of 7056 cells (1.3%) are free. Either widen the corridor to the 60-unit half-width or accept the grid as a centreline guide, not a wall. **Do not wire `resolve()` into `integrateCar` before this is decided.**
2. **Resolution is derived from segment count, so it changes per scene.** `cellUnits = 1600 / segments.size()`. Passing outer[0] (30 segs) gives 53.3 units/cell; the 84-seg town gives 19.05. A scene-dependent cell size means collision feel differs town to town, and a race graph would give yet another. The builder should take an explicit cell size (or a target resolution) instead of the segment count.
3. **Grid is town-only; `GameLoop` also drives races.** `tick`/`tickPlayer` run the same `integrateCar` for `raceMode` courses too. Wiring must gate on the town scene, or a race car collides with Peach Town streets.
4. **World-unit scaling assumption.** The grid spans the hardcoded `[0,1600]` tile extent (`collision_grid.h:39-40`) and the graph's tile-local coords; the test shows the spawn pose (506.283, 442.230) lands inside and free, which supports the shared frame for Peach Town. It is **not** proven for a tile placed elsewhere in the world (`1<<25` streaming grid, `docs/native-reference/field-topology-status.md`); moving to a second town would need an origin offset.
5. **Cost is fine.** `fromTownGraph` = **0.9 ms** one-time (913.6 µs mean, 100 runs, `-O2`); grid storage is 84×84 bits = **882 bytes** (`std::vector<bool>`). Build it once at scene load — never per tick (per-tick it would be ~0.9 ms of a 16 ms budget).
6. **Out-of-range reads are safe.** `blocked()` returns true for points outside `[0,1600]` or on an empty grid, so an unbuilt grid blocks every position rather than letting the car escape. The test asserts both.
7. **`resolve()` samples at most `cellUnits` apart** (`ceil(largestAxis/cellUnits)` steps), so a fast tick (sp·unitsPerSpeedTick up to 4 units/tick at kSpeedMax) is well inside one cell and cannot tunnel — verified by the test's 8-substep check landing free.
