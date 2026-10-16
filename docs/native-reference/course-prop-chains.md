# COURSE prop chains — tail packet decode (C00..C14, C18..C20)

Updated 2026-10-16. Companion to `course-routes.md` (which proved the tail is
a prop draw chain and pinned `N`), `cell-vif-chains.md` (VIF/DMA/GIFtag
bitfields), `field-topology-status.md` (tile-local space), and
`vu1-programs.md` (VU1 entry table). This document decodes the prop chain
records themselves so props can be parsed, placed, and rendered natively.

Method: transient Python decoders under `/tmp` over the read-only disc
`COURSE/*.BIN`; every structural claim below was walked on all 18
loader-referenced files (the corpus of `course-routes.md`). No game bytes are
retained here — only structures, constants, counts, and small illustrative
decoded numbers.

## Result (PROVEN)

Each course tail is **exactly one primary packet block of `N` prop draws**,
byte-exact end to end:

* DMA: one `CNT` tag (`byte3 0x10` = DMA id 1 = CNT, `TTE`) + one zero-QWC
  `RET` (`0x60`). (An earlier note called `0x10` "REF"; REF is DMA id 3,
  `byte3 0x30`, and does not occur here.) No REF/REFE/NEXT/CALL/END tag of
  any kind appears in a tail.
* VIF (stream = tag bytes 8..15 [TTE] + `qwc*16` inline bytes): exactly
  `STCYCL 0x0101` (from TTE), then per draw
  `UNPACK V4-32 NUM=1 →TOPS+0` (GIF tag), `UNPACK V3-32 NUM=4·NLOOP →TOPS+1`
  (vertex units), `MSCALF imm 0x000A`, plus 0–3 trailing `NOP` words.
* Each draw is **one untextured, flat-shaded, alpha-blended GS tristrip**
  (`PRIM 0x264`, `PRE=1`, `EOP=1`, `FLG=PACKED`, `NREG=3`,
  `REGS=0x412 = {ST, RGBAQ, XYZF2}`) of `NLOOP` vertices, 3..50.
* Each vertex unit is 48 bytes = 4 V3-32 vectors; only `x`, `z` and one baked
  alpha/normal constant actually vary (see record layout). `y ≡ 0` in every
  vertex of the corpus: all COURSE props are flat ground-plane strips.
* Cross-checks agree per file: prologue `N` == counted `MSCALF 0xA` ==
  complete draw triplets == distinct per-draw geometry payload groups
  (18/18 files; matches `course-routes.md` pins).

Five files (C02, C03, C05, C13, C20) carry **additional packet blocks** after
the primary block. Their container structure is proven; their payload is not
VIF and is left OPEN (see below).

## Container boundaries and packet blocks

Header (`u32[hdrSize/4]`, `hdrSize = 0x20` on 15 files, `0x30` on C03/C05/C13):

```text
u32[0]      hdrSize (0x20 or 0x30)
u32[1..3]   offA, offB, offC           (offC = start of the prop tail)
u32[4..]    boundary list (region end offsets, monotone, zero padded);
            last boundary == physical file size in every corpus member
```

`boundary[0]` is the end of the primary prop packet (the `RET` offset), and
each further boundary ends one extra packet block. The boundaries partition
`[offC, fileSize)` exactly. Counts: 13 files have 1 boundary (no extras);
C02 and C20 have 2; C05 and C13 have 4; C03 has 5.

`C16.BIN` (the known orphan) has `offC == fileSize` and boundary word `0`:
its tail is **zero-length** (0 props), which is the precise form of its
"no valid tail" exclusion in `course-routes.md`.

### Primary block layout (all 18 files)

```text
+0x00  prologue u32[4] = [0x30, 0x50, 0, N]
         0x30 = offset of second setup record, 0x50 = offset of the DMA chain,
         0    = no next block, N = prop draw count
+0x10  setup record A (32 B)  = u32[8] {0x10000000, 0, 0x01000101, 0,
                                         0x60000000, 0, 0, 0}
+0x30  setup record B (32 B)  = identical to A
+0x50  main chain: CNT tag (word0 = 0x10000000 | qwc, word1 = 0,
       TTE VIF words = {0x01000101 STCYCL(1,1), 0x6C018000 UNPACK V4-32
       NUM=1 imm=0x8000}), then qwc*16 bytes of inline VIF payload,
       then RET (0x60000000, QWC=0)
```

Setup records A and B are byte-identical in all files: each is a zero-QWC
`CNT+TTE` that pushes `STCYCL 0x0101` + `NOP` to VIF1, followed by a zero-QWC
`RET` — separate no-op DMA kicks that arm the VIF cycle state before the main
chain. The prologue/extra-descriptor correspondence
(`[setupB, chain, next, N]` vs the extras' `[0x10, chain, next, 0, setupA,
setupB, 0, 0]`) is INFERRED from the offset arithmetic; the constant values
themselves are proven.

### Extra packet blocks (C02 ×1, C03 ×4, C05 ×3, C13 ×3, C20 ×1)

Each extra block spans `[boundary[i-1], boundary[i])` and contains:

```text
+0x00      descriptor u32[8] = [0x10, offChain, offNext, 0,
                                offSetupA, offSetupB, 0, 0]   (block-relative)
offSetupA/B  two 16-byte DMA tags: RET(qwc=0) and CNT(qwc=0)+TTE{STCYCL 0x0101, NOP}
offChain     DMA chain: CNT(QWC=1030) → CNT(QWC=70 or 14) → RET(QWC=2)
```

Proven regularities across all 8 extra blocks: first CNT is always
`QWC=1030`; the second is `70` (C02, C05, C13) or `14` (C03, C20); the `RET`
carries `QWC=2`. `offNext` points at the next block's descriptor and lands
exactly at its boundary. Neighbouring blocks often share identical relative
descriptors (C03 extra1==extra2, extra3==extra4; C05 extra2==extra3; C13
extra1==extra2).

**OPEN:** the extra-block chain payload decodes to noise as VIF (no valid
`UNPACK→TOPS` / `MSCALF` grammar; command histogram is indistinguishable from
arbitrary data), so these blocks are not prop draws under the primary
grammar. What consumes them (and whether they relate to the still-open
COURSE route question in `course-routes.md`) is unresolved. A parser should
skip them strictly via the header boundaries.

## C00 annotated walkthrough (PROVEN; illustrative offsets)

File facts: size 0x23EEA0 (2,354,848 B); header
`[0x20, 0xAC800, 0x22AB90, 0x23BE60, 0x23EEA0, 0, 0, 0]`; one boundary =
file size; `offC = 0x23BE60`; tail = 12,352 B.

| off | bytes | decode |
|---|---|---|
| `+0x00` | 16 | prologue u32 `[0x30, 0x50, 0, 12]` → **N = 12** |
| `+0x10` | 32 | setup A: `CNT(qwc=0)+TTE{STCYCL 0x0101, NOP}` + `RET(qwc=0)` |
| `+0x30` | 32 | setup B: identical |
| `+0x50` | 16 | `CNT` word0 `0x100002FD` → **QWC = 765**; TTE = VIF words {`STCYCL 0x0101`, `UNPACK V4-32 NUM=1 →TOPS+0`} |
| `+0x60` | 12,240 | inline payload: 12 draws (below) + 1 trailing `NOP` |
| `0x23EE90` | 16 | `RET` (qwc=0); tail end == file size |

Draw grammar (each draw: GIF tag quadword, then `UNPACK V3-32 NUM=4·NLOOP
imm=0x8001 →TOPS+1`, then `MSCALF 0x1500000A`):

| # | GIF tag @ | NLOOP | V3 NUM | geom bytes | draw bytes |
|---:|---|---:|---:|---:|---:|
| 1 | `0x23BEC0` | 47 | 188 | 2256 | 2284 |
| 2 | `0x23C7AC` | 50 | 200 | 2400 | 2428 |
| 3 | `0x23D128` | 3 | 12 | 144 | 172 |
| 4 | `0x23D1D4` | 9 | 36 | 432 | 460 |
| 5 | `0x23D3A0` | 6 | 24 | 288 | 316 |
| 6 | `0x23D4DC` | 17 | 68 | 816 | 844 |
| 7 | `0x23D828` | 3 | 12 | 144 | 172 |
| 8 | `0x23D8D4` | 49 | 196 | 2352 | 2380 |
| 9 | `0x23E220` | 15 | 60 | 720 | 748 |
| 10 | `0x23E50C` | 20 | 80 | 960 | 988 |
| 11 | `0x23E8E8` | 25 | 100 | 1200 | 1228 |
| 12 | `0x23EDB4` | 4 | 16 | 192 | 220 |

(`draw bytes` = 4 VIF cmd + 16 tag + 4 VIF cmd + geom + 4 MSCALF.) After the
last `MSCALF` at `0x23EE88` one `NOP` pads the packet; the CNT window
ends exactly at `0x23EE90`. Byte identity: `8 + 765·16 = 12,248` walked =
`4·(1 STCYCL) + Σ(28 + 48·NLOOP)` over 12 draws + 4 pad = 12,248.

Draw 1 GIF tag decode (identical shape for every draw in the corpus):
`lo = 0x313240000000802F, hi = 0x0000000000000412` →
`NLOOP=47, EOP=1, PRE=1, PRIM=0x264, FLG=0 (PACKED), NREG=3,
REGS nibbles {2,1,4} = ST, RGBAQ, XYZF2`. `PRIM 0x264` =
TRISTRIP + ABE (alpha blend); IIP=0 (flat), TME=0 (no texture), FGE=0,
AA1=0, FST=0, CTXT=0, FIX=0.

Draw 1 first vertex unit (48 B = 4 V3-32 vectors, 12 floats), illustrative:

```text
vec0 = (786.479, 0.0, 489.712)   position (x, y, z); y == 0 exactly
vec1 = (0.0, 1.0, 0.0)           baked constant (see flavors)
vec2 = (255.0, 255.0, 255.0)     baked constant
vec3 = (0.0, 0.0, 128.0)         (pad, pad, alpha=128 -> 50% via ABE)
```

`MSCALF imm 0xA` selects VU1 jump-table slot 10 = **OffsetStrip** @ VU1
`0x1898` ("unlit/offset strips", `vu1-programs.md`): consistent with the
unlit, untextured, offset strips observed here. The TOPS+0/TOPS+1 upload
contract mirrors the terrain chains in `cell-vif-chains.md` (the VU program
consumes the tag and records and re-emits PATH3 data); the exact lane→GS
register mapping is VU-program-internal.

## Prop record layout

```text
PropDraw (one VIF triplet):
  GIFtag (16 B at TOPS+0)   NLOOP=n ∈ [3,50]; EOP=1; PRE=1; PRIM=0x264;
                            FLG=PACKED; NREG=3; REGS=0x412; hi word constant 0x412
  V3-32 NUM=4n (TOPS+1)     n vertex units of 48 B
  MSCALF 0x000A

Vertex unit (48 B, 4 x V3-32):
  +0x00  f32 x          tile-local x
  +0x04  f32 y          == 0.0 in every corpus vertex
  +0x08  f32 z          tile-local z
  +0x0c  3×f32          baked normal constant, per course (flavors below)
  +0x18  3×f32          == (255,255,255) in every corpus vertex
  +0x24  2×f32          == (0,0) in every corpus vertex
  +0x2c  f32   alpha    128.0 (50% blend) or 1.0; per course
```

Two flavors exist corpus-wide, both constant within a course:

| flavor | vec1 constant | alpha | courses | units |
|---|---|---:|---|---:|
| A | (0, 1, 0) | 128.0 | C00..C13, C18, C19, C20 | 6,877 |
| B | (0, 0, 1) | 1.0 | C14 only | 383 |

The lane naming above is INFERRED (normal/color/alpha semantics for ST,
RGBAQ, XYZF2 with `TME=0`); what is PROVEN is the constant nature of lanes
`+0x0c..+0x2c` and that only `x`, `z` (and the course-wide flavor) vary.
There are **no** per-prop transform qwords, no TEX0/CLAMP/material state, no
`STROW/STCOL/STMASK/DIRECT`, and no texture coordinates anywhere in the
tails: placement and color are baked into the vertices, texture mapping is
off, and any world offset is applied from VU1 state by the OffsetStrip entry
(cf. WorldStrip's VU qword 26 batch offset, `vu1-programs.md`).

Byte accounting (exact on all 18 files), with `V = Σ NLOOP`:

```text
VIF window bytes        = 8 + 16·qwc          (TTE words + inline payload)
command content         = 4 + 28·N + 48·V     (STCYCL + per-draw 28 + geometry)
trailing NOP pad P      = 16·qwc + 4 − 28·N − 48·V      ∈ {0,4,8,12}
primary packet size     = bounds[0] − offC = 112 + 16·qwc
                          (0x10 prologue + 2×32 setup + 16 tag + payload + 16 RET;
                           where extra blocks exist the header's further boundaries
                           extend the region past this point)
```

P is exactly the 0..3 NOP words that re-align the packet, so
`qwc·16 ≡ (28N + 48V − 4) mod 16`.

## Coordinate-space verdict

**PROVEN:** prop positions are IEEE-754 floats in the same tile-local render
space as FLD terrain. All 7,260 corpus vertices satisfy `y == 0.0` exactly,
`x ∈ [73.690, 1456.111]`, `z ∈ [10.500, 1246.137]` — strictly inside the
0..1600 tile box used by `drawFieldTiles` (`field-topology-status.md`).
Values are floats, not fixed-point integers, so the file does **not** carry
world fixed-point coordinates.

**INFERRED (unchanged from `course-routes.md`):** the only defensible world
scale remains `1600 tile units == 2^25 authored fixed units` (1 unit =
20971.52 fixed). A native renderer places props at
`tileOrigin + (x, 0, z)`; the tile origin comes from the streaming quadrant
system, and any finer course-internal offset would have to come from VU1
state (not from this file).

Per-course extents (each course's props occupy its own sub-rectangle;
`y ≡ 0` everywhere):

| file | x range | z range | | file | x range | z range |
|---|---|---|---|---|---|---|
| C00 | 305.0–900.0 | 330.0–580.0 | | C10 | 73.7–515.7 | 30.9–968.7 |
| C01 | 305.0–900.0 | 330.0–580.0 | | C11 | 416.3–1231.4 | 568.0–1132.0 |
| C02 | 80.0–757.3 | 30.0–460.0 | | C12 | 140.0–740.0 | 144.0–575.2 |
| C03 | 80.0–749.0 | 10.5–689.0 | | C13 | 473.0–1094.9 | 670.5–1155.5 |
| C04 | 202.5–899.0 | 229.0–580.7 | | C14 | 455.3–1456.1 | 484.8–1206.6 |
| C05 | 258.4–836.7 | 55.3–936.3 | | C18 | 194.7–1264.2 | 253.2–1246.1 |
| C06 | 416.3–1231.4 | 568.0–1132.0 | | C19 | 194.7–1264.2 | 253.2–1246.1 |
| C07 | 408.0–1074.2 | 256.0–887.5 | | C20 | 276.4–1339.1 | 457.4–1158.5 |
| C08 | 467.8–1192.2 | 521.5–938.5 | | C09 | 467.8–1192.2 | 521.5–938.5 |

## Corpus table (18 files; all rows machine-verified)

`N` = prologue count = `MSCALF 0xA` count = complete draw triplets = distinct
per-draw geometry payload groups (all four agree on every row). `V` = total
prop vertices (Σ NLOOP). "payload bytes" = `16·qwc`. "pad" = trailing NOP
bytes. "extras" = additional packet blocks after the primary block.

| file | N | V | qwc | payload bytes | tail B | pad | flavor | extras | distinct==N |
|---|---:|---:|---:|---:|---:|---:|---|---:|---|
| C00 | 12 | 248 | 765 | 12,240 | 12,352 | 4 | A | 0 | ✓ |
| C01 | 18 | 303 | 941 | 15,056 | 15,168 | 12 | A | 0 | ✓ |
| C02 | 46 | 382 | 1,227 | 19,632 | 19,744 | 12 | A | 1 | ✓ |
| C03 | 47 | 210 | 712 | 11,392 | 11,504 | 0 | A | 4 | ✓ |
| C04 | 13 | 341 | 1,046 | 16,736 | 16,848 | 8 | A | 0 | ✓ |
| C05 | 20 | 462 | 1,421 | 22,736 | 22,848 | 4 | A | 3 | ✓ |
| C06 | 18 | 351 | 1,085 | 17,360 | 17,472 | 12 | A | 0 | ✓ |
| C07 | 15 | 404 | 1,238 | 19,808 | 19,920 | 0 | A | 0 | ✓ |
| C08 | 15 | 353 | 1,085 | 17,360 | 17,472 | 0 | A | 0 | ✓ |
| C09 | 22 | 333 | 1,038 | 16,608 | 16,720 | 12 | A | 0 | ✓ |
| C10 | 14 | 408 | 1,249 | 19,984 | 20,096 | 12 | A | 0 | ✓ |
| C11 | 18 | 351 | 1,085 | 17,360 | 17,472 | 12 | A | 0 | ✓ |
| C12 | 28 | 346 | 1,087 | 17,392 | 17,504 | 4 | A | 0 | ✓ |
| C13 | 9 | 257 | 787 | 12,592 | 12,704 | 8 | A | 3 | ✓ |
| C14 | 44 | 383 | 1,226 | 19,616 | 19,728 | 4 | B | 0 | ✓ |
| C18 | 102 | 885 | 2,834 | 45,344 | 45,456 | 12 | A | 0 | ✓ |
| C19 | 102 | 885 | 2,834 | 45,344 | 45,456 | 12 | A | 0 | ✓ |
| C20 | 9 | 358 | 1,090 | 17,440 | 17,552 | 8 | A | 1 | ✓ |
| **Σ** | **552** | **7,260** | | | | | | 12 | |

qwc/tail-bytes/N agree with the pins in `course-routes.md` on all 18 rows.
Every V3 `NUM` in the corpus is divisible by 4; `NLOOP` spans `[3, 50]`
(a tristrip of n vertices = n−2 triangles; the 50 cap is baked into the
assets, not a VIF limit — `NUM=4·50=200 ≤ 255`).

Sharing: `distinct == N` per file (a geometry payload never repeats inside a
file — hashes are position-sensitive). Corpus-wide there are **426 distinct
geometry payloads**: C00/C01 share 6, C06/C11 share all 18, C18/C19 share all
102. The primary tails of C06 and C11 are byte-identical, as are C18 and C19
(sha1-checked over `[offC, RET end)`), consistent with their identical
ranges above.

## Parser contract (proposal for a future native extractor)

Described, not implemented — no source changes accompany this document.

```text
parseCoursePropTail(file: span<const u8>) -> expected<CoursePropTail, Error>

CoursePropVertex { float x, z; uint8_t alpha; }        // y == 0; flavor constants implied
CoursePropDraw   { uint32_t vertexCount;               // == NLOOP, 3..50
                   GifTag tag;                         // decode: NLOOP/EOP/PRE/PRIM/FLG/NREG/REGS
                   std::vector<CoursePropVertex> verts; }
CoursePropBlock  { bool primary; uint32_t propCount;   // N (primary only)
                   std::vector<CoursePropDraw> draws; }
CoursePropTail   { std::vector<uint32_t> boundaries;   // header boundary list, last == size
                   CoursePropBlock primary;
                   uint32_t extraBlockCount; }          // extras are skipped, not parsed
```

Walk rules: parse the header boundary list; treat `[offC, bounds[0])` as the
primary block, each further `[bounds[i-1], bounds[i])` as an opaque extra
block to skip (never dereferenced). Primary: expect prologue
`{0x30, 0x50, 0, N}`, skip two 32-byte setup records, start the chain at
`offC+0x50`, transfer `qwc` quadwords of VIF (TTE words are the first two
VIF codes at tag+8), require `RET(qwc=0)` next.

Validation criteria (all currently hold 18/18; a native extractor should
assert them disc-gated):

1. Header: `hdrSize ∈ {0x20, 0x30}`; boundaries monotone; last == file size;
   `bounds[0] − offC == 112 + 16·qwc`.
2. Prologue words 0..2 == `{0x30, 0x50, 0}`; `N ≥ 1`; both setup records
   byte-equal the constant 8-tuple `{0x10000000, 0, 0x01000101, 0,
   0x60000000, 0, 0, 0}`.
3. DMA: chain ids exactly `CNT` (byte3 `0x10`) then `RET` (`0x60`, qwc 0);
   no REF/REFE/NEXT/CALL/END; TTE words == `{0x01000101, 0x6C018000}`.
4. VIF grammar: exactly one `STCYCL 0x0101`, `N × (V4-32 NUM=1 imm 0x8000 →
   V3-32 NUM=4·NLOOP imm 0x8001 → MSCALF imm 0x000A)`, `0..3` trailing NOPs,
   no other commands, stream ends exactly at the RET.
5. GIF tags: `NLOOP ∈ [3,50]`, `EOP=1`, `PRE=1`, `PRIM=0x264`, `FLG=PACKED`,
   `NREG=3`, `REGS=0x412`; every `V3 NUM == 4·NLOOP`.
6. Vertex units: `y == 0`; `lane1 ∈ {(0,1,0), (0,0,1)}`;
   `lane2 == (255,255,255)`; `lane3 == (0,0,alpha)`, `alpha ∈ {128, 1}`;
   `x, z ∈ (0, 1600)`; one flavor per course (constant alpha and normal).
7. Byte identity: `16·qwc == 28·N + 48·V + P − 4` with `P` the trailing NOP
   byte count; `tail == 112 + 16·qwc`.
8. Corpus pins (values above, `course-routes.md`-style): per-file
   `(N, V, qwc)` table; `C16` empty tail; C06==C11 and C18==C19 primary-tail
   byte identity; 426 distinct geometry payloads (position-sensitive sha1 of
   each draw's 48·NLOOP geometry bytes).

Deliberate non-goals: do **not** interpret draws as routes/checkpoints or a
centerline (`course-routes.md` rejected that), and do not resolve the extra
blocks' payload from this grammar — they are outside it. Texture binding is
unnecessary for these draws (`TME=0`); rendering needs only the strip
topology, the flat color/alpha, and a tile-relative offset supplied by the
host.
