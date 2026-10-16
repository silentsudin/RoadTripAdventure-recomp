# Navigation centerline drive follower

`gameplay/nav_drive.{h,cpp}` is a deterministic, geometry-only kinematic
follower over a `NavPath` centerline. It keeps a single arc-length cursor that
a host advances by a fixed number of centerline units per frame, producing a
position and heading for each step, wrapping on a closed lap and counting laps.
It performs no car dynamics, no collision, and no branch selection, and it
embeds no game data.

## Relationship to `nav_walkable`

`nav_walkable` is a diagnostic: one call materializes every fixed-step cursor
along the whole centerline and reports length/coverage. `nav_drive` is the
per-frame counterpart: state is a single cursor, and each `step()` returns the
pose at the new cursor. Both share the same geometry conventions — zero-length
edges are ignored, and a closed path includes the closing edge back to
`centerline.front()`.

## API

| symbol | meaning |
|---|---|
| `NavPose` | `{ Pt position; float heading; float arcLength; int lap; }` |
| `NavDrive(const NavPath&)` | Builds the segment table once from the centerline. |
| `NavDrive::step(float speedUnitsPerFrame)` | Advances the cursor and returns the new pose. Non-finite or non-positive speed is a deterministic no-op. |
| `NavDrive::pose()` | Pose at the current cursor without advancing. |
| `NavDrive::reset()` | Cursor back to the polyline start, laps to zero. |
| `NavDrive::segmentCount()` / `totalLength()` / `closed()` / `empty()` | Segment-table introspection. |

`heading` is radians with **0 = +Z** (`atan2(dx, dz)`), matching the authored
render convention in `preview/preview.cpp` (`sin`/`cos` heading basis). On a
closed path the cursor wraps past the total length and `lap` increments once
per full traversal; on an open path the cursor clamps at the final point.

## Host wiring (`rt-app`, `src/native/app/main.cpp`)

`RT_APP_ROUTE=1` opens the user's `SLUS_203.98`, builds static graph 0, runs it
through `buildNavPath`, and constructs a `NavDrive`. At start it logs:

```
rt-app route: nav lap drive segments=N length=L
```

Each authored `PLAYING` frame advances the follower once and logs every wrap:

```
rt-app route: lap K complete
```

The follower drives a **render-local copy** of the frame snapshot: session
authoring (race state, lap, collectibles, camera matrices) still reads the
authored snapshot, so only the car mesh's visible position and heading change.
With `RT_APP_ROUTE` unset, no route code runs and stdout gains no new lines.
When the ELF or graph is unavailable the flag logs a `SKIP` line and leaves the
authored car untouched.

## Disc evidence

With the user's `SLUS_203.98`, static graph 0 (candidate Peach Town) yields a
closed centerline with **34 nonzero segments** and pinned length **1652.558**
tile-local units — matching `nav_walkable`'s independently computed length. A
10-unit cursor walked `ceil(1652.558/10) = 166` steps to complete exactly one
lap, wrapping to within one step of the start.

## Ledger

| claim | status |
|---|---|
| `NavDrive` advances a straight segment by exactly `speed` units | PROVEN (unit test) |
| Closed square lap has perimeter 12 and pinned corner-midpoint headings | PROVEN (unit test) |
| Cursor wraps and increments `lap` once per closed traversal | PROVEN (unit test + disc) |
| Peach Town centerline drives 34 segments / 1652.558 units per lap | PROVEN (disc test) |
| The demo car's render-visible pose follows the authentic centerline | DEMONSTRATED (RT_APP_ROUTE smoke: lap 1/2 complete, exit 0) |
| The follower matches the game's position-driven vehicle dynamics | OPEN — car physics/collision are outside this kinematic follower |
