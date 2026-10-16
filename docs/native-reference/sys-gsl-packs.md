# SYS/ GSL packs — DMA-stream UI texture containers (fonts, HUD, menus)

Updated 2026-10-16. Characterizes the `.GSL` container family (`SYS/*.GSL`, and the same
format in `ITEM/{METER0..10,PARTS}.GSL`), the ELF loader that consumes them, and the
FONT/FONTE glyph atlas. All numbers verified against the read-only disc extract
(`~/Library/Application Support/RoadTripRecomp/disc`, never copied into the repo); probe
scripts ran from `/tmp`, no game bytes committed. Complements [`asset-map.md`](asset-map.md)
(which proves *where* these files live on disc and which descriptor-table entry selects
them) and [`field-textures.md`](field-textures.md) (the FLD upload chain, whose packet
grammar this family shares).

**Headline (PROVEN): a .GSL file is not a TOC container at all — it is a serialized PS2 DMA
packet stream, dumped whole from a PC build tool.** No magic, no entry table, no filenames.
The game validates the DMA tag chain, optionally rebases one VRAM field, then points DMA
channel 2 (GIF) at the buffer and lets the hardware consume the file directly. 49/49 files
(37 SYS + 12 ITEM) walk cleanly to the END tag.

---

## 1. Container grammar (PROVEN, 49/49 files)

### Chain walk (exactly what `FUN_0020d120` executes)

```
cur = 0
loop:
  b = file[cur+3]
  b == 0x70  -> END of file (DMA END tag, QWC < 0x100)
  b != 0x10  -> fatal("GSL DMA Tag Error!")   ; 0x10 = CNT tag (ID=1) with QWC < 0x1000
  qwc        = u16le @ cur
  payload    = [cur+16, cur+16 + qwc*16)      ; inline (CNT mode), exact span
  cur       += (qwc+1)*16                     ; tag itself + payload
```

The tag qword 0 decodes as `QWC[15:0] | ID[31:28]`: every unit is **CNT** (inline data),
never REF, so the tool-pointer junk in tag qword 1 (`0x0012ff6c`-class addresses) is never
dereferenced — the GIF consumes everything inline.

### Unit layout (offsets from unit start `T`)

| offset | size | content |
|---|---|---|
| `T+0`  | 16 | DMA CNT tag: `u16 QWC`, byte 3 = `0x10`; qword 1 = tool junk (ignored) |
| `T+16` | 16 | tool junk — serialized C++ object-header remnant (`04 00 00 00` + host pointers). Part of the GIF payload stream but field-masked into harmlessness |
| `T+32` | 64 | four GIF A+D packed quads `{u64 value, u32 reg}`: regs `0x50 BITBLTBUF`, `0x51 TRXPOS`, `0x52 TRXREG`, `0x53 TRXDIR` |
| `T+96` | 16 | GIF IMAGE-mode tag (FLG = 2), `NLOOP = image_bytes/16`; hi qword = tool junk (ignored) |
| `T+112`| `NLOOP*16` | raw image data; span = `NLOOP·16` ≥ `ceil(w·h·bpp/8)`. Byte-exact for all but **four PSMT4 units** whose non-quad-aligned logical sizes (`SELECT` 200×150, `METER4` 40×150 + 140×36, `METER5` 28×55) round up through NLOOP with <16 B overhang (the qwc invariant is unaffected) |

**Invariant (asserted on all 1142 units): `qwc * 16 == 96 + NLOOP*16`.**

The "P/Q/R/S" letters visible in hexdumps are not tags or magic — they are the **GS register
IDs** `0x50/0x51/0x52/0x53` (BITBLTBUF/TRXPOS/TRXREG/TRXDIR) of the A+D quads. The quads
carry the real packed fields in the meaningful bit positions; the tool left host pointers in
the bits the GS masks off (e.g. a `0x00872a5c`-class heap pointer under BITBLTBUF's
source-block bits, which are unused for host→local uploads).

### Texture / CLUT pairing

Every texture unit is immediately followed by its palette unit (surplus count noted below):

| texture PSM | palette unit | palette size |
|---|---|---|
| `0x14` PSMT4 | `0x02` PSMCT16 (`16×2` small sprites, `16×16` fonts) **or** `0x00` PSMCT32 `16×2` | 64 B / 512 B / 64 B |
| `0x13` PSMT8 | `0x00` PSMCT32 `16×16` | 1 KB |

(a 16-entry palette is `16×2` in CT32 or `16×2`…`16×16` in CT16 — TRXREG just sizes the
VRAM block; entry count is what matters.)

Corpus: **1142 upload units across SYS+ITEM (49 files)** — **SYS: 968 units** (= **485
textures** [440 PSMT4 + 43 PSMT8 + **2 PSMCT24 32×32** at `SYS/S00.GSL` `0x1440`/`0x12710`]
+ **483 CLUTs** [393 CT32 16×2 + 47 CT16 16×16 + 43 CT32 16×16]; every palette immediately
follows its texture — strict 1:1 with **zero surplus palettes**; the two surplus uploads are
the CT24 textures in `S00`. *(An earlier pass had the texture/CLUT counts inverted and called
the surplus uploads palette-only; corrected against the full-corpus extraction.))*
+ **ITEM: 174 units** (`METER0..10` 52 + `PARTS` 122). *(The original pass quoted 968 as
the corpus total; the re-walk shows that figure covers SYS only.)*

### VRAM placement baked in the file

`BITBLTBUF.DBP` (bits 32–45) is **absolute** in most files and the loader does *not* rebase
them: `NOTE` pages `0x1f2e..`, `SCORE`/`SCOREA` `0x2c36..`, `S00`/`S01` `0x309e..`,
`SORA` `0x3cfe..`, `EFFECT` `0x3f29..`, `SCOREA07` `0x3a34..`. The four files whose first
unit carries `DBP=0` (`FONT`, `FONTE`, `LOGO`, `TITLE`) belong to request categories whose
loader **adds a runtime VRAM page base** (§2). `FONTE`'s own CLUT sits at `DBP=0x180`,
exactly past its 96 KB texture (0x180 pages of 256 B) — the same "palette page starts where
the texture's pages end" rule as `field-textures.md`.

---

## 2. Loader path (PROVEN)

All consumer code sits in the asset-loader cluster `0x0020af00..0x0020e100`:

- **`FUN_0020d120` (`AssetTaskStep`)** dispatches on the request tag at `tcb+0x1c`
  (category = bits 16–19, low byte = descriptor index). State 0 reads the descriptor via
  `FUN_0020e1f0` into the fixed buffer `0x00100000`; state 1 walks the chain from the buffer
  start, then calls **`sendTexturePackets(0x00100000)`**.
- **`sendTexturePackets` @ `0x00207b50`**: `FlushCache`, then DMAC channel 2 (GIF):
  `QWC=0`, `TADR = buffer`, `CHCR = 0x105` — the *hardware* walks the CNT chain from the
  file start; tag + payload bytes stream straight into the GIF.
- **`fatal("GSL DMA Tag Error!")`** — string at vaddr `0x002ea490` (file offset `0xeb490`;
  ELF delta **`0x1ff000`**, verified from all 3 phdrs, *not* `0x200000`). 28 references from
  every walk site: `FUN_0020d120`, `FUN_0020aef0/b348/b5e8` (staged chains), `loadFieldTile`,
  `FUN_0020c188`, `loadCarModel(At)`, `FUN_0020dde0/d008/e030`.
- **DBP rebase variants** — several categories patch the u64 at `tag+0x20` with mask
  `0xffffc000ffffffff`, `bits 32–45 += <page base>` before the kick (same rewrite as
  `SendTexturePacketsDMA` uses for COURSE chains):

| category | file(s) | DBP addend |
|---|---|---|
| `0x40000` | table B misc packs (**FONTE**, SELECT, STAMP, NOTE, POSE, PUTI.GSL, S00–S02, SCORE…, OPTION2) | *none* (absolute pages) |
| `0x10000` | B+6 `SYS/TITLE.GSL` | `+0x1d8e` |
| `0x20000` | B+43 `SYS/TAKARA.E3D` then B+45 `SYS/LOGO.GSL` (boot logo chain) | `+0x1d8e` |
| `0x60000` | B+25 `SYS/PUTI.BIN` (chunked 0xC000 reads — same packet grammar per chunk) | `+0x237a` |
| `0xA0000` | `ITEM/METER0..10.GSL` (gauges) | `+0x237a` |
| `0x80000` | B+42 `ITEM/PARTS.GSL` | `+0x2c7e` |
| `0x0` staged | `FUN_0020aef0` EFFECT→CARS PARTS/TIRE/WHEEL; `FUN_0020b348` POSE→COIN; `FUN_0020b5e8` SELECT/S00 chains | per-step |

`SYS/SCORE.GSL` + `SCOREA*.GSL` are additionally registered twice (tables B and C —
`asset-map.md`); table C has no code reference, so B is the live path.

Which task issues which request tag is `asset-map.md` territory (task pool spawns carry the
tag at `tcb+0x1c`). The specific spawner of the font request `0x40000|0x00` (B+0 =
`FONTE.GSL`) was not pinned to one call site this pass [OPEN].

---

## 3. Per-file inventory (PROVEN — unit counts from the chain walk)

All 37 `SYS/*.GSL` + 12 `ITEM/*.GSL` parse to a clean END tag. Shape = `psm width×height`.

| file | units | notable contents |
|---|---:|---|
| `FONT.GSL` | 2 | T4 **512×384** glyph atlas + CT16 `16×16` CLUT — JP font, **orphan** (no LBA constant, `asset-map.md`) |
| `FONTE.GSL` | 2 | identical geometry to FONT — EN font actually used (B+0) |
| `LOGO.GSL` | 2 | T8 384×384 + CT32 CLUT (boot logo, DBP rebased `+0x1d8e`) |
| `TITLE.GSL` | 4 | T8 **640×448** bg + T8 440×220 + 2 CT32 |
| `WORLDMAP.GSL` | 2 | T4 **640×448** map + 16×2 CT32 |
| `NOTE.GSL` | 6 | T8 192×192, T8 368×319, T8 512×160 + 3 CT32 (stamp/result notes) |
| `SELECT.GSL` | 6 | T4 256×216, T4 192×896, T4 200×150 + 3 small CLUTs (player-select UI) |
| `SORA.GSL` | 4 | T8 512×96 + T4 **1024×128** strip (sky/parallax UI) |
| `OPTION2.GSL` | 2 | T8 256×128 + CT32 (options screen) |
| `POSE.GSL` | 8 | T8 256×128, T4 256×256, 2× T4 256×128 + 4 CLUTs (notebook poses) |
| `SCORE.GSL` | 6 | 2× T4 256×256 + T8 64×64 + 3 CT16 |
| `SCOREA00..20.GSL` (20 files — `SCOREA16.GSL` does not exist on disc) | 4–26 | per-race score screens; the size-twins share `{2× T4 256×256 + CT16}`; `SCOREA05` 9× T8 192×320 + 2× T4 256×256 + 256×40 + 64×64; `SCOREA06` 2× T8 **640×448** + SCORE-like tail; `SCOREA07` 7× T4 256×256 + 256×40 + 64×64×2 with 16×2 CT32s |
| `EFFECT.GSL` | 72 | 30× T4 32×32, 6× T4 64×32, 36× 16×2 CT32 (garage parts staging) |
| `STAMP.GSL` | 202 | 100× {T4 64×64 + 16×2 CT32} pairs + T8 192×192 + 16×16 CT32 head (stamp-note icons) |
| `PUTI.GSL` | 204 | T8 192×192 + T4 320×128 head + 100× {T4 64×47 + 16×2 CT32} pairs |
| `S00.GSL` / `S01.GSL` | 172 / 124 | small T4 sprites (S00: 32×32 ×48, 64×64 ×21, 64×32 ×7, 128×64 ×3) with 16×2 CT32s (scene-intro icons; shared VRAM bank `0x309e`) |
| `S02.GSL` | 18 | 9× T8 220×156 + CT32 (dialog portraits) |
| `ITEM/METER0..10.GSL` | 2–8 | gauge sprites: T8/T4 256×128, 128×120, 64×16 … + CT16/CT32 |
| `ITEM/PARTS.GSL` | 122 | mixed shop icons: T4 64×64 ×34, T8 64×64 ×15, T8 128×64 ×12, + 31× 16×2 CT32 / 30× 16×16 CLUTs |

Non-GSL SYS files are different containers: `HG2.ICO`/`COIN.BIN`/`OPTION.BIN`/`PUTI.BIN`
(model/data packs) and `*.E3D` (3-D) — not covered here.

---

## 4. FONT/FONTE glyph atlas — cross-check verdict: **MATCH**

Applying the `field-textures.md` decode conventions (payload-linear, T4 low nibble first,
CSM1 CLUT) to `FONTE.GSL`:

- Unit 0 declares TRXREG **512×384 PSMT4**; image data occupies file `0x70..0x18070`
  (98,304 B = 512·384/2 exactly). Rows are 256 B = 512 px, linear (no GS swizzle on disc).
- **Cell geometry (corrected by full decode, `assets/glyph_atlas.cpp`):** 16 px wide ×
  **24 px pitch**, 32 columns × 16 rows = **512 slots** — the earlier 12-px/1024-slot
  reading was wrong (text rows land on exact 24-px boundaries; the 12-px grid split every
  glyph across two bands and put slot 33 on a zero row). Glyph stream starts at
  slot 0 = `0x21` (`!`), **31 characters per row**, each row's col-31 slot holding a
  coloured icon (circled numerals / button art in the yellow ramp). Char→slot:
  `g = code−0x21; slot = (g/31)*32 + (g%31)` — verified `A`=33, `^`=62, `_`=64
  (row 2 col 0), `` ` ``=65, with pixel-perfect A/B/C/D renders at slots 33–36 (both
  numbering schemes coincide there, e.g. (`#`=index 2, `+`=3, `.`=1, space=0):

  ```
  A            B            C            D
   .?##?.      .#########?.   ..###????##?.  .?########?..
  ..#?##..     .##?. ..?##.  .?##..   ..##. .?#?. ...?##?.
  .?#.?#?.     .##?.  ..#?.  ..##?.          .?#?.    .?##..
  ```

  (Decoded letterforms — illustrative only, not committed game data.)
- **CLUT** at file `0x180e0..0x182e0` (512 B = CT16 `16×16`, 256 entries; logical PSMT4
  entry `i` reads physical `(i&7) | ((i&8)<<1)` — the GS CSM1 bit-3/4 exchange already
  pinned in `field-textures.md`). Logical entries: 0 = `0x0000`, 1/3 = `0x0842` (dark
  gray outline), **2 = `0x7fff` (white — the glyph core)**, 4 = light gray, 7 = mid gray,
  5/6/8–15 = warm yellow ramp (the UI accent color family). Index histogram over the atlas
  uses all 16 logical entries, dominated by 0 (background).
- **Alpha (ratified convention):** every CT16 entry has the alpha bit clear, including the
  white core (`0x7fff`). The native decoder follows `image_converter`'s TEXA/AEM convention
  (TEXA 0x80 → nonzero alpha = 255, palette entry 0 transparent) — renders correct glyphs
  in the app; byte-exact against the `convertImage` oracle.
- **Verdict: MATCH.** The FLD palette grammar decodes the font byte-exact; `FONT.GSL` is
  the same container with a different (JP) glyph payload — 39,430 differing bytes, only 3
  differing bytes in the first `0x70` (tool pointers). Whether FONT's slots beyond ASCII
  follow Shift-JIS index order is [OPEN].

`DrawDialogTextLine` (`0x00202D10`) / `FUN_002017B0` glyph loop draw from this atlas;
the **16×12 grid is the text-layout advance** (12-px line advance over 24-tall shadowed
bitmaps), not the atlas cell pitch — atlas geometry is 16×24 per the correction above.

### Full-corpus extraction (rt-pack-extract, PROVEN over the user's disc)

`src/native/tools/pack_extract_main.cpp` + `pack_catalog.hpp` catalog all **222 packs /
1490 units** (SYS 37, ITEM 12, SHOP 22, CAR 151), validate `qwc==6+NLOOP` on every unit,
and emit deterministic JSON catalogs (two runs byte-identical; `native_assets_packs_integration_test`).
Notes established by that pass:

- **`.lbn_map.tsv` column 3 is BYTES** (equals the real file size on all 376 mapped
  entries), not sectors; sectors = `ceil(bytes/2048)`.
- **SHOP layout:** `T00..T20.BIN` = texture chain at file offset 0 (T8 640×384 + CT32
  16×16, END at `0x3c510`) + trailing model payload; `GARAGE.BIN` starts with a 16-byte
  region directory, chain `0x19990..0x52380` (2 tex + 2 pal) + trailing payload.
- CAR pack chains are **CNT (ID 1), not REF** — see `car-data-status.md` correction.
- `convertImage()` cannot decode pack units directly (packs carry 16 B tool junk where the
  strict FLD A+D parse demands a GIF packet header); `pack_catalog.hpp` mirrors its proven
  bitfield extraction read-only instead.

---

## 5. Runtime extraction plan (UI assets, no bytes committed)

1. **Parser** — implement the §1 walk verbatim (it *is* the game's validation): 16 B tags,
   `byte[3] ∈ {0x10, 0x70}`, `qwc*16 == 96 + NLOOP*16` per unit; reject otherwise.
2. **Per unit, emit a descriptor** (never bytes): `{fileOffset, psm, width, height,
   dbp, clutOffset, clutPsm}` — the same descriptor philosophy as
   `fieldTextureCatalog()` in `src/native/assets/field_converter.cpp`.
3. **Decode on demand** — T4 low-nibble-first, T8 byte-per-pixel; CSM1 unswizzle for CLUT;
   CT32 palettes byte0=R, alpha ∈ {0x00, 0x80} (per `field-textures.md`); CT16 RGBA5551.
4. **Repo hygiene** — commit only `src/native/assets/gsl_pack.{h,cpp}` + tests that pin
   *structure*: 49/49 clean walks, per-file unit counts (table §3), `FONTE` unit-0
   `TRXREG=512×384 @ file 0x70`, CLUT at `0x180e0`, logical palette entry 2 == white.
   Golden *images* stay out of git (same policy as the FLD tiles); tests synthesize chains
   for error paths (`BadHeader`/`Truncated`), mirroring `test_field_converter.cpp`.
5. **Asset picker** — for UI work the interesting units are: font (`FONTE` unit 0),
   HUD gauges (`ITEM/METER*`), map (`WORLDMAP`), portraits (`S02`), icons (`STAMP`,
   `PUTI`, `ITEM/PARTS`), title/logo (`TITLE`, `LOGO` — note DBP rebase adds `0x1d8e`).

---

## Ledger

**PROVEN** (byte-verified against the disc extract + ELF):
- Chain grammar (CNT tags `0x10`, END `0x70`, stride `(qwc+1)*16`), unit layout, and the
  `qwc*16 == 96 + NLOOP*16` invariant — 1142/1142 units across 49/49 files (SYS 968 +
  ITEM 174; independently re-walked 2026-10-16, zero failures).
- Unit payload = junk-16 + A+D{0x50,0x51,0x52,0x53} + GIF IMAGE tag + raw image; image
  bytes == `w*h*bpp/8` exactly.
- Loader: `FUN_0020d120` categories, `sendTexturePackets` = DMA CH2 `TADR` kick,
  `GSL DMA Tag Error!` @ `0x2ea490` (delta `0x1ff000` verified from phdrs), DBP rebase
  addends per category, B/C table registration and the `FONT.GSL` orphan.
- `FONTE` atlas: 512×384 PSMT4 @ `0x70`, 16×12 ASCII-ordered cells, CT16 CLUT @ `0x180e0`
  with CSM1 order; decoded letterforms + white-core palette confirm a real glyph atlas.

**INFERRED:**
- The format extends to every descriptor-table `.GSL` (all 49 sampled files parse; no
  counterexample). Non-.GSL SYS files excluded.
- Tool provenance (Windows x86 serializer) from the pointer shapes
  (`0x00405bbe` code / `0x1012b9a4` DLL data / `0x0012f9ec` stack) — consistent across all
  files and headers, but the tool itself is unidentified.
- `S00`/`S01` sharing VRAM bank `0x309e` implies never-resident-at-once (scene-sequential).

**OPEN:**
- Task/site that issues the font request `0x40000|0x00` (B+0 binding is proven; the
  spawning call site isn't pinned).
- CLUT alpha-bit semantics (all-zero including white).
- `FONT.GSL` slot ordering beyond ASCII (Shift-JIS?) and which build stages consumed it.
- The two surplus CLUT-only uploads (which files/why — likely a palette refresh).
- Exact GIF-visible byte stream semantics (whether the junk 16 B at `T+16` and tag hi
  qwords are consumed as data by the GIF in TTE mode, or masked by NLOOP/field logic) —
  harmless for extraction, unresolved at the hardware-behavior level.
