# Field-cell VIF chains — PMP bridge and streamed UVs

Updated 2026-10-15. This pass is against the read-only disc
`FLD/000.BIN` (2,506,352 bytes).  No game bytes are retained here.  File
offsets below are offsets in that file.

## Result

**PROVEN:** each of the 64 field-cell directory entries names one prebuilt,
inline VIF1 DMA chain.  Its strips are already expanded into the five-qword
`WorldStrip` input contract:

| qword in record | VIF source | Contents |
|---:|---|---|
| 0 | first V3-32 vector | position `(x, height, y)` |
| 1 | second V3-32 vector | colour set A |
| 2 | third V3-32 vector | colour set B |
| 3 | fourth V3-32 vector | colour set C |
| 4 | fifth V3-32 vector | `(s, t, alphaAdd)` |

Thus the VIF `V3-32 @ TOP+1` source is an interleaved **60-byte record**
(five 12-byte vectors), not an array of compact PMP vertices.  Every draw
batch has an immediately preceding V4-32 GIF strip tag at `TOP+0`, followed
by `UNPACK V3-32 @ TOP+1`, whose `NUM` is exactly `5 * vertexCount`, then
`MSCALF 8` (WorldStrip).  This is the missing PMP-to-GPU bridge at the final
asset representation: the VU receives positions, three colours, and authored
UVs directly.

**OPEN:** this establishes the render-side record, but not a unique
PMP-packet-to-strip identity: PMP positions repeat, so matching a position
triple can identify one or more source packets.  Neither the literal PMP
`materialRef` nor a usable material-index table occurs in the normal chains
of the simple `000.BIN` terrain case.  The material choice must therefore be
resolved by the EE-side builder/loader (or a separate texture state chain),
not inferred from UVs.

## DMA and VIF parsing

Header is `[0x20, 0x5800, 0x1b2490, 0x263b80, 0x263e70]`.  Directory entries
are relative to `0x5800`; entries 0--63 are cell chains.  All 64 chains in
this tile have this DMA shape:

```text
CNT(qwc) with TTE: the tag upper 64 bits are the first two VIF codes,
then qwc*16 bytes of inline VIF FIFO source; RET(qwc=0) terminates.
```

There are **64 CNT**, **64 zero-QWC RET**, and **zero REF/REFE/NEXT/CALL**
tags in those chains.  In particular, no VIF record is supplied by a DMA REF
address into `[offB, offC)`; the record bytes are inline in `[0x5990,
0x1b2470)`.  Payload ranges in this document use half-open `[start,end)`.
VIF was decoded as `cmd=(word>>24)&0x7f`, `num=(word>>16)&0xff` (`0` = 256),
`imm=word&0xffff`; V3/V4-32 payload sizes are `num*12`/`num*16`.

### First three chains, in directory order

| cell | directory rel. | DMA file range | CNT QWC | VIF commands | strip groups |
|---:|---:|---:|---:|---:|---:|
| 0 | `0x190` | `[0x5990,0xeea0)` | `0x94f` | 445 | 146 |
| 1 | `0x96a0` | `[0xeea0,0x11450)` | `0x259` | 134 | 43 |
| 2 | `0xbc50` | `[0x11450,0x15a10)` | `0x45a` | 211 | 68 |

The range includes the initial DMA tag and terminal RET.  Exact V3 `NUM`
histograms (the first five-vector material upload is included) are:

| cell | V3-32 NUM: count | V4-32 count | MSCALF 8 count |
|---:|---|---:|---:|
| 0 | `5:3, 15:79, 20:43, 25:5, 30:13, 40:4, 50:2` | 146 | 146 |
| 1 | `5:2, 15:39, 20:1, 25:2, 35:1` | 43 | 43 |
| 2 | `5:3, 15:45, 20:12, 25:3, 30:5, 40:2, 90:1` | 68 | 68 |

`NUM/5` is the submitted vertex count for every post-tag V3 upload.  The
first V3 in each cell is the separate five-vector material/header upload to
VU address `0x192`; it is not a WorldStrip vertex batch.

### Cell 0 end-to-end structural trace

The first tag's TTE VIF words provide commands 0--1.  The initial material
block is identical in cells 0, 1, 3, and 4; cell 2 differs only in one
payload word.  It is followed by 146 repetitions of the strip group.

| order | VIF command | NUM | destination / imm | payload file range | meaning |
|---:|---|---:|---|---|---|
| 0 | `STCYCL` | -- | `0x0101` | none | one write per cycle |
| 1 | `UNPACK V3-32` | 5 | `TOPS+0x192` (`imm=0x8192`) | `[0x59a0,0x59dc)` | batch GS/header state |
| 2 | `UNPACK V4-32` | 1 | `TOPS+0` (`imm=0x8000`) | `[0x59e0,0x59f0)` | output GIF tag |
| 3 | `UNPACK V3-32` | 30 | `TOPS+1` (`imm=0x8001`) | `[0x59f4,0x5b5c)` | six five-qword vertices |
| 4 | `MSCALF` | -- | `0x0008` | none | WorldStrip |
| 5 | `UNPACK V4-32` | 1 | `TOPS+0` | `[0x5b60,0x5b70)` | next GIF tag |
| 6 | `UNPACK V3-32` | 30 | `TOPS+1` | `[0x5b74,0x5cdc)` | six vertices |
| 7 | `MSCALF` | -- | `0x0008` | none | WorldStrip |
| 8 | `UNPACK V4-32` | 1 | `TOPS+0` | `[0x5ce0,0x5cf0)` | next GIF tag |
| 9 | `UNPACK V3-32` | 20 | `TOPS+1` | `[0x5cf4,0x5de4)` | four vertices |
| 10 | `MSCALF` | -- | `0x0008` | none | WorldStrip |
| 11--442 | `V4-32(1)`, `V3-32(5*n)`, `MSCALF(8)` | -- | `TOPS+0`, `TOPS+1`, `8` | inline, contiguous | remaining 143 strip groups |
| 443--444 | `NOP`, `NOP` | -- | `0` | none | FIFO padding |

The final inline VIF byte is `0xee8f`; the following tag at `0xee90` is
`RET(qwc=0)`, and directory cell 1 begins at `0xeea0`.  This is a complete
DMA-chain boundary, not a scan heuristic.

## PMP correspondence and references

**PROVEN (byte comparison):** qword 0 of VIF five-vector records has exact
12-byte matches to PMP vertex `(x,height,y)` triples.  Examples selected from
three different cells:

| cell | inline VIF position range | record | matching PMP packet position(s) | materialRef in matching packet |
|---:|---|---:|---|---:|
| 0 | `[0x59f4,0x5a00)` | first record of V3 NUM 30 | `0x1b2c48 + 0x10` | `0x00100651` |
| 1 | `[0xef40,0xef4c)` | record 1 of V3 NUM 35 | `0x1b42a8 + 0x10` | `0x00100651` |
| 2 | `[0x114b4,0x114c0)` | first record of V3 NUM 90 | `0x1b52e8 + 0x10` | `0x00100651` |

Cell 0 has 221 position-record matches, cell 1 has 93, and cell 2 has 139
(a position may match several PMP packets because the mesh reuses positions).
The position is **copied inline**, not REF-addressed: all 64 chain DMA tags
are CNT/RET only, and none has an address into PMP.  This answers the
inline-vs-REF question for at least cells 0--2 and, by the complete tag walk,
for all cells of `000.BIN`.

## Authentic UV stream

**PROVEN:** there are no V2-32 uploads in the 64 cell chains.  UVs are the
first two floats of qword 4 in each V3-32 five-vector record:

```text
record base + 0x00: position xyz
record base + 0x0c: colour A rgb
record base + 0x18: colour B rgb
record base + 0x24: colour C rgb
record base + 0x30: UV/alphaAdd = s, t, alphaAdd
next record: base + 0x3c
```

For cell 0's first batch, qword 4 begins at `0x5a24`; the next vertex's qword
4 is `0x5a60`.  Thus UV stride is **60 bytes per submitted vertex** (and
12-byte component-vector stride).  Across all cells, 15,746 records yielded
31,492 finite UV components: range `[-8.520119, 9.0]`; 26,269 components
(83.42%) lie in `[0,1]`, and 11,339 complete pairs (72.01%) do.  The most
frequent values are `1.0` (8,377), `0.0` (8,246), and `0.5` (261).  Other
samples include `(0.135288,0.954359)`, `(0.209859,0.507742)`,
`(0.484301,0.065970)`, and wrapping pairs such as `(0.553264,-0.396851)`.
This is authored UV data with wrapping, not a `coord/40` reconstruction.

## MaterialRef and the alleged attribute table

The section-bounded PMP walk finds 6,185 packet headers in `000.BIN`; their
only material words are `0x00100651` (5,926 packets) and `0x80100651` (259
packets).  Neither
four-byte word occurs in the normal cell-chain region `[0x5990,0x1b2470)`.
The VIF material/header upload at `TOPS+0x192` is present once per cell, but
is not a literal copy of either word.  It cannot yet be decoded as TEX0/CLUT:
it is five V3 writes (60 source bytes), and its destination qwords' fourth
lanes are inherited VIF/VU state.

### Falsified table location

The previously reported 256-word location, `offA + 65*4 = 0x5904`, cannot be
an independent table in this file.  It overlaps the first cell's verified
DMA chain `[0x5990,0xeea0)`: table indices `35..255` are literally VIF
commands and inline strip data.  For example index `35` is the low word of
the CNT tag at `0x5990`; indices `37..` include `STCYCL`, `UNPACK`, and their
payload.  Treating its 248 nonzero words as attributes is therefore invalid.

For reproducibility, here are all 256 u32 values at that *claimed* location,
not a raw payload dump and **not** a decoded attribute table:

```text
00: 002b0092 00900044 002f0031 00670032 00320033 0038005b 004f0031 00330033 00310032 00420055 00330031 00310033 00320031 005b0032 0032002f 00e50069
10: 00450088 003d0047 004d002b 002e0031 0114004b 00320064 004f002e 002f006a 00320034 00310042 0033002d 00870033 01a006f5 002a0032 009c0031 002f0031
20: 00000000 00000000 00000000 1000094f 00000000 01000101 68058192 00008004 10000000 0000000e 00080128 00000f80 00000015 dd40a880 20051405 00000007
30: 00000000 00000000 00000009 8a90a8a1 2000000a 00000035 6c018000 00008006 313e4000 00000412 313ec000 681e8001 428c77c0 c21f2daa 42c53940 40bea327
40: 420c9472 420c9472 3f800000 42086ed0 4202365d 40b9f370 42091fc0 4227f2ff 3e0a88fc 3f7450db 3f800000 4283faf8 c222892b 42c17c78 40ed91b1 423c3a1a
50: 423c3a1a 3f800000 4223ca54 420fcbf9 40c382df 421ae781 42394cbb 00000000 3f800000 3f800000 428b9478 c1fca634 42c748a0 413074f7 4223de00 4223de00
60: 3f800000 421e2b15 42159eb5 412b57cc 421f1e3c 42422608 3e56e52e 3f01fb64 3f800000 42844cc0 c1f5b458 42c4f370 4184f4f5 426ed1a8 426ed1a8 3f800000
70: 424b482a 422e34c3 4153f4c8 423e5bfc 4261d590 3e006d3f 3eebcbae 3f800000 42919010 c1bba78e 42cedd88 41a0d4e8 42596746 42398480 3f800000 425150c3
80: 42284368 419bd877 4252a9d9 425af4ce 3ef7f659 3d871b88 3f800000 4287b508 c1b3d6c5 42ca21a8 41ec8e56 429c2c97 428544c5 3f800000 428504e8 4242afc3
90: 41bcbac3 427932c6 427c56ed 3ea03a14 3c527f40 3f800000 15000008 6c018000 00008006 313e4000 00000412 313ec000 681e8001 42844cc0 c1f5b458 42c4f370
a0: 4184f4f5 426ed1a8 426ed1a8 3f800000 424b482a 422e34c3 4153f4c8 423e5bfc 4261d590 3e006d3f 3eebcbae 3f800000 4283faf8 c222892b 42c17c78 40ed91b1
b0: 423c3a1a 423c3a1a 3f800000 4223ca54 420fcbf9 40c382df 421ae781 42394cbb 00000000 3f800000 3f800000 4280bdb0 c1f655cc 42ce1de0 41b01824 429e9eb3
c0: 429e9eb3 3f800000 427ce3a5 42483d1f 417ec54c 42657d31 42847638 3eea164a 3eedfd8c 3f800000 427d3d50 c22273a3 42caae68 41278f8a 42848230 42848230
d0: 3f800000 4254709d 42296c14 40f475a9 42415227 425fba23 3eaa6aab 3f7fb512 3f800000 428380e0 c1ef82b6 42d86650 41b2ea0f 429c3582 429c3582 3f800000
e0: 427a79dc 4247cbd4 41828add 4263f3a0 4283e957 3f545544 3ed63d8a 3f800000 4280f030 c21f922d 42d5fe88 41306525 42832060 42832060 3f800000 42533ae5
f0: 42297bec 410198cb 4240ad21 425f7382 3f3e0afb 3f75aea5 3f800000 15000008 6c018000 00008004 313e4000 00000412 313ec000 68148001 42919010 c1bba78e
```

The word histogram itself exposes the overlap: 97 words have high byte `0x42`
(ordinary position floats), 32 `0x3f` (including 26 exact `1.0`), and the
only eight zeros are not table-like sentinels.  Only 14 nonzero values are
less than the file size; the others cannot be file offsets.  `0x651` does not
index this window: neither exact material word is present, nor does index
`0x51`/`0x151` encode a texture pointer.

### Cross-tile negative correlation

The same invalid 256-word window reports 229--252 nonzero values across the
64 tiles (mode 248, 31 tiles), while their distinct PMP materialRef count is
1--21 (mode 2, 27 tiles).  The most common pair is `(248 nonzero, 2 refs)`
for 26 tiles, but it is an overlap artifact, not a correspondence.  Across
the corpus there is no `table[i] <-> distinct-materialRef[i]` relationship:
for 000, 001, 002, 010, 020, and many others the count is 248 vs 2; complex
tiles vary to 10--21 refs while the window still stays near 240--250.  Exact
material-word membership is zero for the simple two-ref tiles.

**Attribute-table hypothesis: rejected (high confidence).**  The table's
claimed base has a deterministic DMA-chain overlap, so it cannot be a table.
The falsifier was direct: a valid cell-0 CNT tag starts at `0x5990`, only
`0x8c` bytes after `0x5904`, whereas a 256-u32 table requires `0x400` bytes.
To find the real table, trace the loader's actual pointer used for the
256-entry relocation and log its runtime address before `vifCallModel`; a
non-overlapping 1024-byte span whose entries are read during the PMP builder
would immediately disprove this rejection.

## Next steps

1. Instrument/break on the EE routine that converts PMP section packets into
   these cell chains; record the source packet address, its materialRef, and
   the emitted five-vector record.  Position duplicates make static matching
   insufficient for a bijection.
2. At that same site, capture the texture-state selection feeding the
   `TOP+0x192` header.  Decode its resulting GIF A+D registers (TEX0/TEX1,
   CLAMP, TEXCLUT) after VIF lane preservation, rather than interpreting the
   60-byte V3 source directly.
3. Correct the loader-note/table-base claim before a native texture mapper
   relies on it.  The authentic per-vertex UV extraction can proceed now,
   independently of material selection.

## Extractor implementation (`extractCellStrips`, 2026-10-16)

`src/native/assets/field_converter.{h,cpp}` now exposes
`extractCellStrips(std::span<const std::uint8_t> tileFile)` returning
`expected<std::vector<FieldCellStrips>, FieldTileError>`.  It parses the
64 per-cell chains directly and is the intended replacement for PMP-packet
extraction when rendering (authentic UVs, colours and material binding).

API summary:

* `FieldVertex { x, height, y; colA[3], colB[3], colC[3]; s, t, alphaAdd; }`
  — one decoded 60-byte record.
* `FieldStrip { gifTag, gifRegs, cell, materialIndex, vertices }` — the
  `@TOP+0` GIF tag quad (low qword + regs descriptor), and the vertices from
  the paired `@TOP+1` upload.  Free helpers decode the tag with the jak
  `gif-tag64` layout: nloop 0..14, eop 15, pre **46**, prim **47..57**,
  flg 58..59, nreg 60..63 (earlier guesses "PRE bit 16 / PRIM bits 17..20"
  are wrong).  Corpus output tags: EOP+PRE set, PRIM `0x27c` (410,635) and
  `0x26c` (7,757), PACKED, NREG 3, regs `0x412` (ST/RGBAQ/XYZF2).
* `FieldCellMaterial { regs[5], regAddrs[5], qwordCount, tex0/tex1/clamp/
  miptbp1 + decoded TEX0 fields, textureIndex, paletteIndex }` — captured
  from `UNPACK V3-32 @TOPS+0x192` as `[data_lo, data_hi, regAddr]` vectors
  (vector 0's third lane is the `0xE` A+D regs descriptor).  NUM=5 headers
  carry TEX1_2(0x15)/TEX0_2(0x07)/CLAMP_2(0x09)/MIPTBP1_2(0x35); NUM=4
  headers carry only the first three (header GIF tag NLOOP 3).  TEX0 fields
  follow jak `gs-tex0`: tbp0@0:14, tbw@14:6, psm@20:6, tw@26, th@30,
  tcc@34, tfx@35, cbp@37:14, cpsm@51, csm@55, csa@56:5, cld@61:3
  (prompt-style guesses "TW 20..23 / CBP 32..45" are wrong).
* `FieldCellStrips { cell, material, materials[], strips[],
  unknownCommands, malformedUploads, nloopMismatches, terminated, truncated }`.

Walk semantics: chains are byte3==`0x10` DMA CNT tags (qwc inline payload,
TTE upper 64 bits = first two VIF codes) terminated by RET `0x60` (qwc 0;
byte3 `0x70` END also accepted).  Verified cmd codes against
`jak-project/common/dma/dma.h`: NOP `0x00`, STCYCL `0x01`, MSCALF `0x15`,
UNPACK V3-32 `0x68`, UNPACK V4-32 `0x6C` — the task-prompt value
"V3-32 = 0x64" is actually V2-32.  These five are the *only* commands in
the whole corpus (plus 6,518 trailing NOPs).  Unknown VIF commands are
skipped word-wise with counted diagnostics; payloads past the chain end stop
the cell gracefully (`truncated`); a zero directory slot or a chain not
rooted at a CNT tag skips the cell without failing the tile.  Cell `i`'s
region ends at the next non-zero directory entry clamped to `offB` — cell
`63`'s successor is directory entry 64, which on `000.BIN` (rel `0x1acc70`)
points *past* `offB`, so the clamp, not the directory, defines that limit.
Material binding resolves TEX0 `TBP0`/`CBP` against `fieldTextureCatalog()`
pages; a failing upload chain leaves links unresolved but extraction
proceeds.

### Pinned disc numbers (probe 2026-10-16, embedded in tests)

* `FLD/000.BIN`: 64 cells, **6,669 strips**, **25,944 vertices**; cell 0 =
  146 strips / 564 vertices (matches the trace above).  Every chain is
  RET-terminated; zero unknown commands, zero malformed uploads, and GIF
  `NLOOP == NUM/5` on all 6,669 strips.
* Corpus (64 tiles): 4,096 cells, **4,090 cells with strips**; the six
  empty cells (`023.BIN` cells 29, 30, 37, 38, 45, 46 — valid CNT chains
  containing only `STCYCL + NOP + RET`) are the only documented misses.
  **418,392 strips**, **1,749,827 vertices**; `MSCALF 8` count equals the
  strip count exactly.
* Binding: first-header TEX0 `TBP0` resolves to a staged tile upload in
  **4,090/4,090 (100%)** material-bearing cells; `CBP` resolves to a
  CLUT-shaped upload in **4,078/4,090 (99.6%)**.  All 12 misses are
  `232.BIN` cells 0/1, 8/9, 16/17, 24/25, 32/33, 56/57 whose `CBP` is
  `0x31b7` — a page the tile uploads as a PSMT8 128x64 *image*, i.e. the
  palette aliases into a texture page rather than a dedicated 16-column
  CLUT transfer.  `000.BIN`: 64/64 and 64/64.

### Corrections to this document's earlier claims

1. **Materials switch mid-cell.**  The grammar's "`@0x192` header once per
   cell" understates reality: each chain carries 1–182 header uploads
   (corpus total **16,463**; `000.BIN` 162, 2–6 per cell), and **3,431**
   corpus cells (all 64 of `000.BIN`) upload a second header at least once.
   Each upload re-arms the WorldStrip material kick, so strips inherit the
   *most recent* header; `FieldStrip::materialIndex` records this.
2. **Cell 0 has three headers**, at tag `0x599c` (TTE), `0xc614` and
   `0xed38`.  The *first* selects TEX0 `0x20051405dd40a880` — PSMT4
   page `0x2880` (128x128), CLUT `0x28a0` — exactly as
   `gif-material-streams.md`'s trace table states.  That doc's "all 64
   header sources … same bits exact for every cell" compact dump
   (`dd40a880`→`9930a85e`, page `0x285e`) actually matches cell 0's *last*
   header (PSMT8 `0x285e` + CLUT `0x286e`), not every cell's only header.
3. **Header coverage is near-total**: 4,090/4,096 cells carry at least one
   `@0x192` header, not the "3,028 of 4,096 (four tiles none)" previously
   reported; no tile lacks headers.  The distinct first-material qword
   records across the corpus number 299.
4. The "zero REF/REFE/NEXT/CALL tags" finding holds: corpus-wide only ids
   `0x10` (4,138 — 42 chains span two CNT frames) and `0x60` (4,096) occur.
