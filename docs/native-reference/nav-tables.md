# AI navigation tables

This is the executable-resident source of the AI route geometry.  It is
separate from `COURSE/*.BIN`: the COURSE tails are VU1 prop-draw chains (see
[`course-routes.md`](course-routes.md)), whereas this data is read directly by
the EE AI code.

No table payload is checked into this repository.  Addresses, formats, counts,
and a few diagnostic values below are sufficient for a runtime reader to decode
the user's own `SLUS_203.98`.

## Result

`FUN_00250068` is the route follower.  Its inputs are:

* a 24-byte segment array at AI-object `+0x250`; and
* an 8-byte transition array at AI-object `+0x254`.

`FUN_0024fcd8` initializes those two pointers.  There are two selection modes:

1. **Static scene/town mode.**  A ROM directory at virtual address
   `0x002bd578` holds `{segmentPtr, transitionPtr}` pairs.  The selected pair
   is copied directly to `+0x250/+0x254`.
2. **Course/entity mode.**  For scene/type byte `-1` or `0x22`, it calls
   `FUN_0025fbc0`.  That routine first selects a 32-entry outer directory at
   `0x002d98a8` by `T = (taskTag >> 10) & 0x1f`, then returns pair `car` from
   that course's 8-byte-pair array.  This is still ROM data, not a table built
   from a COURSE file at runtime.

Thus the model is **not one global graph** and not a single graph per COURSE
file: it is a catalog of independently addressed graphs.  Static scene entries
and course/entity entries can share a graph pair.

## PROVEN layout

All fields are little-endian; the PS2 executable is loaded as
`r5900:LE:32:default`.

### Segment record — 24 bytes

| offset | type | meaning | status |
|---:|---|---|---|
| `0x00` | `f32` | endpoint A X | PROVEN |
| `0x04` | `f32` | endpoint A Z | PROVEN |
| `0x08` | `f32` | endpoint B X | PROVEN |
| `0x0c` | `f32` | endpoint B Z | PROVEN |
| `0x10` | `f32` | auxiliary value A | PROVEN as float read; semantic role OPEN |
| `0x14` | `f32` | auxiliary value B | PROVEN as float read; semantic role OPEN |

The first four values are directly used for signed 2-D side tests and heading
calculation in `FUN_00250068`; they are endpoints, rather than a point plus
heading/length.  The function also reads `+0x10/+0x14` while resolving an
alternate transition, but that is insufficient to assign a stable meaning.

### Transition record — 8 bytes

| offset | type | meaning | status |
|---:|---|---|---|
| `0x00` | `u8` | first segment index | PROVEN |
| `0x01` | `u8` | second segment index | PROVEN |
| `0x02` | `u8` | side-A candidate transition 0 | INFERRED |
| `0x03` | `u8` | side-A candidate transition 1 | INFERRED |
| `0x04` | `u8` | side-B candidate transition 0 | INFERRED |
| `0x05` | `u8` | side-B candidate transition 1 | INFERRED |
| `0x06` | `u8` | follower output/index copied to AI state | PROVEN access; exact semantic name OPEN |
| `0x07` | `u8` | reserved/unknown | OPEN |

The two candidate pairs are selected from a line-side test.  Their absolute
orientation is deliberately not named “forward” or “backward”: the follower
can traverse either direction.  The graph's nodes are transition records; an
edge is one of `+2..+5`; `+0/+1` name the two geometric segments evaluated at
the node.  This is enough to reproduce centerline geometry and topology
without inventing a checkpoint format.

## Static scene directory and counts

`0x002bd578` is an array of 8-byte `{u32 segmentVaddr, u32 transitionVaddr}`
pairs.  In the static branch, `FUN_0024fcd8` uses signed scene/type byte
`param_2+0x22` as the directory index, except that type `11` aliases index `6`
and type `16` aliases index `15`.  Entries 18 and 21 are null.  The other
populated indices and segment counts are:

| index | segments | index | segments | index | segments |
|---:|---:|---:|---:|---:|---:|
| 0 | 35 | 1 | 43 | 2 | 120 |
| 3 | 143 | 4 | 66 | 5 | 163 |
| 6 | 90 | 7 | 128 | 8 | 42 |
| 9 | 37 | 10 | 143 | 11 | 90 (alias of 6) |
| 12 | 143 | 13 | 224 | 14 | 197 |
| 15 | 220 | 16 | 220 (alias of 15) | 17 | 106 |
| 19 | 23 | 20 | 86 | 22 | 15 |

For every inspected pair, `transitionVaddr - segmentVaddr == segmentCount *
24`; exactly `segmentCount * 8` bytes of transition records follow.  Therefore
the segment count is recoverable from the pair itself, with no uncommitted
count array needed.

The first populated pair is `{0x002ac620, 0x002ac968}`.  Its 35 segments
validate the rule: the first four endpoint pairs are `(590,569)->(590,571)`,
`(600,569)->(600,571)`, `(615,558)->(615,562)`, and
`(640,550)->(640,554)`.  All 35 of its endpoints are in `[0,1600]²`.  This is
the candidate scene-index-0 / **Peach Town** graph: `asset-map.md` identifies
scene-table entry 0 as Peach Town, tile id 11, `FLD/023.BIN`.  The index-to-name
binding remains **INFERRED** until the producer of the AI type byte is traced,
but the tile-local bounds check is PROVEN.

The first transition in that graph is the diagnostic byte sequence
`00 01 22 22 01 01 00 00`; it demonstrates that candidate bytes are transition
indices (not coordinates or fixed-point metadata).

## Scale verdict

**PROVEN:** route endpoints are direct IEEE-754 single-precision, tile-local
world coordinates.  They require no `2^25` conversion.  The Peach candidate
above provides a direct numeric validation against the 1600-unit `023.BIN`
footprint.

`1 << 25` remains the fixed-point *streaming-grid* scale documented in
`field-topology-status.md`: it locates a 1600-unit tile in the larger world.
It is not the storage scale of these route segment coordinates.

## Start, goal, and graph boundaries

`FUN_00218458` zeroes the 0x270-byte AI object before `FUN_0024fcd8` supplies
the tables, so the follower's current transition byte (`+0x24a`) begins at
zero in this construction path.  That demonstrates an **initial traversal
node**, not a semantic start/finish marker.  No separately encoded goal,
finish, node count, or speed-limit field was found in either record format.
Closed-looking initial transitions (for example index 0 selecting transition
34 on one side in the first graph) are graph edges, not goal markers.

## Runtime extraction specification

Use the user's own ELF, as `field_tile_index.cpp` does for `.lbn_map`; do not
embed table bytes.

1. Parse the ELF program headers.  Locate the `PT_LOAD` containing each virtual
   address and translate with `fileOffset = p_offset + (vaddr - p_vaddr)`.
   In the analyzed ELF the relevant data load has `p_vaddr=0x0029a880` and
   `p_offset=0x0009b880`, so the concrete mapping is `fileOffset = vaddr -
   0x00200000`.  Validate the headers rather than assuming that relation.
2. Read the 8-byte static pair at `0x002bd578 + 8*sceneIndex`, reject a null
   pair, and apply the two aliases above if reproducing `FUN_0024fcd8` exactly.
   Decode `n = (transitionVaddr - segmentVaddr) / 24` only when the difference
   is positive and exactly divisible by 24.  Read `n` 24-byte segments and `n`
   8-byte transitions.
3. For course/entity mode, read `u32 outer[32]` at `0x002d98a8`.  If
   `outer[T]` is nonzero, read `{segmentVaddr, transitionVaddr}` at
   `outer[T] + 8*car` and use the same validated decode.  To mimic the game,
   use `car-1` when task flags lack `0x80000` and `car != 0`; otherwise use
   `car`.  `FUN_0025fbc0` rejects null outer pointers and null segment pointers.
4. Emit endpoints as `(x,z)` float pairs and preserve transition bytes as graph
   metadata.  A centerline renderer may draw each segment independently; a
   route traversal must use the transition graph rather than assuming records
   are a single sorted polyline.

## Ledger

| claim | status | evidence |
|---|---|---|
| Navigation geometry is ROM-resident and independent of COURSE prop tails | PROVEN | `FUN_0024fcd8`, `FUN_00250068`; no COURSE pointer in their selection/read path |
| Static pair directory base is `0x002bd578` | PROVEN | direct indexed loads in `FUN_0024fcd8` |
| Course/entity outer directory base is `0x002d98a8` | PROVEN | `FUN_0025fbc0` direct indexed load |
| Segment stride/layout through endpoint B | PROVEN | `* 0x18` indexing and float accesses in `FUN_00250068` |
| Transition stride and graph-candidate role | PROVEN / INFERRED as marked | `* 8` indexing and branch-following logic in `FUN_00250068` |
| Route coordinates use `2^25` fixed point | REJECTED | ROM bytes decode as in-range `f32` endpoints |
| Static graph 0 lies in Peach Town's tile | PROVEN bounds; town-name binding INFERRED | all 35 endpoints in `[0,1600]²`; scene table entry 0 is Peach Town/023 |
| Explicit start/goal marker, segment speed limit, and auxiliary-float meaning | OPEN | no reader establishes these semantics |
| Complete names for static type indices and course `T`/entity labels | OPEN | requires tracing the producer tables, not guessing from adjacent assets |
