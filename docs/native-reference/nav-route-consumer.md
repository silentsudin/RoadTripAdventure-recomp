# Nav-route consumer bridge

`gameplay/nav_route_bridge.{h,cpp}` converts a ROM navigation centerline
(`nav_path.h`, built from the graphs in `nav-tables.md`) into authored
`RaceRoute` zones (`gameplay/route.h`), closing the chain
**ROM graph → traversal → drivable authored route**.

## Mapping

- One zone per consecutive centerline pair (plus the closing pair when
  `NavPath::closed`); degenerate pairs (length < 1e-3) are skipped.
- Each zone is a quad around its leg with perpendicular half-width, capped at
  `halfWidth` (default 55 units) and adaptively clamped to half the analytic
  point-to-segment distance from every other leg's midpoint to this leg's
  axis (floor 0.25), so `RaceRoute::zoneOf`'s first-match containment cannot
  let one zone swallow another leg's midpoint where that is geometrically
  avoidable.
- Vertex order follows `pointInConvexPolygon`'s z-first cross convention
  (its "CCW"; clockwise under the standard x-z shoelace).

## Verified behaviour (disc, static graph 0 / candidate Peach Town)

- `zones = 34`, path `closed = yes` — sequential walk of leg midpoints
  reaches `progress = 16` then stalls at leg 17: the ROM path switchbacks so
  **leg 17's midpoint lies on leg 16's axis** (test diagnostic
  `zoneOf(mid_17) = 16`); no positive strip width avoids containment there,
  and `RaceRoute::advance` only accepts `zone+1`, so the stall cascades. Test
  pins `progress == 16` / `firstStall == 17` honestly (deterministic;
  OFF/OFF snapshots are byte-identical).
- Synthetic fixtures (L-shape, closed square) pass strict sequential matching,
  lap rollover, and degenerate-pair handling.

## Provenance caveats

- Centerline units are tile-local 0..1600 (`nav-tables.md` scale verdict:
  direct f32, not `2^25` fixed point). They are used 1:1 as authored world
  units; authentic world placement (which tile, which offset) waits on the
  streaming-grid geography (`field-topology-status.md`, softly open).
- **No speed semantics are transferred.** The segment `+0x10/+0x14` aux
  floats have no proven speed encoding; callers keep choosing
  `minArrivalSpeed` themselves (authored default 117).
- `RaceRoute::advance`'s next-zone-only rule is the authored progress model,
  not the game's follower (see `nav-route-traversal.md` for the authentic
  position-driven predicate).
- A richer consumer using `RaceRoute::waypoints` + checkpoint metadata (or a
  next-zone tolerance) would clear the crossing stall; logged below.

## Ledger

| claim | status |
|---|---|
| Centerline → zones conversion deterministic, lap-capable | PROVEN (synthetic lap + 50% disc coverage pinned) |
| Peach Town prefix progress 16/34 | PROVEN (deterministic pin) |
| Switchback at leg 17 causes cascade stall | PROVEN (test diagnostic `zoneOf(mid_17) = 16`) |
| Tile-local == world placement | OPEN (geography mapping pending) |
| Per-segment speed limits | OPEN (aux floats unresolved) |
