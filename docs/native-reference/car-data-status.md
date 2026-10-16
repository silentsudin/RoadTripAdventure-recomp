# CAR car-model data status (Road Trip, SLUS-20398)

First-pass container grammar, 2026-10-16. Corpus: `CAR0/..CAR4/`, `CARS/` in the
read-only disc extract (`~/Library/Application Support/RoadTripRecomp/disc`).
Python probes ran from `/tmp/rt-car/` only; structures/constants/counts below,
no game bytes. Companion to `action-data-status.md`; descriptor context in
`asset-map.md`; the FLD GIF grammar reused here is specified in
`field-textures.md`.

## Result summary

| question | verdict |
|---|---|
| descriptor join | **154/154 files matched** by exact `(LBA, ceil(size/2048))` — 151 via table E (`0x29b438`), 3 via table B (`0x29af58`) |
| container class | **New grammar** — 12-word header with a six-pointer model directory; **not** FLD `[hdrsz,offA,offB,offC]`, **not** PMP-direct (zero `0x600/0x608` magic) |
| one file == one car | **Yes** — 6 model variants + exactly one 128×128 PSMT8 texture + one 256-entry CT32 palette, loaded as a unit into one slot |
| car mapping confidence | **High (PROVEN)** — table E record order is Q00→Q150 ascending 151/151, and the loader's `idx` byte *is* the Q-number (debug string `"CAR SIZE OVER! Q%02d:%X"`) |
| texture availability | **Every car self-textured** — 151/151 files carry a structurally identical 2-upload chain (fixed 0x4510-byte region; payload = per-car unique livery pixels — tails are *not* byte-identical across files); decode conventions of `field-texture-rendering.md` apply directly |

## Inventory + disc identity (PROVEN)

154 files, 32,636,192 bytes:

| dir | files | bytes | naming |
|---|---:|---:|---|
| `CAR0` | 30 | 6,216,432 | `Q00.BIN..Q29.BIN`, 183,280..230,560 B |
| `CAR1` | 30 | 6,565,328 | `Q30..Q59`, 194,640..230,624 B |
| `CAR2` | 30 | 6,343,808 | `Q60..Q89`, 155,312..229,968 B |
| `CAR3` | 30 | 6,334,512 | `Q90..Q119`, 178,064..229,744 B |
| `CAR4` | 30 | 6,334,592 | `Q120..Q149`, 182,656..229,440 B |
| `CARS` | 4 | 841,520 | `Q150.BIN` + `PARTS.BIN`, `TIRE.BIN`, `WHEEL.BIN` |

Q-number coverage 0..150 with **no gaps**. The split is purely a directory
shard: folder *n* holds Q[30n..30n+29] and the 151st car lives in `CARS` next
to the three part packs.

**Descriptor join (re-ran the `asset-map.md` method).** ELF phdr map derived
per-phdr: two PT_LOADs (text `0x200000`/off `0x1000`, data `0x29a880`/off
`0x9b880`) → **vaddr−fileoff = 0x1ff000 for both**, verified from the program
headers (not assumed). Descriptors re-read at vaddr `0x29b438` (table E, 151
records) and `0x29af58` (table B, 46 records), tags all `0x00333388`:

- table E → **151/151 exact unique matches** to the 151 Q-files.
- remaining 3 files match table B records **B+2 = `CARS/PARTS.BIN`**
  (`0x89be`), **B+3 = `CARS/TIRE.BIN`** (`0x8999`), **B+4 = `CARS/WHEEL.BIN`**
  (`0x8a13`) — exactly the `FUN_0020aef0` garage/showroom staging chain
  (B+1 `ITEM/EFFECT.GSL` → B+2..B+4).
- **Matched/total = 154/154; zero ambiguous, zero unmatched.**

Two stronger facts fall out of the join:

- **Table E record order is Q00..Q150 ascending, 151/151** (record *i* has
  Q-*i*'s LBA). The per-folder head-shuffle noted in `asset-map.md` is disc
  write order only; the table the loader indexes is in Q order, so
  `loadCarModel(idx)` ↔ `Q{idx:02d}.BIN` is direct.
- The whole car block is **one contiguous disc run**: sorted-by-LBA table E
  adjacencies have 150/150 zero gaps (`0x4c5d..0x8999`), and Q150's end
  (`0x8999`) is TIRE's start — PARTS/TIRE/WHEEL close the run at `0x8ae6`,
  where the COURSE block begins (`course-data-status.md`).

## Q-file container grammar (PROVEN 151/151)

**Header — 12 words (0x30 bytes):** `[0..5]` six file-relative model starts
(all `0x30`, strictly ascending), `[6]` model-region end == **texture-chain
start**, `0x219a0..0x33fd0`, never above `0x34000` (the loader fatals above),
`[7]` == physical file size in **151/151** files, `[8..11]` zero.
This matches the header already pinned in `src/native/assets/model_converter.h`
(word indices shift by the 12-vs-8-word reading; that header's `+0x18/+0x1C`
are words 6/7 here and the "zero padding" is words 8..11).

**Model *k*** spans `[word_k, word_{k+1})` (word 6 closes model 5) and opens
with five words: partA offset (model-relative, always 16-aligned), partB
offset, partA count, partB count, main VIF/DMA chain at `+0x10`. Corpus walk:

- **906/906 main chains** (6 × 151) parse as CNT frames (`byte3 0x10`)
  terminated by `RET` with `qwc == 0` — same chain grammar
  `decodeVifChain` consumes; no failures, no stray tag ids.
- partA count nonzero: models 0 and 1 in **all 151** files.
- partB count nonzero: model 0 in **all 151** files (counts 2..33; max 33 on
  `CAR2/Q80.BIN`), model 2 in exactly **3** files (`CAR1/Q44`, `CAR2/Q71`,
  `CAR4/Q141`, counts 1/1/2).

**PMP magic density: zero.** No `0x600`/`0x608` word exists at any structural
offset; whole-file u32 scans across all 154 files find 3 stray word-hits in 2
files (vertex-data coincidences). The FLD `0x600/0x608` PMP container and the
FLD `[hdrsz,offA,offB,offC]` shape are both **absent** from this class — car
models are raw VIF/DMA chain bundles, not PMP blocks.

**Vertex streams:** `MSCALF` census over all 151 model regions —
**105,498 × `MSCALF 4`** (VU program 4 "LitStrip", the cars/props/lit-model
program per `vu1-programs.md`) and **4,373 × `MSCALF 2`** = 109,871 calls;
`MSCALF 8` (WorldStrip) and the ACTION `MSCALF 6` variant do not occur.

### Texture chain — one fixed layout, structurally identical in every file

*Correction (independent re-check 2026-10-16): the region is constant-length and
constant-structure, but content differs per file — all 151 tails are distinct
because each car carries its own livery texture (verified: Q00 vs Q01 differ in
17,168 of 17,680 bytes). "Byte-identical" was wrong; structure below is right
(Q00: regs chain, NLOOP 1024 + palette NLOOP 64 + END, byte-exact).*

`[word6, EOF)` is always exactly **0x4510** bytes and always parses as:

| rel. offset | frame | contents |
|---|---|---|
| `+0x0000` | REF `qwc 0x406` | 128×128 **PSMT8** (`0x13`) upload → **DBP 0, DBW 2** |
| `+0x4070` | REF `qwc 70` | 16×16 **PSMCT32** upload → **DBP 64** (256-entry CLUT) |
| `+0x44e0` | END | terminator |

151/151 files END-terminated, 302 uploads total (151 textures + 151 palettes),
DSAX/DSAY zero, payload byte-exact (`128·128` and `16·16·4` bytes), no parse
errors. **GIF IMAGE uploads per file: exactly 2.** PSM/dims distribution is
therefore the degenerate set `{PSMT8 128×128 ×151, CT32 16×16 ×151}`.

**Bank rule: the FLD `0x285e + k·0x420` absolute-page rule does not apply.**
In-file destination pages are **0-based** (min DBP 0 in 151/151 files). The
loader rebases at load time: `loadCarModel` (`0x20c7b8`, decompiled) rewrites
bits 32..45 (mask `0xffffc000ffffffff`) of the `BITBLTBUF` qwords in the
staged chain to `(inFile + slot*0x44 + 0x243e) & 0x3fff`. The slot stride
`0x44` pages equals this file's exact footprint (64 texture + 4 palette
pages), so the 8 slots occupy `0x243e..0x265e` — safely below the FLD bank
base `0x285e`. (Correction to `functions.tsv`/`asset-map.md` wording: the
rebase target is the upload **DBP in BITBLTBUF**, not TEX0 TBP; same constant,
same mask as `SendTexturePacketsDMA`.)

**Texture availability verdict:** every car ships its own texture page and
palette in the same file, in the proven payload-linear/CSM1/byte0=R encoding
of `field-texture-rendering.md` — a native loader needs no external texture
source for the Q class.

## CARS/PARTS, TIRE, WHEEL (PROVEN shape)

Region-directory containers, a variable-length sibling of the ACTION `0x30`
multi-boundary variant: word 0 = directory byte size (`0x40`/`0x30`/`0x50`),
the following nonzero words are **ascending region boundaries whose last entry
equals EOF**; no GIF uploads and no PMP magic anywhere. Every interior region
(except a trailing marker region in two files) opens with the **same
five-word car-model header** as the Q files (16-aligned part offsets, CNT main
chain at `+0x10`):

| file | regions | model-shaped | notes |
|---|---:|---:|---|
| `PARTS.BIN` (0x2a3c0) | 12 | 11 + marker | partA counts 7 and 18 on two models; marker region at `0x24a50` begins `u32 0x10870206` |
| `TIRE.BIN` (0x12270) | 6 | 5 + marker | marker region at `0xff60`, same `0x10870206` |
| `WHEEL.BIN` (0x691f0) | 15 | 15 (all) | counts all zero → single main chain each |

31 model-shaped regions total — consistent with these being the garage
tuning-part / tire / wheel mesh packs drawn onto the player car (consumers
below). The `0x10870206`-prologue marker regions are the constant-template
family of the COURSE trailing tables (`course-data-status.md`).

## Consumer trace (Ghidra, SLUS_203.98)

**Strings:** `"CAR SIZE OVER! Q%02d:%X"` @`0x2ea638` (the 0x34000 gate, prints
the Q-number) and `"car model copy #%d -> #%d"` @`0x2ea470`. There are **no
disc path templates** for CAR files (`search_strings` for "CAR", `%s/%d`
shapes): loading is purely LBA-descriptor based; `Q%02d` exists only in debug
output, so the folder/Q-name sharding is a disc-layout artifact, not a runtime
concept.

**`loadCarModel` `0x20c7b8` (decompiled, PROVEN):** stages
`FUN_0020e1f0(idx*12 + 0x29b438, 0x100000)` (table E record for `idx`),
fatals if word 6 > 0x34000, `memmove`s **only word-6 bytes** into
`carModelData + slot*0x34000`, rebuilds the six model pointers as
`&carModels[slot*6 + i] = dest + word_i`, then walks the staged texture chain
(REF/END tags, exactly the FLD frame grammar) applying the DBP rebase above
and DMA-sends it via `sendTexturePackets`. The model region and texture chain
are consumed by different paths — models copied, textures streamed from the
0x100000 staging buffer.

**Asset steps** (`AssetTaskStep` `0x20d120`; the `0xB` category check is the
`lui 0xB` at `0x20d278` — the only `lui 0xB` in the ELF):

- `0xB0000` → `CarModelLoadStep` `0x20cc30`: `idx = tag & 0xff` (Q-number),
  `slot = (tag>>8 & 7) + 15` (8 slots, 15..22), and the loaded Q-number is
  remembered in an 8-byte table at `0x335720[slot]`.
- `0x70000` → `CarModelReloadStep` `0x20cd10`: re-loads a remembered slot from
  `0x335720` with a variant — the standard way modes re-request the player car.
- `0xC0000` → `loadCarModelAt` `0x20c9d0` → lone table-G record =
  `SYS/OPTION.BIN` through the same 0x34000-slot machinery (`asset-map.md`).

**Static reload-tag construction sites** (cat `0x7`, all `gs_tasks_spawn(…
0x20dc50, …)`): `0x70001` (town-entry flow `FUN_00211170` ← `FUN_00205088`;
showroom variant `FUN_0026d678`), `0x70003` (race-prep `FUN_0026b848`,
garage `FUN_0026d108`), `0x70006` (`FUN_00211320`), `0x70300` (race manager
`FUN_0020f950`, itself spawned by the menu flows `FUN_0026b1b8/b470/b5c8`;
also `FUN_00211648`), `0x70301` (`FUN_0026be50`, `FUN_0026d140`), and
`0x70400 | residentCount` (**town resident cars**: `FUN_00211530` and
`FUN_00210158` read `residentStart/residentCount` from the scripted scene
table at `0x2be438`). Observed reload Q-numbers are all low (Q00..Q06) — the
starter-car band. `FUN_0026b848` also shows the pack-request pattern around
car modes: gauge pack `0xa0000 | gaugeIdx` (runtime index off
`0x0177f778`, METER0..10 through table D+22) plus shop pack `0x300001`.

**Request space beyond ACTION's types 18..38:** the `0x50000` misc loader base
`0x29be50` = F+64 makes type 38 (A20) the last in-bounds record — **there is
no type ≥ 39**; the neighbouring spaces are the `0xA` gauge family
(D+22, idx 0..10) and the `0x4` table-B packs. The car-adjacent packs the game
requests directly are the staged garage chain `FUN_0020aef0`
(B+1 `EFFECT.GSL` → B+2 `PARTS` → B+3 `TIRE` → B+4 `WHEEL`, texture send
between steps) and B-table misc entries. The showroom draws the loaded slot's
six models via `DrawShowroomCarModel` `0x223238`
(`vifCallModel` into `&carModels[idx*6] + 0x10`, model index 0..5).

**OPEN (producer):** no code path constructs a `0xB0000` tag by immediate
(the category exists only in the dispatcher), and the tag-shaped `0xb0310`
words in the story-text region turned out to be message-pointer-table content
(text coincidence, xref `0x2a5574` is a string pointer array). Fresh car
loads therefore carry runtime idx/slot values — save-data/script-driven.
Candidate producers need a runtime or save-structure trace.

## Ledger

**PROVEN:** 154/154 descriptor join; table E order = Q ascending; contiguous
disc run; 12-word header grammar (151/151); six-pointer model directory;
906/906 chain walks; fixed 2-upload texture chain (302 uploads, PSM/dims set,
0x4510 region, END-terminated); DBP rebase `+0x243e + slot*0x44` with slot
stride == file footprint; loader internals (staged read, word-6 copy, pointer
rebuild, gate vs `CAR SIZE OVER`); slot formulas and remembered-Q table
`0x335720`; reload-tag construction sites; extras region counts + car-model
header shape + marker regions; zero PMP magic; zero FLD bank rule.

**INFERRED:** the six model roles (body near/far, variant a/b, extra a/b —
from `model_converter.h` §8.4 naming; indices used by the showroom drawer but
per-index semantics unlabelled); partA/partB counts as draw gates; extras
regions = tuning-part/tire/wheel meshes for the garage chains; marker regions
= COURSE-template-family prop/control tables.

**OPEN:**
- producers of fresh `0xB0000` load tags (runtime idx/slot source;
  save-structure or runtime trace needed);
- per-index model-role semantics (which of models 0..5 is the LOD/variant
  used where — needs the draw-path slot/variant selection);
- per-region identity inside PARTS/TIRE/WHEEL (which mesh is which part) and
  the `0x10870206` marker-region schema;
- car sampler state: `MSCALF 4` chains carry no in-stream TEX0 candidate in
  the FLD `@0x192` form — the draw wrappers
  (`DrawShowroomCarModel`, `InitCarRenderRegs` `0x223410`) upload GIF regs
  per draw; the exact TEX0/TEX1 filter state for a faithful native sampler is
  not yet pinned (the texture itself is unambiguous);
- `functions.tsv` wording: CarModelLoadStep's "texture rebase TBP" should read
  "upload DBP (BITBLTBUF bits 32..45)" — same constant `0x243e + slot*0x44`
  [docs-sync].

## Runtime extraction plan (feed `assets/model_loader`) — describe only

1. **Meshes:** already covered — `convertCarModels` (`model_converter.{h,cpp}`)
   converts the six models' main + count-gated part chains per file. Keep the
   0x30-byte header reading (words 0..7) and the ≤0x34000 gate as validation.
2. **Textures:** add a trivial car-texture catalog reusing the proven
   `FieldTexture` grammar (or a fixed 2-record parser): emit
   `{PSMT8 128×128 @dbp0, CT32 256-entry @dbp64}` and decode RGBA via the
   existing `image_converter` conventions (payload-linear, CSM1
   `swapBits34(csa*16+entry)`, palette byte0=R). Pin with corpus counts:
   151/151 layouts, 302 uploads, region size 0x4510.
3. **Binding:** one texture per car file — bind directly per Q-number; model
   the runtime placement only if VRAM-accurate replay is wanted
   (`tbp = 0x243e + slot*0x44`, slot from the request tag; slots 15..22,
   `0x335720` remembered-Q table for reload semantics).
4. **Source of bytes:** extracted `CAR*/Q*.BIN` directly (LBA join is 1:1) or
   descriptor-driven reads via table E for bit-exact streaming order; the
   `CARS` extras need the multi-region directory walk (31 model regions +
   marker skip) before their meshes can enter the same pipeline.
5. **Tests to pin:** header invariants (151/151), chain walk (906), texture
   chain fingerprint (2 uploads, PSM/dims, dbp 0/64), extras region census
   (11/5/15 + markers) — all skip cleanly without `$RT_DISC`.

Method artifacts: probes `/tmp/rt-car/probe1_inventory.py` (join),
`probe2_grammar.py`/`probe4_corpus.py`/`probe5_texlayout.py` (container +
texture census), `probe3_mapping.py`/`probe6..8` (table order, tag
construction scans), Ghidra MCP session on `/SLUS_203.98` (string search +
decompile of `0x20c7b8`, `0x20cc30`, `0x20e708`, `0x211530`, `0x211170`,
`0x26b848`, `0x26b470`). No game bytes retained.
