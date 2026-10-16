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
3. **~~256-entry attribute table~~ — REJECTED (see `cell-vif-chains.md`)**:
   the window at `offA + 65*4` overlaps cell 0's inline DMA chain (cell 0's
   chain begins at `offA + 0x190`), so those "entries" are chain payload, not
   a table. Material state instead travels inside each chain as
   `UNPACK V3-32 @ 0x192` A+D/GIF payloads — see
   `docs/native-reference/cell-vif-chains.md`.
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
`fieldTileData + slot * 0x3A8000`. `case 3`: relativize the 65-entry
directory by the tile base (the previously assumed second "256-entry table"
is rejected — `cell-vif-chains.md`), read the PMP magic at
`(header[2]-header[1])`-style offset, store `fieldCellTables[slot]`.

## Corpus behaviour (64 tiles — ALL parseable with relaxed validation)

Tile names are a 3-digit `ABC.BIN` structure: all 64 combinations of
`{0-3}{0-3}{0-3}` exist (16 per leading digit, four leading digits). Packet
vertices sit on a 40-unit grid spanning roughly 1000 units per tile. The
naming is consistent with a 4-region × 4×4 grid world; the id formula derived
from `FUN_0021c8d0` plus the ROM LBA table (see "Tile identity & streaming")
resolves digit↔filename ordering outright — 64/64 descriptor size matches,
zero LBA overlaps. Only the metric fixed-unit scale remains softly open
(cosmetic; topology pinned).

An earlier split "50 canonical / 13 layout variants / 1 header variant" was
**an artifact of overly strict validation**, not format families. The full
corpus (64/64) parses with two relaxations plus one new constant anchor:

1. header first word (header size) may be `0x20` **or larger** — `023.BIN`
   uses `0x160`; bounded by `0x20 ≤ hdrsz ≤ 0x1000` and `offA ≥ hdrsz`.
2. `fileSize` (header word 4) may be **≤** the actual file length — 14 tiles
   carry trailing padding (e.g. `011.BIN`: field `0x2a9f00`, actual
   `0x2b0190`); `023.BIN` is padded *and* header-extended.
3. the directory's first entry is `0x190` (relative to `offA`) on **all 64
   tiles** — a usable additional anchor.

Ordering note: the `offA ≤ offB ≤ offC` ordering **holds for all 64 tiles** —
the earlier "13 reordered-dir variants" note was a hex digit-count misread
(`0x1e800` < `0x1e3dc0`); those tiles differ only in trailing padding.

Totals with relaxed validation: **383,287 packets / 1,639,813 vertices
(~880k triangles) across all 64 tiles** (per-tile samples: `011.BIN` =
5901 pkts / 26523 verts, `023.BIN` = 3303 / 15252).

`tests/native_assets/test_field_converter.cpp` now asserts the relaxed
corpus (64/64 parse, 14 padded / 1 extended-header classification) so drift
in either direction is caught.

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

Corpus totals (strict-validation 50 tiles): **288,745 packets, 1,235,837
vertices, ~658k triangles**; relaxed totals (all 64): **383,287 / 1,639,813 /
~880k**. Section gap equals N-2 on 288,732/288,745
packets (99.995%). FLD/000.BIN: 6,185 packets / 25,590 vertices / 13,220
face records — pinned by the test suite.

## World placement (PROVEN from `drawFieldTiles` @ 0x21FC24)

The renderer iterates each loaded tile as **8×8 cells of 200 units**
(1600×1600 total — exactly matching packet vertex extents `x,y ∈ [0,1600]`,
height `z ∈ [-53, +51]`). Per cell it runs `cellVisible(origin +
col*200 + 100, origin + row*200 + 100)` frustum culling and, when visible,
`vifCallModel(fieldCellTables[slot*4 + cell])` — the 65-entry directory's
first 64 slots are the per-cell model refs (slot 65's role unresolved — the
older "attribute table base" reading was rejected, see `cell-vif-chains.md`).
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

## Tile identity & streaming (digit order RESOLVED 2026-10-16 — static proof)

**id ↔ filename:** for tile `ABC.BIN` (digits A,B,C ∈ 0..3):

```
id = (A << 4) | (B << 2) | swap2(C)      // swap2 swaps C's two bits
                                        // C=0→0, 1→2, 2→1, 3→3
```

**Proof chain (no runtime needed):**
1. `FUN_002201c0` spawns neighbor requests `gs_tasks_spawn(ctx, doneCb, id | 0x390000)`
   where `id = FUN_0021c8d0(x, y)`; world step constants ±0x1000000 (columns)
   and ±0x2000000 (rows) — fixed-point grid, 1<<24 per column slot,
   1<<25 per row (row parity shear `b` shifts alternate rows by half a slot:
   diagonal/brick lattice, columns indexed 0..15, each row uses parity `b`).
2. Task dispatcher `FUN_0020d120`, category nibble 9 (`tag & 0xf0000 == 0x90000`):
   `loadFieldTile(req, tcb, 0x29bb50 + (tag & 0xff) * 0xc, tag & 0xff, tag & 3)`
   — **ROM table of 12-byte descriptors indexed by the 6-bit id**
   (the table physically continues past index 63 with COURSE and ACTION
   sub-blocks — 103 records total, see `asset-map.md`; category 9 indexes
   only the FLD 64):
   `{u32 pack constant 0x00333388; u32 sectorOffset; u32 sectorCount}`,
   slot = id & 3 (the four 0x3A8000 streaming buffers).
3. Cross-check vs disc: for every `ABC.BIN`, `ceil(physicalSize / 2048)`
   equals `table[formula(A,B,C)].sectorCount` — **64/64 matches**, and the
   64 LBA ranges are pairwise **non-overlapping** (0 collisions). Size-twin
   names (18 files share 0x4A5 sectors; 4 share 0x4C8) are separated by the
   formula into distinct ids; the twins that are byte-identical content are
   interchangeable anyway.
4. Code semantics of `FUN_0021c8d0`: `id = ((y>>25)&6)*8 + (((x>>24)-b)&0xE) + b`,
   `b = (y>>25)&1`. Bit-for-bit: id[5:4] = y[26:25] (= A, so b = A&1 ✓),
   id[3:1] = ((x>>24) − b) & 0xE, id[0] = b. Inverting (tentative metric
   placement — see open note below) for tile `ABC`:
   `rowBits = (A<<1)|b` (y base = rowBits << 25) and
   `xIndex = ((B<<1)|(C&1)) + b` (x base = xIndex << 24, mod 16).

Residual (cosmetic, optional): one live trace of `gs_tasks_spawn` at an id
change would confirm the metric scale mapping fixed-units → the 1600-unit
tile extents; topology is fully pinned statically.

Loader chain (unchanged summary): `loadFieldTile` = `FUN_0020c188` — case 0/1
DMA the 0x800-byte header window, case 2 walks/validates REF chain + size caps
(0x3A8000; slot 3 0x528000) + bulk read into `fieldTileData + slot*0x3A8000`,
case 3 relativizes the 65-entry directory (`fieldCellTables[slot]`).
`printf` traces: `misc %d` / ` Total Size=0x%x`.

## Historical note: previous streaming hypothesis (PROVEN from `FUN_0021c8d0` / `FUN_002201c0`)

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
filename digits: `A = id>>4`, `B = (id>>2)&3`, `C = swap2(id&3)` — matching
the LBA-table proof above (`swap2` swaps the low two bits).

Practical consequence for the native app: with the car at world `(x, z)`, the
visible world is this tile + the quadrant neighbours chosen by the same id
walk; 4 tile slots stream, matching the 4 × `0x3A8000` slots in
`fieldTileData`.

CLOSED 2026-10-16 (`world-geography.md`): the fixed-unit scale is pinned —
the f32 divisor at `0x332c0c` is `20971.51953125 = f32(2^25/1600)`, so one
tile spans exactly `0x02000000` fixed units on an `0x10000000`-wrapped grid
(8 rows × 16 half-columns, odd-row shear `b`). The old note's "only 32 of 64
ids reachable through `FUN_0021c8d0`" was a misread: `A=(z>>26)&3` (id bits
5:4) and `b=(z>>25)&1` (id bit 0) are independent — all 64 ids are
car-reachable, consistent with the 64 valid descriptors (41 unique contents fit
comfortably). The name↔id assignment remains pinned by size equality per id.

## Runtime chain construction (PROVEN from loader case 3 @ 0x20C430 region)

`FUN_0020c188` after DMA-completing the tile into `fieldTileData[slot*0x3A8000]`:
- sets `fieldCellTables[slot] = tileBase` and **relativizes all 65 directory
  entries in place** (65 iterations, entry += tileBase) — they become absolute
  addresses of **per-cell VIF/DMA chains inside the loaded tile** (entry0 =
  tileBase+0x190).
- a parallel tag-walk (case 2) patches each DMA tag's address qword
  (`tag+0x10`) to absolute and calls `sendTexturePackets(chainStart)` —
  textures upload at LOAD time, before any model call.
- the request type byte (`param+0x22`) selects chain-table start index 4 or 5
  (`'\x12'..'\x1c'`→5, `'&'`→4) and walks nonzero directory words from there,
  size-capped at 0x750000 ("Total Size=" / `Field Size Over!` fatal). Index 4/5
  matches the MSCALF imm 0x4/0x8 VU1 entries — terrain categories, with more
  categories reachable via other type bytes.

So: VIF chains, vertex/UV payloads, textures and model calls all live inside
one tile buffer; the directory indexes their chains. Chain-level layout mapping
(UNPACK offsets, stride, material binding) is tracked in
`vu0-macro-streams.md` / `vu1-programs.md` / `cell-vif-chains.md`.

## What remains for real terrain

- Bind materials end-to-end: parse each chain's `UNPACK V3-32 @ 0x192`
  A+D/GIF payload (TEX0/CLUT/FRAME register words) and connect to texture
  pages carried in the same tile (`sendTexturePackets` uploads them at load).
  UV location itself is solved — per-vertex `(s,t,alphaAdd)` is streamed
  (`cell-vif-chains.md`); `materialRef` is not literal in chains and its
  old attribute-table path is rejected, so binding must come from the GIF
  register payloads per chain. Active: `field-textures.md` (texture pages).
- ~~VU0 macro disassembler~~ — **resolved 2026-10-15**: the game contains no
  VU0 macro usage at all (VCALLMS = 0; zero MPG uploads across 4,834 VIF
  chains); VU0 runs only inline macro-mode math utilities in the ELF. All
  vertex processing is VU1, whose shared image is located and disassembled
  (`vu1-programs.md`) with a golden-verified standalone disassembler
  (`/private/var/folders/.../opencode/vu-dis/vu-dis`).
- ~~Apply validation relaxations~~ — **done 2026-10-16**: `parseFieldTileHeader`
   accepts `0x20 ≤ hdrsz ≤ 0x1000` and `fileSize ≤` physical size; corpus
   tests now assert 64/64 parse (14 padded / 1 extended-header). The rumored
   dir/PMP reordering did not exist (hex misread, corrected above).

`assets/field_converter.{h,cpp}` exposes `catalogFieldTile()` (header →
directory walk → attribute count → PMP magic/section table → extension
offset) and `extractFieldPackets()` (drift-proof anchored packet walk). Tile
geometry itself is fully extracted today; authentic UV/material binding is
tracked in the companion VU docs, not blocked on VU RE anymore.
