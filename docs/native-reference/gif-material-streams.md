# Field GIF material streams — WorldStrip state and tile pages

Updated 2026-10-16. This pass uses the read-only SLUS-20398 disc extract.
Offsets are file offsets in `FLD/000.BIN`, unless stated otherwise. No game
payload is retained in this repository; five *derived*, grayscale indexed-page
inspection images were written under `/tmp/rt-tex/` only.

## Result

**PROVEN:** texture state reaches the GS through hypothesis **(a)**, but at
the **per-cell** cadence rather than per strip: the initial `UNPACK V3-32,
NUM=5,@TOPS+0x192` is a five-qword GIF A+D packet. WorldStrip tests its
NLOOP, `XGKICK`s that packet once, then clears NLOOP and XGKICKs every strip
packet. The packet sets `TEX1_2`, `TEX0_2`, `CLAMP_2`, and `MIPTBP1_2`.

The first material in `000.BIN`, for example, selects the PSMT4 image uploaded
to GS page `0x2880` (128x128), and its TEX0 CLUT base is `0x28a0`; those are
exactly the destination bases of the staged 128x128 PSMT4 and 16x2 PSMCT32
GIF IMAGE transfers. Thus the texture-page association is direct and does not
depend on matching PMP positions or `materialRef`.

**FALSIFIED:** no VIF `DIRECT`/`DIRECTHL` command occurs in any of the 64
`000.BIN` cell chains. The pre-directory region is not a VIF-DIRECT stream:
it is ten ordinary DMA CNT packets containing GIF tags, packed A+D setup
writes (`BITBLTBUF`, `TRXPOS`, `TRXREG`, `TRXDIR`), and GIF IMAGE payloads.

## WorldStrip output GIF stream (PROVEN)

The strict `vu-dis` listing of entry 4 (the image slice beginning at VU byte
`0x0080`) shows the relevant stores in this order:

```text
sq.xyzw vf27, -3(vi07)       # STQ
sq.xyzw vf28, -2(vi07)       # RGBAQ
sq.xyzw vf29, -1(vi07)       # XYZF2
...
xgkick vi13                  # @TOP + 0x192 material A+D packet, if NLOOP != 0
xgkick vi02                  # @TOP + 0xfa output strip packet
iswr.x vi00, vi13            # clear material GIF-tag NLOOP
```

`vi02 = TOP + 0xfa`; before the vertex loop the program copies the uploaded
`TOP+0` qword (the strip GIF tag) to `TOP+0xfa`. It then emits exactly three
qwords per submitted vertex. Therefore a V4 upload at `TOP+0` whose GIF tag
has `NLOOP = vertexCount`, `FLG=PACKED`, `NREG=3`, and descriptors
`ST(2), RGBAQ(1), XYZF2(4)` produces this stream:

```text
GIFtag(NLOOP=vertexCount, EOP=1, PRE=1, PRIM=0x27c, PACKED, ST/RGBAQ/XYZF2)
  repeat vertexCount:
    STQ   = (s*Q, t*Q, Q, carried-w)
    RGBAQ = FTOI0(colour result)
    XYZF2 = FTOI4(screen result, fog), with ADC as required
```

Cell 0 has 146 such groups. Its first three source tags are respectively
`NLOOP=6`, `6`, and `4`; all decode to `EOP=1, PRE=1, PRIM=0x27c,
FLG=0 (PACKED), NREG=3, regs=0x412` (low-nibble first: ST, RGBAQ, XYZF2).
The stream has no texture register writes, so its texture state is the header
kick described below.

## The `@0x192` header is GIF A+D state (PROVEN)

V3-32 writes only XYZ. At qword `TOP+0x192`, the pre-initialised W lanes
complete a GIF tag and the A+D register-address qwords. The five source
vectors are consequently not floating material coefficients. They are:

| V3 vector | assembled GIF content | `000` source XYZ words / result |
|---:|---|---|
| 0 | GIF tag (W pre-existing) | `00008004 10000000 0000000e` -> PACKED A+D, NLOOP 4 |
| 1 | A+D | `TEX1_2 (0x15) = 0x00000f8000080128` |
| 2 | A+D | `TEX0_2 (0x07) = 0x20051405dd40a880` |
| 3 | A+D | `CLAMP_2 (0x09) = 0x0000000000000000` |
| 4 | A+D | `MIPTBP1_2 (0x35) = 0x2000000a8a90a8a1` |

The VU listing independently proves the first word is read as an integer
(`ilwr.x vi04, TOP+0x192`) and used solely as the header kick guard. It is
cleared after `XGKICK`; this explains why the state packet is sent once per
cell invocation, not once per submitted strip.

### `000.BIN`: all 64 header sources

The requested 15 values are best dumped as their 32-bit lanes: interpreting
them as IEEE floats yields mostly denormals and is not a material-float
representation. All cells have the same 15-lane source except CLAMP's first
lane (lane 9). This compact dump is exact for every cell:

```text
lanes  0..14 (u32):
00008004 10000000 0000000e  00080128 00000f80 00000015
9930a85e 20050dc5 00000007  CLAMP 00000000 00000009
8760a872 2000000a 00000035

same bits viewed as f32:
4.59234e-41 2.52435e-29 1.96182e-44  7.35099e-40 5.56035e-42 2.94273e-44
-9.13299e-24 1.12701e-19 9.80909e-45  (CLAMP) 0 1.26117e-44
-1.69014e-34 1.0842e-19 7.42688e-44
```

| cells | lane 9 / `CLAMP_2` | WMS,WMT interpretation |
|---|---:|---|
| `0,1,3--9,11,12,14--18,20--26,28--39,41--63` | `0x0` | repeat, repeat |
| `2,13,19,27,33` | `0x5` | clamp, clamp |
| `10` | `0x1` | clamp, repeat |
| `40` | `0x4` | repeat, clamp |

So the one-word cell-2 difference previously observed is a real sampler-wrap
variant. It is not a texture index or UV scale.

Across the 64 tiles, 3,028 of 4,096 cells had a header (four tiles have no
matching WorldStrip header); there are 135 distinct complete A+D records.
Distinct-records-per-tile histogram: `0:4, 1:4, 2:5, 3:8, 4:37, 5:2, 6:3,
8:1`. TEX0 varies per record while the strip GIF tag does not, establishing
cell material selection without an EE-side PMP correlation.

### TEX0 page correlation

For the common `000` record, decoding TEX0 gives:

```text
TBP0=0x2880, TBW=2, PSM=0x14 (PSMT4), TW=7, TH=7 (128x128),
TCC=1, TFX=0, CBP=0x28a0, CPSM=0 (PSMCT32), CSM=0, CSA=0, CLD=1.
```

The two matching staged transfers are `[0x2440,0x44b0)` to DBP `0x2880`
(128x128 PSMT4) and `[0x44b0,0x45a0)` to DBP `0x28a0` (16x2 PSMCT32).
This is an exact base/page/format match. The native renderer should treat
`TBP0 + PSM + TW/TH + CBP/CPSM/CSM/CSA` as the texture identity, not just the
base, because multiple uploads and palettes can share a tile.

## Tile GIF IMAGE pages (PROVEN)

Each texture transfer is one DMA CNT packet: a four-loop PACKED A+D setup,
then one IMAGE GIF tag. In `000`, the first packet is `[0x20,0x1090)`: setup
at `0x30`, image tag at `0x80`, and 256 IMAGE qwords. Its `TRXREG` is 64x64
and `BITBLTBUF.DPSM=0x13` (PSMT8), so the 4,096-byte payload is complete.

`000` inventory (10 IMAGE transfers): PSMT8 pages 64x64, 32x32, and 16x16;
PSMT4 pages 128x128, two 64x64, and 32x32; one 16x16 PSMCT32 CLUT; and two
16x2 PSMCT32 palette transfers. The latter are palette records, not texture
pages. Five `/tmp/rt-tex/*-p8-*.png` samples are deliberately grayscale
linear-index diagnostics; they prove dimensions/payload extent, but are not
final visual decodes because GS PSMT8/4 swizzling and CLUT addressing still
need emulation.

Corpus total: **3,097** GIF IMAGE transfers: PSMT8 `467`, PSMT4 `1,541`,
PSMCT32 `1,084`, PSMCT24 `5`. Size/format count is bounded by the transfers
below (the PSMCT32 16x2 entries are the frequent small palettes):

| destination format | image sizes and counts |
|---|---|
| PSMT8 (`0x13`) | 16x16:57, 32x16:16, 32x32:69, 64x32:30, 64x64:132, 64x128:6, 128x64:61, 128x128:72, 128x256:1, 256x128:19, 256x256:1, 64x256:4 |
| PSMT4 (`0x14`) | 16x16:1, 32x16:11, 32x32:175, 32x64:9, 32x128:3, 32x256:1, 64x32:145, 64x64:539, 64x78:1, 64x128:17, 64x256:19, 128x32:2, 128x64:102, 128x128:166, 128x256:9, 256x128:9, 256x256:2 |
| PSMCT32 (`0x00`) | 16x2:1,061; 16x16:351 |
| PSMCT24 (`0x01`) | 32x32:1, 32x64:1, 64x32:1, 64x64:3, 128x128:1 |

Per-tile transfer inventory (`p8/p4/c32/c24` are destination PSM classes,
including palette uploads in c32):

| tile | p8 | p4 | c32 | c24 | all |
|---|---:|---:|---:|---:|---:|
| `000` | 3 | 4 | 3 | 0 | 10 |
| `001` | 3 | 4 | 3 | 0 | 10 |
| `002` | 3 | 4 | 3 | 0 | 10 |
| `003` | 6 | 4 | 6 | 0 | 16 |
| `010` | 3 | 4 | 3 | 0 | 10 |
| `011` | 9 | 16 | 21 | 0 | 46 |
| `012` | 12 | 32 | 34 | 0 | 78 |
| `013` | 10 | 138 | 143 | 1 | 292 |
| `020` | 7 | 4 | 7 | 0 | 18 |
| `021` | 6 | 4 | 6 | 0 | 16 |
| `022` | 15 | 36 | 43 | 0 | 94 |
| `023` | 9 | 117 | 124 | 0 | 250 |
| `030` | 6 | 4 | 6 | 0 | 16 |
| `031` | 3 | 4 | 3 | 0 | 10 |
| `032` | 6 | 3 | 5 | 0 | 14 |
| `033` | 3 | 4 | 3 | 0 | 10 |
| `100` | 3 | 4 | 3 | 0 | 10 |
| `101` | 3 | 4 | 3 | 0 | 10 |
| `102` | 10 | 9 | 15 | 0 | 34 |
| `103` | 13 | 51 | 60 | 0 | 124 |
| `110` | 9 | 25 | 29 | 0 | 63 |
| `111` | 13 | 22 | 29 | 0 | 64 |
| `112` | 7 | 4 | 11 | 0 | 22 |
| `113` | 15 | 81 | 91 | 2 | 189 |
| `120` | 10 | 28 | 32 | 0 | 70 |
| `121` | 7 | 13 | 14 | 0 | 34 |
| `122` | 3 | 4 | 3 | 0 | 10 |
| `123` | 3 | 4 | 3 | 0 | 10 |
| `130` | 6 | 4 | 6 | 0 | 16 |
| `131` | 3 | 4 | 3 | 0 | 10 |
| `132` | 3 | 4 | 3 | 0 | 10 |
| `133` | 3 | 4 | 3 | 0 | 10 |
| `200` | 8 | 7 | 11 | 0 | 26 |
| `201` | 0 | 1 | 1 | 0 | 2 |
| `202` | 12 | 13 | 21 | 0 | 46 |
| `203` | 13 | 64 | 77 | 0 | 154 |
| `210` | 7 | 38 | 45 | 0 | 90 |
| `211` | 14 | 8 | 16 | 0 | 38 |
| `212` | 10 | 24 | 30 | 0 | 64 |
| `213` | 19 | 43 | 58 | 0 | 120 |
| `220` | 27 | 29 | 47 | 0 | 103 |
| `221` | 20 | 21 | 36 | 0 | 77 |
| `222` | 13 | 14 | 23 | 0 | 50 |
| `223` | 15 | 127 | 137 | 3 | 282 |
| `230` | 6 | 3 | 5 | 0 | 14 |
| `231` | 3 | 4 | 3 | 0 | 10 |
| `232` | 6 | 18 | 20 | 1 | 45 |
| `233` | 12 | 79 | 87 | 0 | 178 |
| `300` | 3 | 4 | 3 | 0 | 10 |
| `301` | 8 | 7 | 11 | 0 | 26 |
| `302` | 3 | 4 | 3 | 0 | 10 |
| `303` | 3 | 4 | 3 | 0 | 10 |
| `310` | 9 | 7 | 12 | 0 | 28 |
| `311` | 3 | 4 | 3 | 0 | 10 |
| `312` | 3 | 4 | 3 | 0 | 10 |
| `313` | 3 | 4 | 3 | 0 | 10 |
| `320` | 3 | 4 | 3 | 0 | 10 |
| `321` | 8 | 4 | 8 | 0 | 20 |
| `322` | 3 | 4 | 3 | 0 | 10 |
| `323` | 3 | 4 | 3 | 0 | 10 |
| `330` | 7 | 4 | 7 | 0 | 18 |
| `331` | 3 | 4 | 3 | 0 | 10 |
| `332` | 3 | 4 | 3 | 0 | 10 |
| `333` | 3 | 4 | 3 | 0 | 10 |

## Native converter contract and remaining gaps

Expose a tile-level GS upload replay plus a cell material key, rather than
attempting to bake an unproven `materialRef` mapping:

```cpp
struct FieldGsImage {
  uint16_t dbp; uint8_t dbw, dpsm;
  uint16_t width, height;
  std::vector<uint8_t> image_qwords;  // retained/decoded externally; swizzle-aware
};
struct FieldSamplerState {
  uint64_t tex0, tex1, clamp, miptbp1;
  TexturePageId page; PaletteId palette;  // resolved from TEX0 TBP0/CBP
};
struct FieldStrip {
  std::span<const FieldVertex> vertices;  // streamed s,t,alphaAdd and colours
  FieldSamplerState material;             // inherited from owning cell
};
FieldTileMaterials extractFieldGifMaterials(TileBytes);
```

`extractFieldGifMaterials` must replay each pre-directory GIF IMAGE transfer
into a GS-page map keyed by DBP/PSM, then resolve each cell TEX0's `TBP0` and
`CBP`. Every WorldStrip group in that cell inherits the same sampler state;
its GIF tag identifies only primitive packing, not a material switch.

**OPEN:** the first implementation may use the exact page/palette relation
above, but must not call grayscale linear-index output an authentic texture
decode. Remaining work is GS memory-layout swizzling for PSMT4/PSMT8, CLUT
CSM/CSA permutation, and PSMCT24 handling. A falsifier for the established
cell->page mapping would be a valid header TEX0 whose `TBP0/PSM/TW/TH` cannot
be resolved to a preceding staged IMAGE transfer after replaying the tile's
GIF packets. None of the 3,028 parsed headers produced that evidence in this
pass.
