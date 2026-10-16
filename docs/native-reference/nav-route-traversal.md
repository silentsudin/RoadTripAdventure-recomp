# Navigation route traversal

`NavGraph` retains the ROM transition graph exactly.  This note describes the
host-side deterministic centerline ordering in `gameplay/nav_path.{h,cpp}` and
the separate, position-driven follower observed in `FUN_00250068`.

## Game follower

Let `cross(S, P)` for segment `S = (ax, az) -> (bx, bz)` and vehicle position
`P = (x, z)` be:

```
(bz - az) * (x - bx) - (bx - ax) * (z - bz)
```

* **PROVEN:** If `cross(segmentA, P) < 0`, the follower enters the `+2/+3`
  candidate branch; it repeatedly selects a next transition while the new
  transition's `segmentA` still has a negative cross value.  Otherwise it
  checks `segmentB`; while `cross(segmentB, P) >= 0`, it follows `+4/+5`.
* **PROVEN:** A pair with equal bytes chooses that byte.  For unequal bytes,
  the choice is byte 0 when
  `(segmentB.auxB - segmentA.auxB) * (x - segmentB.auxA) -
  (segmentB.auxA - segmentA.auxA) * (z - segmentB.auxB) < 0`; otherwise it
  chooses byte 1.  This is the decompiler's exact auxiliary-float side test.
* **PROVEN:** Each selected transition byte is written to AI state `+0x24a`;
  after the loop its `+6` byte (`followerOutput`) is copied to `+0x24b`.
  The routine later returns one of the settled node's `+4/+5` bytes.  Thus
  `followerOutput` is an emitted state value, not itself the transition index
  used to advance the follower loop.
* **INFERRED:** `+2/+3` and `+4/+5` are side-A and side-B names respectively;
  the exact world-space labels are intentionally not called forward/backward.

## Host centerline rule

`buildNavPath` is deliberately not a simulation of that position-driven loop.
It uses `SideA` by default (or `SideB` explicitly), scores that pair's valid
candidate transitions by the dot product of successive segment-midpoint
headings, and chooses the largest score.  Ties preserve byte order.  `SideB`
reverses the pair heading so the two conventions remain oppositely oriented.

The result appends the midpoint of each referenced segment once, records each
transition at most once, and permits one final return to the initial transition
as a closure.  It stops rather than indexing a dangling candidate byte or
revisiting any other node.  This makes the output deterministic and useful for
rendering/playable host paths without claiming that it duplicates vehicle AI.

## Open

* **OPEN:** The semantic role of the two auxiliary floats remains unknown.
* **OPEN:** The game makes its branch decision from vehicle position; the
  straight-ahead dot-product ordering is a host reconstruction policy, not a
  proven game selection policy.
* **OPEN:** A graph has no proven start/finish metadata; transition zero is the
  observed initialization value, so it is only the default host start.
