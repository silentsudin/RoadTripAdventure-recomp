# HANDOFF — W8-03 collision-grid sizing repair

## Delivered files

| path | purpose |
|---|---|
| `src/native/gameplay/collision_grid.h` | widens free cells to cover the documented road ribbon, while retaining the existing one-argument builder call. |
| `tests/native_w23/CMakeLists.txt` | target definition for `w23_collision_sizing_test`. |
| `tests/native_w23/collision_sizing_test.cpp` | ELF-gated Peach Town ribbon sizing evidence. |

No `src/native/CMakeLists.txt`, car integration, or W20 source was edited.

## Sizing decision and grounding

`OccupancyGrid::fromTownGraph(graph, roadHalfWidth, margin)` remains backward compatible: existing callers still use the one-argument form.  Its default road half-width is 60 units, grounded by the existing drive-tuning evidence in `src/native/gameplay/game_loop.h:33-34`.  The builder now frees a cell whose **centre** is within:

```
roadHalfWidth + optional margin + (sqrt(2) * cellUnits / 2)
```

of any route segment.  The final term is not an arbitrary road widening: it is the furthest a point in a square cell can be from that cell's centre.  Therefore a point at the documented 60-unit road boundary maps to a free cell.  The geometry is the 84-record Peach Town graph from outer-directory entry 1 (`0x002d98a8` → `0x002d9310`, pair `0x002c5ff0/0x002c67d0`), as documented in `docs/native-reference/nav-tables.md` and `nav-index-binding.md`; no ROM payload is committed.

## Test evidence

The new test was intentionally run once against the prior builder before production code changed:

```
FAIL at least 99 percent of dense +/-40 route ribbon samples are free
evidence grid=84x84 cellUnits=19.0476 freeCells=95
evidence ribbon_samples=4284 free=3671 fraction=0.856909 offset=40.0
checks=8 failed=1
```

After the sizing repair, standalone compile/run with `RT_ELF` set:

```
evidence grid=84x84 cellUnits=19.0476 freeCells=375
evidence ribbon_samples=4284 free=4284 fraction=1.000000 offset=40.0
evidence outside_samples=6048 blocked=6048 minimum_distance=200.0
evidence segment_bbox=x[418.0,552.0] z[443.0,1084.0] grid=[0.0,1600.0]^2 buffer=60.0
evidence resolve_route_steps=84 collisions=0
checks=8 failed=0
```

Free cells changed from **95** to **375** (of 7,056).

## Gate blocker: W20 contradicts the accepted 60-unit road width

The required unchanged W20 gate cannot pass together with the repaired 60-unit corridor.  Its `collision_grid_test.cpp:237-257` deliberately selects samples farther than two cells (38.095 units) from every route segment and requires every such sample to be blocked.  That includes points inside the 60-unit road half-width.  On the repaired grid it reports:

```
evidence grid=84x84 cellUnits=19.0476 freeCells=375
FAIL every sample >2 cells from all route segments is BLOCKED
evidence interior_samples=548 blocked=0
checks=22 failed=1
```

This is a semantic conflict, not a build regression: the original 95-cell grid makes the W20 assertion pass precisely by producing the invisible walls this task fixes.  Do not weaken the production sizing to satisfy it.  The coordinator should replace that stale assertion with one that samples **beyond 60 units plus rasterization margin**, citing the 60-unit evidence; that is a test correction to documented ground truth, not threshold loosening.

The requested CMake command also cannot build W23 yet because the coordinator-owned `src/native/CMakeLists.txt` currently registers W20 but not `tests/native_w23`:

```
[100%] Built target w20_collision_grid_test
gmake: *** No rule to make target 'w23_collision_sizing_test'.  Stop.
```

Once the coordinator adds its normal `add_subdirectory` registration and corrects W20's obsolete narrow-corridor assertion, rerun from `build/w-W8-3`:

```
cmake --build build/w-W8-3 --target w20_collision_grid_test w23_collision_sizing_test -j4
RT_ELF="$HOME/Library/Application Support/RoadTripRecomp/disc/SLUS_203.98" \
  build/w-W8-3/native_w20/w20_collision_grid_test
RT_ELF="$HOME/Library/Application Support/RoadTripRecomp/disc/SLUS_203.98" \
  build/w-W8-3/native_w23/w23_collision_sizing_test
```

## Ready-to-apply `integrateCar` wiring (do not apply here)

At the town-scene graph-load site, construct once with `OccupancyGrid::fromTownGraph(*townGraph, OccupancyGrid::kTownRoadHalfWidth)` (zero optional margin).  In `GameLoop::integrateCar`, retain the existing `previous` and calculated `desired` tile-local positions, then only for that ready Peach Town grid call `resolve(previous, desired, {desired.x - previous.x, desired.z - previous.z}, 0.3f)`.  Assign its returned position; on contact apply the existing collision speed policy.  Do not invoke it in race mode or rebuild the grid per tick.  This task intentionally does not land that wiring.
