# Navigation waypoint walkability

`gameplay/nav_walkable.{h,cpp}` supplies a deterministic, geometry-only walk
over the `NavPath` centerline.  It advances a cursor at fixed arc-length
intervals, includes the closing edge for a closed path, and finishes exactly
at the final point (the first point after one closed lap).  It neither creates
polygons nor selects graph branches.

## Why this complements zone containment

`nav_route_bridge` makes wide authored containment zones for `RaceRoute`. That
is useful for car-progress gameplay, but it has an unavoidable ambiguity where
the Peach Town switchback's leg-17 midpoint lies on leg 16's axis: first-match
zone containment pins its sequential progress at 16 of 34.

The waypoint walk uses the ordered ROM-derived centerline directly. It has no
overlapping containment region to choose between, so a cursor can traverse all
arc length and return to its start after a closed lap. This **does not replace**
zone containment: it is evidence that the geometry itself is continuously
walkable, not a replacement for authored car arrival/progress rules.

`nodeCoverage` is likewise diagnostic only. It reports the fraction of unique
transition nodes in `NavPath::nodeOrder` whose midpoint was within a requested
tolerance of a sampled cursor position; it cannot gate a route or invent
checkpoint behavior.

## Disc evidence

With the user's `SLUS_203.98`, static graph 0 (candidate Peach Town) builds a
closed `NavPath` with pinned length **1652.558** tile-local units. A 10-unit
cursor step visits 100% of that length, returns within `1e-3` of its start, and
reports node coverage 34/34. The independent containment bridge remains
pinned at 16/34 as documented in `nav-route-consumer.md`.

The disc-gated test prints this complete static-graph corpus. All paths walk
100% of their constructed polyline; `closed` says whether the traversal has a
closing edge, not whether an open path failed to walk.

| static graph | closed | length |
|---:|:---:|---:|
| 0 | yes | 1652.558 |
| 1 | yes | 1322.340 |
| 2 | yes | 1887.079 |
| 3 | yes | 3353.322 |
| 4 | yes | 1646.929 |
| 5 | yes | 2669.433 |
| 6 | yes | 2457.862 |
| 7 | no | 1097.924 |
| 8 | yes | 2059.158 |
| 9 | yes | 2062.806 |
| 10 | yes | 2563.850 |
| 11 | yes | 2457.862 |
| 12 | yes | 2310.720 |
| 13 | yes | 1991.091 |
| 14 | yes | 3599.624 |
| 15 | yes | 3841.203 |
| 16 | yes | 3841.203 |
| 17 | yes | 3134.658 |
| 19 | yes | 1500.000 |
| 20 | yes | 2066.572 |
| 22 | yes | 2800.000 |

Closure: 20/21 (95.2%). Graphs 11 and 16 are the documented static-directory
aliases of 6 and 15, respectively.

## Ledger

| claim | status |
|---|---|
| Constructed ROM centerlines can be walked end-to-end without containment ambiguity | PROVEN for all 21 populated static entries under host `buildNavPath` ordering |
| Candidate Peach Town centerline completes a closed 1652.558-unit lap | PROVEN |
| Peach Town authored containment progress reaches 34 zones | REJECTED; it remains pinned at 16/34 by the documented switchback overlap |
| ROM geometry alone proves a vehicle can drive every route | OPEN — car dynamics, collision/road surface, and placement are outside this walk |
| The host ordering matches the game's branch choices at every junction | OPEN — `buildNavPath` has an explicit straight-ahead policy |
| A route has a proven junction-choice/checkpoint policy | OPEN |
