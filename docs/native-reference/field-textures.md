# FLD field textures — upload chain, VRAM placement, material binding (PROVEN)

Updated 2026-10-16. Completes the "texture pages" thread opened in
`field-topology-status.md`: every host-to-local texture upload carried by the
64 `FLD/*.BIN` tiles is parsed, placed, and bound. All numbers below were
verified against the read-only disc extract (`$RT_DISC/FLD`, never copied into
the repo) and are pinned by `tests/native_assets/test_field_converter.cpp`.

## Chain grammar (PROVEN, byte-exact on 3,097/3,097 uploads)

The upload chain occupies `[headerSize, offA)`, where `headerSize` is header
word 0 — `0x20` canonical, `0x160` on `023.BIN`. The chain grammar is
identical on all 64 tiles; the 14 tiles whose declared `fileSize` (word 4)
is smaller than the physical file (trailing padding) parse byte-exact with
the same grammar, so `fileSize` is deliberately unchecked here.

- **Frames.** DMA REF tags (byte 3 `0x10`) with inline payloads of
  `qwc × 16` bytes, terminated by a REF-End tag (byte 3 `0x70`). The
  END-tag-to-`offA` strip is the END payload plus a u16 index/padding block
  (`000.BIN`: END at `0x5080`, strip `0x780`; corpus strips `0xe0..0x7a0`).
- **Uploads.** Each payload is a sequence of FLG0 (packed) GIF A+D packets
  with NREG 1, REGS low nibble `0xE`, NLOOP 1..64, where each data quad is
  `[64-bit value][GS register index]`. A texture upload is one packet
  carrying BITBLTBUF (`0x50`) / TRXPOS (`0x51`) / TRXREG (`0x52`) /
  TRXDIR (`0x53`, DIR = 0) followed by IMAGE-mode (FLG2) packets.
- **Whole-image transfers.** DSAX/DSAY (TRXPOS bits 32–42/48–58) are zero on
  every disc upload; the IMAGE byte total equals `width*height*bpp/8` exactly
  — never padded — on all 3,097 uploads.
- **Register packing.** The game packs BITBLTBUF as `dbw<<48 | dbp<<32 |
  psm<<56` and TRXREG as `h<<32 | w` (fields land in the value qword's upper
  half; byte-level layout from disc, not the raw GS register layout).

## Inventory (all 64 tiles)

**3,097 uploads = 1,685 textures + 1,412 CT32 CLUTs.**

| kind | count | top shapes |
|---|---|---|
| PSMT4 (0x14) | 1,210 | 64×64 ×539, 32×32 ×175, 128×128 ×166, 64×32 ×145 |
| PSMT8 (0x13) | 468 | 64×64 ×132 |
| PSMCT24 (0x01) | 7 | (rare, large) |
| PSMCT32 (0x00) CLUT | 1,412 | 16×2 ×1,061, 16×16 ×351 |

Per-tile uploads range from 2 (`201.BIN`) to 292 (`013.BIN`).
1,350/1,685 textures are directly followed by their palette; the remaining
335 share an earlier palette and resolve through the material block's CBP.
`psm` is kept as the raw GS value — proven inventory is only {0x00, 0x01,
0x13, 0x14}.

## GS VRAM placement (PROVEN 64/64)

Per tile, the minimum destination page is exactly

```
minDbp = 0x285E + (filenameLastDigit * 0x420)   // pages (256-byte blocks)
```

verified on 64/64 tiles. The chain carries **absolute** page numbers with no
runtime rebase: Ghidra's `loadFieldTile` chain walk (case 2) is a pure walk +
`sendTexturePackets`, and the DBP-rewriting variant `SendTexturePacketsDMA`
@ 0x20E030 (mask `0xffffc000ffffffff`, bits 32–45) is used only for misc /
COURSE chains, not FLD tiles.

The `0x420` stride overflows the next tile's base only for the eight z=3
tiles (013, 023, 103, 113, 203, 213, 223, 233): their spans reach
`0x4ef..0x7f2` pages past the base, ending at most at page `0x3CB0` — inside
VRAM and past every other tile's working set, so absolute addressing is safe.

**CLUT pairing rule:** a texture's own palette is the CLUT upload whose page
starts exactly where the texture's pages end — `clutDbp == dbp +
ceil(dataBytes/256)`. Example `000.BIN` (10 uploads, pages `0x285e..0x28a9`,
18,176 texture + 1,280 palette bytes = 19,456): T8 64×64 @`0x285e` + CT32
16×16 @`0x286e`; T4 64×64 @`0x2877` + CT32 16×2 @`0x287f`; T4 128×128
@`0x2880` + CT32 16×2 @`0x28a0`.

## Material binding (PROVEN, from cell VIF streams)

All 65 directory targets are raw VIF streams (their leading word is FLUSHE —
the old "targets start with a DMA tag" observation was a byte-3 `0x10`
coincidence, see the correction below). Inside them, MATERIAL blocks are
`UNPACK V3-32 @ 0x192` carrying 12-byte `{lo, hi, reg}` GIF A+D vectors for
TEX0_2 (`0x07`), CLAMP_2 (`0x09`), TEX1_2 (`0x15`), MIPTBP1_2 (`0x35`);
batches open with a UNPACK V4-32 n1 header at address 0, vertices are UNPACK
V3-32, and VU1 programs are selected by MSCALF.

Corpus: **7,264 material records; 497 distinct `(tbp, psm)` bindings.**

- TW/TH — TEX0 **bits 26–29 / 30–33** (not the usual 32–39/48–55, because
  the `{lo,hi}` vector is packed at 32-bit granularity around the TW/TH
  field positions) match an upload's dimensions on **497/497** bindings.
- CBP matches a CLUT upload's `dbp` in the same tile on **494/497**; the
  three misses sit on 013, 113, 232 and likely resolve through palettes
  uploaded by neighbouring chains.
- Bound TEX0s show a constant CLD = 2 quirk.

`000.BIN`: 162 material blocks, 3 bindings — T4 128² @`0x2880` + clut
`0x28a0` ×90, T8 64² @`0x285e` + `0x286e` ×64, T4 64² @`0x2877` +
`0x287f` ×8. **Texture selection therefore happens per batch inside the cell
VIF streams, not per PMP packet.**

## Pixel decode conventions (PROVEN)

- **Palette byte order: byte 0 = R.** Car-paint palettes identify the
  convention: byte0=R yields 408 yellow + 1,492 warm entries vs 134/72 when
  swapped, matching OpenKh's `X8B8G8R8` CLUT
  (github.com/ninjadynamics/PS2Docs ps2textures.md). Alpha bytes are in
  {0x00, 0x80}.
- **CSM1 CLUT swizzle is required** (GS CSM1 bit 3/4 exchange): swizzled
  neighbor-difference 6.2 vs 24.0 unswizzled. Same `loadClut` convention as
  `image_converter.cpp`.
- **T4 nibble order: low nibble first** (1,007/1,025 smoothness wins).

## `materialRef` — still OPEN

The PMP packet header word 3 (`materialRef`, e.g. `0x100651`) is **not** a
texture index. Disproven hypotheses (all corpus-checked):

1. directory-relative offset — `0x100651 + offA` lands mid-stream, not at
   any tag/MAT offset and not 16-aligned;
2. attribute-table index — values exceed 256;
3. PMP-relative offset — exceeds the PMP span;
4. per-tile digit/texture index — `201.BIN` carries ref `0x313` with only 1
   texture in its chain, and `000.BIN`/`222.BIN` share `0x100651` despite
   different chain populations;
5. loader rewrite — case 3 relativizes only header words [1..65] and the PMP
   block's words [0..255] (magic + section table); word 3 is never touched.

Observed facts: 48 distinct values corpus-wide (29 ignoring bit 31), hex
digits only {0..6}; dominated by `0x100651` ×252,623 and `0x80100651`
×8,779; near-constant per tile (`000.BIN`: 5,926 vs 259 split). The working
candidate is a material key consumed by the terrain draw path (the MATERIAL
blocks above carry the actual GPU state); resolving its dictionary needs the
VU1 terrain program trace (`vu1-programs.md`).

## Corrections to `field-topology-status.md`

- **"256-entry attribute table"**: the loader relativizes only the PMP block's
  words [0..255] (magic + section table), so the earlier "248 attributes"
  count at `offA + 65*4` counted cell-0 stream payload. The physical region at
  `offA + 260` holds ~32 non-zero u32 before cell 0's stream begins at
  `offA + 0x190` (`000.BIN`: `0x5904..0x5984`); as u16 pairs these look like
  32 table entries, semantics unresolved.
- **Directory targets** begin with FLUSHE, not a DMA tag (byte-3 coincidence).
- **Packet `materialRef`** does not bind textures per packet (see above).

## Native API

`src/native/assets/field_converter.{h,cpp}` exposes

```cpp
expected<std::vector<FieldTexture>, FieldTileError>
fieldTextureCatalog(std::span<const std::uint8_t> tileFile);
```

returning per-upload `FieldTexture` descriptors
(`tagOffset/gifOffset/dataOffset/dataBytes/dbp/dbw/psm/width/height/
clutOffset/clutDbp/isClut`). Malformed structure → `ChainDecode`, payload or
file overrun → `Truncated`, implausible header → `BadHeader`. Tests pin the
grammar (`testTextureCatalogSyntheticChain`/`testTextureCatalogErrors`) and
the disc facts (`testTextureCatalogRealTiles`: 64/64 tiles, 3,097/1,685/1,412
counts, the per-tile base-page rule, and the `000.BIN` descriptor set); they
skip cleanly when no disc extract is present.
