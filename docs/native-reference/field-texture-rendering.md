# Field texture rendering: PS2 decode, sampling verdicts, preview integration

Companion to `field-textures.md` (upload inventory) and `gif-material-streams.md`
(material packets). This documents the authentic-texture path added in
`decodeFieldTextures()` / `buildFieldTexturedTerrain()` (src/native/assets/
field_converter.{h,cpp}) and the textured Vulkan pipeline in
src/native/preview/preview.{h,cpp}, with the numbers that pin each verdict.

## The swizzle verdict: decode the payload, not VRAM

GS VRAM stores PSMT8/PSMT4 block-swizzled: page = 8 KiB; PSMT8 pages are
128x64 px of 16x16-px blocks and PSMT4 pages 128x128 px of 16x32-px blocks
(PCSX2 `GSTables.cpp` `_blockTable8`/`_blockTable4` + `columnTable8/4`,
cached reads at /tmp/rt-tex-render/GS{Tables,State}.cpp). **None of that
applies to the tile files.** A GIF IMAGE transfer payload is linear row-major
within its box; the GS writes it into VRAM with the swizzle, and the sampler
later walks the *same* swizzle to fetch texels. The two permutations compose
to identity, so decoding straight from the payload byte stream reproduces
exactly what the sampler sees.

Adjudication against the validated oracle (`build/macos-parity2/rt_texture_extract`, 5,024 PNGs over all dirs):

- FLD/003.BIN, four indexed pages: **45,056/45,056 pixels RGB-exact** with
  payload-linear pixels; applying the VRAM swizzle to the payload instead
  scores **1.07%** on the same images.
- Whole-corpus oracle join (`/tmp/rt-tex-render/oracle-map.json`, keyed
  `tagOffset + 16 == sources.csv offset`, indexed pages only, palette
  variants excluded): 435 pages across 23 tiles, **2,424,925 / 2,469,760
  RGB-exact = 98.18%**. The entire 44,835-px residual is three tiles —
  023.BIN 12,067, 203.BIN 8,192, 213.BIN 24,576 — pages whose palettes are
  only bound through a *preceding* shared CLUT that the material CBP does not
  name with a CLUT-shaped upload (e.g. 203.BIN tag 0x91C0 dbp 0x3555 with
  CBP 0x354B); these pages are **never referenced by terrain strips**
  (corpus unresolved-triangle count is 0), so preview terrain is unaffected.
  The oracle additionally emits trailing uploads beyond `offA` (13 joined
  pages across 12 tiles, e.g. 011.BIN csv offset 0x2ABC80 vs `offA`
  0x1E800) — MODE2-region uploads our [headerSize, offA) catalogue
  intentionally excludes.

Pixel-order pins inside the payload (all above, plus `test_field_converter`
synthetic fixtures): PSMT4 unpacks **low nibble first**; CLUT8 word position
is `swapBits34(csa * 16 + entry)` — bits 3 and 4 of the flat entry index
swapped (`(e & ~0x18) | ((e & 0x08) << 1) | ((e & 0x10) >> 1)`; see
`swappedClutWordPos` in field_converter.cpp, reference Extract.cpp:139);
PSMT8 bank selection clamps to the last bank
(`min(raw >> 4, 15) * 16 + (raw & 15)`).

## Colour and alpha conventions

Word layout byte0=R, byte1=G, byte2=B, byte3=A (oracle-exact, 003.BIN).
GS alpha runs 0..128 (0x80 = opaque); host RGBA8 receives
`min(255, a * 255 / 128)` (Extract.cpp:346). PSMCT24 uploads carry no alpha:
AEM makes an all-zero RGB entry transparent (alpha 0) and everything else
opaque 0x80 → 255 (Extract.h:20). Pages with no resolvable palette decode to
grayscale fallback (index, index*17 for PSMT4) at alpha 0x80 — counted as
CLUT misses.

## Corpus decode stats (64 FLD tiles, current code)

| metric | value |
|---|---|
| texture pages (non-CLUT uploads) | 1,685 |
| CLUT attached via trailing tag or material CBP | 1,384 (82.1%) |
| CBP aliases onto a non-CLUT upload | 0 |
| no palette (grayscale fallback) | 294 (17.4%) |
| indexed texels | 10,678,912 |
| indexed texels palette-resolved | 9,967,488 (93.3%) |
| terrain triangles after strip expansion | 913,043 |
| triangles with unresolved page id | **0** |
| vertices | 1,749,827 (cells with strips 4,090 / 4,096) |
| deduplicated pages by content (grid mode) | 579 of 1,685 |

Per-tile pins: 000.BIN 7 pages / 3 hits / 4 miss / 30,976 idx px /
12,606 tris; 003.BIN 10 / 6 / 4 / 59,648 idx px / 12,844 tris;
232.BIN 25 / 18 / 0 alias / 6 miss / 6,919 tris / 14 distinct material
textures.

### The 232.BIN CBP alias, refined

The earlier corpus audit flagged 12 cells whose TEX0 CBP = 0x31B7 named a
*page* instead of a CLUT. Cell-level inspection (p232 probe): cells 0/1 bind
tbp0 0x30B7 = a **PSMCT24 128x128** texture — direct colour, no palette —
with CBP 0x31B7, which is the dbp of a real *texture* page (PSMT8 128x64)
later in the chain. Every genuinely indexed page in 232.BIN carries its own
trailing CLUT instead. So the alias exists in the bindings but is inert:
the decode reports `clutAliasHits = 0` corpus-wide, and the CBP-alias attach
path remains as defence for other content.

## Strip topology

GIF PRIM 0x27c: EmuTC `ps2tek` bits 0..2 = 4 → **TRI_STRIP** (+ gouraud,
texture, fog, alphablend, ctx2); bit 8 = 0 ⇒ STQ coords.
`buildFieldTexturedTerrain` expands consecutive triples `(j-2, j-1, j)`.
A tile's cells may switch material mid-chain (each @0x192 header re-arms the
next WorldStrip); triangle page id comes from the strip's bound material's
catalogued texture upload (`tex0Tbp0 == dbp`).

## UV semantics

The V4-32 vertex stream carries `(s, t, alphaAdd)`; 1.0 of s/t spans one
texture page (PCSX2 `GSState.cpp` `GetTexCoordsImpl` ~5108: texel =
st / q * 2^TW|TH). The q slot is used as **additive alpha**: measured across
the corpus, alphaAdd == 1.0 for 1,749,258/1,749,827 vertices (99.97%, the
rest 0.0/0.999), so the stored s/t already are the page fractions the sampler
uses, and the raw values are streamed to the GPU unchanged. Corpus range:
u ∈ [-39.500175, 39.0], t ∈ [-21.320000, 21.002501]; 50.6% of vertices sit
inside the unit page — the rest rely on REPEAT wrapping at integer bounds
(measured over all 1,749,827 textured vertices). Cell-0 vertex-0 pin
(000/003.BIN, stream offset 0x5a24): (0.13528818, 0.95435876, 1.0).

## Preview integration (RT_APP_TEXTURES)

`RT_APP_TEXTURES=1` (with `RT_APP_TERRAIN=1`) switches the terrain overlay to
authentic textures (src/native/app/main.cpp `loadTerrainMesh`):

- Grid mode places tile id at x = (id&0xF)*80 − 640, z = (id>>4)*160 − 240
  render units, **positions offset, UVs local**; pages dedupe across tiles by
  content hash (579 unique of 1,685 uploads).
- `TerrainMesh` gained `uvs` (parallel to positions), `textures`
  (`PreviewTexture{width,height,rgba}`) and `triangleTexture` (per-triangle
  page id, −1 = unresolved). `isTextured()` gates the new path; the old
  height-banded flat path is untouched when textures are absent (regression
  snapshot byte-same at 201 distinct colours).
- New pipeline (preview_textured.vert/.frag, SPIR-V appended to
  preview_shaders.h; existing arrays byte-identical): pos3+colour3+uv2,
  same 144-byte push-constant block. Perspective-correct UV by carrying
  `uv * gl_Position.w` and dividing in the fragment stage. Fragment samples
  `sampler2D` binding 0 (VK_FILTER_NEAREST, REPEAT/REPEAT — GS-like),
  multiplies the decode's RGBA into the flat lighting+fog model of
  preview.frag, and discards texels with alpha < 0.02 (AEM-empty entries).
- Draws are page-batched: triangles bucketed by page slot (slot = id + 1,
  slot 0 = internal 1×1 white dummy for unresolved), one indexed
  `vkCmdDrawIndexed` per slot inside the single render pass; underlay, road
  ribbon and car keep the flat pipeline. Page images (VkImage, R8G8B8A8) are
  uploaded once and cached by content hash for the renderer lifetime.

Verification snapshots (800x600, PLAYING frame run): `/tmp/rt-textured.ppm`
distinct colours **1,808**, ground-row green levels **43** vs banded
`/tmp/rt-world2.ppm` / `/tmp/rt-bandcheck.ppm` **201 / 8** — the flat height
bands are replaced by real texel detail. Note the two runs are not
frame-locked (real-time dt drifts the camera); the colour-diversity metrics
are the comparison, not pixel equality.

## Known gaps / deferred

1. Pages whose palette is only reachable via a preceding shared CLUT that no
   material CBP names with a CLUT-shaped upload decode to grayscale
   (294/1,685 pages, 6.7% of indexed texels). None are sampled by terrain
   triangles today.
2. One page (023.BIN tag 0x1C3C0, 128x32 PSMT4) resolves via material CBP to
   a CLUT but matches the oracle on only 221/4,096 px — palette bank selection
   for that binding needs a dedicated pass (unused by strips in nearby cells).
3. Multi-bank textures: each page bakes the CSA of the *first* material that
   names it; the oracle emits `palette-variants/*` for csa=1 second banks.
   A T4 page bound from two banks would need variant pages.
4. Trailing uploads beyond `offA` (13 oracle pages across 12 tiles, e.g.
   011.BIN 0x2ABC80) are outside the catalogue region and undecoded (MODE2
   object textures).
5. Preview sampling assumes page row 0 = image top (PNG convention); the GS
   v-axis direction for terrain decals is not independently pinned on a
   reference display.
6. alphaAdd drives texture modulation/addition in the hardware (GFS ALA);
   the preview modulates by white vertex colour and ignores additive alpha.

## Where the pins live

- `tests/native_assets/test_field_converter.cpp`:
  `testDecodeSyntheticExactRgba` (exact RGBA for T8/T4/CT24/CT32 + miss,
  bits34 permutation, nibble order, GS alpha scale, AEM),
  `testTexturedTerrainSyntheticEmpty`, `testFieldTextureDiscPins`
  (000/003/232 counts above, byte-exact page pixels, cell-0 UV pin).
- Oracle tooling (temp, not in repo): `/tmp/rt-tex-render/oracle_check.cpp`
  + `oracle-map.json` — reruns the corpus RGB-exact comparison.
- App smoke: `RT_APP_TERRAIN=1 RT_APP_TEXTURES=1 RT_APP_NORENDER=1
  RT_APP_NOAUDIO=1 RT_APP_FRAMES=2` → `tiles=64 triangles=913043
  pages=579 clut hits/alias/miss=1384/0/294 unresolved=0`.
