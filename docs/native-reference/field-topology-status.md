# FLD tile format — container structure DECODED (was: topology blocked)

Updated 2026-10-15. The earlier negative result ("padding/macro-driven")
predated locating the runtime loader. Ghidra xrefs to `fieldCellTables`
(0x177AB20) lead to `loadFieldTile` @ 0x20BD4C / `FUN_0020c188` @ 0x20C188,
which hand the container semantics directly. Verified numerically against
`FLD/000.BIN` (2.6 MB) on the user's disc extract.

## Header (5 x u32 @ 0x00)

`[0x20, offA, offB, offC, fileSize]` — 000.BIN: `0x20, 0x5800, 0x1b2490,
0x263b80, 0x263e70`.

## Regions

1. **`[0x20, …)` tile DMA packets.** Four DMA REF tags (hi byte `0x10` =
   REF/chain, QWC = low 16 bits, addr = bits 0–27; addresses are RUNTIME RAM
   offsets against load base `0x100000`, some targeting scratchpad
   `0x1FF0xxxx`). Payloads are inline (tag + QWC quadwords). This is the
   texture/VIF staging the old analysis mistook for a vertex index region.
2. **`offA`: 65-entry monotonic pointer table**, offsets **relative to offA**
   (the loader adds the copy base at runtime — `*p += base`, 65 iterations).
   All 65 non-zero and in-range for 000.BIN; each target begins with a DMA
   tag word (`0x10000259`-style: QWC + `EN|TADR` bits) followed by its
   payload — one sub-block per model chunk.
3. **Immediately after: 256-entry attribute table** (248 non-zero,
   non-monotonic — material/attribute words, semantics still open).
4. **`offB`: PMP model block** — magic word **`0x608`** (or `0x600`),
   literally printed by the loader as `"Old Style PMP format"` for 0x600.
   Followed by a monotonic section-pointer table (`0x1ab8, 0x1ff8, 0x2488,
   …`, relative); 0x608 = new style with an extension block whose file offset
   is the word at `offB + 0x180` (`0x2d838` for 000.BIN, relative).
   Q≈1.0 float6 vertex density starts immediately inside this block —
   matching the long-standing density-peak observation for `[offB, offC)`.
5. **`[offC, fileSize)`**: small tail (`0x2f0` B here) — trailing records.

## Loader state machine (FUN_0020c188)

`case 0/1`: DMA first `0x800`-gram windows of the file into scratch
`0x100000`. `case 2`: walk the REF-tag chain (`byte3 == 0x10`, terminate
`byte3 == 'p'` = `0x70` = REF-End), sync DMA chan 1, `sendTexturePackets`,
size-check `< 0x750000`, copy `DAT_00100004..end` into
`fieldTileData + slot * 0x3A8000`. `case 3`: relativize the 65-entry and
256-entry tables by the tile base, read the PMP magic at
`(header[2]-header[1])`-style offset, store `fieldCellTables[slot]`.

## Corpus behaviour (64 tiles)

Tile names are a 3-digit `ABC.BIN` structure: all 64 combinations of
`{0-3}{0-3}{0-3}` exist (16 per leading digit, four leading digits). Packet
vertices sit on a 40-unit grid spanning roughly 1000 units per tile. The
naming is consistent with a 4-region × 4×4 grid world; the id formula derived
from `FUN_0021c8d0` (see "Tile identity & streaming") closes most of the gap,
but the digit↔filename ordering and the fixed-point scale still need one
runtime cross-check before wiring tiles to absolute map coordinates.

`tests/native_assets/test_field_converter.cpp` catalogs all `FLD/*.BIN`:
**50 canonical** (this layout), **13 layout variants** where `offB < offA`
(alternate region ordering — e.g. `011.BIN`: `offA=0x1e800`,
`offB=0x1e3dc0`), and **1 header variant** (`023.BIN`, first word `0x160`).
Variant parsing is deferred; the split is asserted in the suite so drift is
caught.

## Section packet grammar (PROVEN, extraction implemented)

`extractFieldPackets()` walks each section anchored on packet headers —
`[0x8000|vertexCount, 0x20224000, 0x41, materialRef]` — whose word1/word2
are denormal-as-float bit patterns that never collide with live vertex data,
making the walk drift-free. Each packet is:

1. `N` float4 vertices `(x, z, y, q)` (q≈1.0),
2. `N-2` trailing quads `(nx, ny, nz, w)` — face data whose components are
   **slope-encoded** (near-vertical faces carry components in the thousands,
   so they are gradient/slope records, not unit normals).

Vertices form a triangle fan. Coordinates sit on a 40-unit grid (observed
values are multiples of 40), so host UVs use a planar projection `coord/40`
as the documented stand-in until authentic UVs are traced through the VU0
path.

Corpus totals (all 50 canonical tiles): **288,745 packets, 1,235,837
vertices, ~658k triangles**; section gap equals N-2 on 288,732/288,745
packets (99.995%). FLD/000.BIN: 6,185 packets / 25,590 vertices / 13,220
face records — pinned by the test suite.

## World placement (PROVEN from `drawFieldTiles` @ 0x21FC24)

The renderer iterates each loaded tile as **8×8 cells of 200 units**
(1600×1600 total — exactly matching packet vertex extents `x,y ∈ [0,1600]`,
height `z ∈ [-53, +51]`). Per cell it runs `cellVisible(origin +
col*200 + 100, origin + row*200 + 100)` frustum culling and, when visible,
`vifCallModel(fieldCellTables[slot*4 + cell])` — the 65-entry directory's
first 64 slots are the per-cell model refs (slot 65 = attribute table base).
Up to four parallel cell tables (`DAT_0177ab20/24/28` verified, fourth
sibling implied by the fourth render block) render four tile
layers simultaneously, each anchored by a vec4 origin from
`tileQuadrantOrigins @ 0x29d080` — observed entries `(-800,0,1600)`,
`(800,0,1600)`, `(-1600,0,0)`, `(0,0,0)` = half/full-tile relative anchor
offsets for neighbouring/streamed tiles. `fieldTileData` holds 4 slots of
0x3A8000 (`slot * 0x3A8000` in the loader), so ~4 tiles stream around the
player. Which of the 64 `ABC.BIN` files occupies which world grid position
is still chosen by the streaming-request code (`FUN_0020aef0`/`FUN_0020b348`)
and remains to be pinned; the per-tile geometry→world transform is simply
`origin + local(x, z, y)`.

## Tile identity & streaming (PROVEN from `FUN_0021c8d0` / `FUN_002201c0`)

`FUN_0021c8d0(x, y)` maps fixed-point world coordinates (offsets relative to
the section origin pair at `+0x58/+0x5C`, negatives wrapped modulo
`0x1FFFFFF`/`0xFFFFFF`) to a **6-bit tile id** — exactly 64 tiles, matching
the 64 `ABC.BIN` files:

```c
b  = (y >> 25) & 1;                          // row-parity bit
id = ((y >> 25) & 6) * 8                   // Y-region (y bits 26-25) * 16
   + (((x >> 24) - b) & 0xE)               // X index, parity-shifted, bits 1-3
   + b;                                     // parity in bit 0
```

X granularity is `1<<24` per half-index; the neighbour-quadrant picker
`FUN_002201c0` steps origins by ±`0x1000000`/±`0x2000000` around the car
(`+0x58/+0x5C`), computes up to four neighbour tile ids (centre, the four
diagonals — cardinal neighbours appear implicitly through the x-shift), and
for each: assigns a render slot (`DAT_003335b8..bb` = quadrant codes 0–6)
and, on id change, spawns a loader task
`gs_tasks_spawn(…, FUN_0020dc50, id | 0x390000)` — request type tag
`0x390000` with the 6-bit tile id in the low bits. The debug format
`[%d%d%d]` (@0x2EEAD8, xrefs in the same file) prints the id as the three
filename digits — consistent with `A = id>>4`, `B = (id>>2)&3`, `C = id&3`,
though the digit↔file ordering itself is inferred, not observed.

Practical consequence for the native app: with the car at world `(x, z)`, the
visible world is this tile + the quadrant neighbours chosen by the same id
walk; 4 tile slots stream, matching the 4 × `0x3A8000` slots in
`fieldTileData`.

## What remains for real terrain

- Map `materialRef` (and the 256-entry attribute table) to the tile's texture
  set so packets render with authentic textures instead of planar UVs.
- Interpret the 65 sub-block DMA payload streams (VIF-adjacent, feeds VU0
  macros) — needed for per-cell placement/prop detail, not the tile mesh.
- Parse the 13 `offB < offA` layout-variant tiles and the `0x160`-header
  variant (asserted counts in the corpus test).

`assets/field_converter.{h,cpp}` exposes `catalogFieldTile()` (header →
directory walk → attribute count → PMP magic/section table → extension
offset) and `extractFieldPackets()` (drift-proof anchored packet walk). The
"needs a VU0 macro disassembler" blocker applies only to sub-block payload
detail, authentic UVs/material binding, and COURSE route streams — tile
geometry itself is fully extracted today.
