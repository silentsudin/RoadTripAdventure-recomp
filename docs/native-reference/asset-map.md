# ROM asset descriptor tables — complete disc asset map

Updated 2026-10-16. Full sweep of the `SLUS_203.98` ELF descriptor region, cross-referenced
file-by-file against the user disc extract (`~/Library/Application Support/RoadTripRecomp/disc`,
override `RT_DISC`). This is the completion of the sweep previewed in
`course-data-status.md` ("`C12..C20` are claimed by other descriptor tables in the wider
`0x29aexx`–`0x29bfxx` region") and extends the FLD table proof in
`field-topology-status.md` to the whole asset-loading system.

**Headline (PROVEN): 384 descriptors, 384 exact matches.** Every descriptor in the region
resolves to exactly one file in the extract by matching `(sectorOffset, sectorCount)` against
the disc's real LBA map (`.lbn_map.tsv` shipped alongside the extract) — 384/384, zero
ambiguous, zero bogus. 363 of 376 extract files (99.32 % of payload bytes) are claimed.

## Record format and scan method (PROVEN)

- Descriptor record = 12 bytes `{u32 tag 0x00333388, u32 sectorOffset, u32 sectorCount}`.
  `FUN_0020e1f0` (read wrapper) consumes it as `sceCdRead(*(ptr+4), *(ptr+8), dst, 0x333380)`
  and returns `sectorCount * 0x800` — so `sectorCount == ceil(fileSize/2048)` and
  `sectorOffset` is an **absolute disc LBA** (identical to the extract's `.lbn_map.tsv` values).
- ELF layout: two PT_LOADs — vaddr `0x00200000` + file off `0x1000` (text),
  vaddr `0x29a880` + file off `0x9b880` (data, size `0x9adfb`). The program headers are
  linked at `0x00200000` (no `0x80000000` mask), so Ghidra vaddrs convert directly.
- Alignment trap: the six tables are each internally 12-byte-strided, but the tables sit on
  **different residues mod 12** (4, 4, 8, 0, 4, 8) — a naive fixed-grid scan silently misses
  tables. Detection method: scan all 4-byte-aligned words for the tag, group positions by
  residue mod 12, keep maximal stride-12 runs. The descriptor block spans
  **`0x29ADF0..0x29C024`** (last tag at `0x29C018`); immediately below it
  (`0x29AC00..0x29ADF0`) is jump-table/config data referenced by the streaming code — not
  descriptors.

## Table inventory

| id | vaddr range | recs | category (binding) | files (glob) | monotonic in table order |
|---|---|---|---|---|---|
| A | `0x29ADF0..0x29AF4C` | 29 | SOUND assets (sound-RPC path) | `SOUND/*.VAG`×6, `*.TVB`×3, `*.TSQ`×20 | no |
| B | `0x29AF58..0x29B180` | 46 | misc screens/UI/models, **indexed by request type byte** | `SYS/*`×42 + `CARS/{PARTS,TIRE,WHEEL}.BIN` + `ITEM/PARTS.GSL` | no |
| C | `0x29B190..0x29B28C` | 21 | per-race score UI, race idx 0..20 | `SYS/SCORE.GSL`, `SYS/SCOREA{00..15,17..20}.GSL` | no |
| D | `0x29B290..0x29B41C` | 33 | shop pack + gauges | `SHOP/GARAGE.BIN`, `SHOP/T00..T20.BIN`, `ITEM/METER0..10.GSL` | no (sub-blocks: no) |
| E | `0x29B438..0x29BB4C` | 151 | car models (`loadCarModel`) | `CAR0..CAR4/Q00..Q149.BIN` + `CARS/Q150.BIN` | no (per-folder pattern) |
| F | `0x29BB50..0x29C024` | 103 | **3 sub-tables:** FLD tiles (0–63), COURSE (64–81), ACTION (82–102) | `FLD/ABC.BIN`×64, `COURSE/*.BIN`×18, `ACTION/A00..A20.BIN` | FLD no; COURSE **yes**; ACTION **yes** |
| G | `0x29B428` (lone record) | 1 | OPTION model (`loadCarModelAt`, cat `0xC`) | `SYS/OPTION.BIN` | n/a |

Gaps between tables are all-zero (12 B between A/B, 16 B B/C, 4 B C→D and E→F; 28 B D→E
containing the lone G record + its zero-fill). Total descriptors: 29+46+21+33+151+103+1 = **384**.

## Code bindings (constants → table), from `FUN_0020d120 AssetTaskStep` + helpers

Request tag (u32 at tcb+0x1c): category = `tag & 0xF0000`, low byte = table index,
high byte = slot/variant.

- `0x90000` → `loadFieldTile(…, 0x29bb50 + (tag&0xff)*12, id, id&3)` — **F sub-table 0..63**
  (FLD tiles; 4 streaming slots). PROVEN earlier; id↔filename formula
  `id = (A<<4)|(B<<2)|swap2(C)` re-verified against the exact matches: 64/64, zero mismatches.
- `0x50000` → `FUN_0020c188 FieldTileMiscLoader(…, 0x29be50 + type*12, type, 2)` where
  `type = signed byte at tcb+0x22`. `0x29be50` = F+64, so **type 0..17 = COURSE C00..C14,
  C18..C20** (PROVEN for 0..11 earlier; now extended through C20), and **type 18..38 lands
  exactly on ACTION A00..A20** (F+82 = `0x29bf28`) — the course/action sub-tables are one
  contiguous index space read through the same container loader [INFERENCE: physical adjacency
  + same base/stride; the runtime type values are request data, no direct constant].
- `0x40000` → `0x29af58 + (tag&0xff)*12` — **table B** misc pack.
- `0xA0000` → `0x29b398 + (tag&0xff)*12` — `0x29b398` = D+22 = `ITEM/METER0.GSL` base →
  **gauges METER0..METER10** (idx 0..10).
- `0x60000` → `FUN_0020e250 ReadDiscSectorsAligned(0x29b084, …, (tag&0xff)*0xC000, 0xC000)`
  — `0x29b084` = B+25 = **`SYS/PUTI.BIN` chunked loader** (4.9 MB file in 0xC000 steps).
- `0xB0000` (`FUN_0020cc30`) and `0x70000` (`FUN_0020cd10`) → `loadCarModel` (`0x20c7b8`):
  disc read via **`0x29b438 + idx*12` = table E** (`idx` = request low byte = Q-number
  0..150), RAM slot = `(tag>>8&7)+15` for cat `0xB` (high slot band; cat `0x7` re-loads a
  remembered slot + variant), slot stride `0x34000` into `carModelData` (`0x3FBB80`),
  per-slot texture rebase `TBP 0x243e + slot*0x44`, file header carries a 6-pointer
  model directory. `0x29b438` confirmed in raw bytes (`lui at,0x2a; addiu at,at,-0x4bc8`).
- `0xC0000` (`loadCarModelAt` @ `0x20c9d0`) → **`0x29b428 + idx*12`** — only idx 0 exists:
  the lone record = **`SYS/OPTION.BIN`**, loaded into the same `0x34000`-slot model family
  (options-screen model pack). Note: an earlier session mis-attributed this base to the CAR
  table; the raw constant differs by +0x10 and lands on a different residue grid — the CAR
  table reads go through `loadCarModel` at `0x29b438`.
- `0x10000` → `0x29afa0` = B+6 `SYS/TITLE.GSL`; `0x20000` → staged `0x29b15c` (B+43
  `SYS/TAKARA.E3D`) then `0x29b174` (B+45 `SYS/LOGO.GSL`) — boot logo chain; `0xD0000` →
  `0x29b168` = B+44 `SYS/HG2.ICO`; `0x80000` → `0x29b150` = B+42 `ITEM/PARTS.GSL`
  (parts-shop icons); cat `0x0` case 3 pins `0x29b410` = D+32 (`ITEM/METER10.GSL`) directly.
- Cat `0x0`/`0x1` staged chains: `FUN_0020aef0` walks `0x29af64/70/7c/88` = B+1..4 =
  `SYS/EFFECT.GSL → CARS/PARTS.BIN → TIRE.BIN → WHEEL.BIN` (garage/showroom parts staging,
  texture send between steps); `FUN_0020b348` walks `0x29afc4` (B+9 `SYS/POSE.GSL`) →
  `0x29b00c` (B+15 `SYS/COIN.BIN`) (notebook UI); sibling helpers stage
  `0x29afd0/dc/e8` = SELECT.GSL/S00.GSL/S00.E3D and `0x29b03c/48/54` =
  S01.GSL/S01.E3D/S02.GSL (scene intro chains).
- `0x30000` → `FUN_0020cdc0` state machine (jump table `0x2ea690`); its handlers reference
  `0x29b290` (table D base) → shop pack steps.
- Table A consumer: `0x258f20` (BGM-slot wrapper cluster `0x258E50..0x2590B0`) uses base
  `0x29adf0`, and `0x256f2c` references `0x29ae20` = A+4 (`1CH_L.VAG`, first VAG stream) —
  sound LBAs are handed to the SNDMOD/IOP RPC. PROVEN as the SOUND table.
- Table C: **no code reference found** anywhere in the ELF for any address in
  `0x29b190..0x29b28c` (constant-construction scan over the full text segment). Its 21
  entries are all re-registered in B. Consumer unresolved [OPEN — candidate dead/duplicate
  registration, or a base computed at runtime].

## Table order vs disc order

Offsets are **non-monotonic in table order for every whole table** — ids are deliberately
interleaved against the write order. Patterns:

- **F/COURSE (idx 64..81) and F/ACTION (idx 82..102): strictly ascending** — the disc was
  written course-major, action-major.
- **COURSE block order** (disc = id order): C00,C01,…,C14,C18,C19,C20. No C15/C17 slots
  exist in the table (files absent from disc); **C16 exists on disc but occupies no table
  slot** (see gaps below).
- **FLD sub-table (id order)** = formula order (000,002,001,003,010,012,011,…); disc order
  is raster `ABC` with the `22x` tiles written **first** (222,223,221,220), then
  000..333 raster — id order ≠ disc order by construction.
- **E car block** disc order: `Q03,Q01,Q02,Q00,Q04..Q29` (CAR0), `Q36,Q31,Q32,Q33,Q34,Q35,
  Q30,Q37..Q59` (CAR1), `Q60..Q89` (CAR2), `Q93,Q91,Q92,Q90,Q94..Q119` (CAR3),
  `Q126,Q121,Q122,Q123,Q124,Q125,Q120,Q127..Q149` (CAR4), `Q150` — a head-shuffle per
  30-file folder, then ascending; folders and `CARS/Q150,TIRE,PARTS,WHEEL` are contiguous.
- **A/B/C/D** interleave logically (e.g. B disc order starts TIRE,PARTS,WHEEL — that is the
  CAR-adjacent cluster at LBA `0x8999`; A's VAG/TVB/TSQ pairs swap adjacency vs table order;
  D disc order is METER0,METER1,**METER10**,METER2… then T00..T20, GARAGE last).

## Disc layout — union of all claimed ranges

Boot island `[0x00..0xFB]`: volume descriptors, `SYSTEM.CNF`@0x26, `IOPRP234.IMG`@0x27, IOP
drivers `LIBSD`@0xA0, `MCMAN`@0xAD, `MCSERV`@0xDA, `PADMAN`@0xDE, `SDRDRV`@0xF4,
`SIO2MAN`@0xF8 — boot-path loaded, not descriptor-referenced.

| range (sectors) | size | contents | claimant |
|---|---|---|---|
| `0xFC..0xE2B4` | 57,784 | ACTION(21) + CAR0..CAR4(150) + Q150/TIRE/PARTS/WHEEL + COURSE C00..C20-minus-C16(18) | F(82..102, 64..81), E, B(PARTS/TIRE/WHEEL) |
| **gap `0xE2B4`, 788** | = `COURSE/C16.BIN` | **orphan — see verdict** | none |
| `0xE5C8..0x34D4A` | 157,570 | FLD(64) + ITEM(12) + SHOP(22) + SOUND head (8: 2CH_R, BGM.TVB, BGM_03/04, CQ_MAIN.TSQ/.TVB, 1CH_R, 3CH_L) | F(0..63), D, A |
| **gap `0x34D4A`, 54** | = `SOUND/SNDMOD.IRX` | IOP module copy | none |
| `0x34D80..0x3E538` | 38,840 | SOUND rest (21) + `SYS/SCORE.GSL` | A, B/C |
| **gap `0x3E538`, 49** | = `SYS/FONT.GSL` | orphan (JP font; build uses `FONTE.GSL` = B+0) | none |
| `0x3E569..0x3FC60` | 5,879 | SYS(35 incl. OPTION.BIN) + all SCOREA* | B, C, G |
| tail `0x3FC60..` | | `SNDMOD.IRX`(54) + `SLUS_203.98`(626) | boot path |

Total claimed: 260,073 sectors; every one of the 384 records fits inside these four ranges
with zero cross-table overlap except the SCORE duplicates below.

## Tile-id re-use verdict (the "other 32 ids" puzzle)

`FUN_0021c8d0` can only emit ids with `id[0] == id[4]` (row-parity constraint), leaving 32
ids unreachable — exactly those whose bits 4 and 0 differ: odd `1..15`, even `16..30`,
odd `33..47`, even `48..62`. Those descriptors are **not** aliases:

- All 32 unreachable-id records point at full-size, valid tile files
  (688..2,718 sectors each, all content-matched).
- Overlap check of those 32 sector ranges against **every record in every table**
  (A–G, including the reachable 32 FLD slots): **0 re-appearances** [PROVEN].
- The only cross-table range overlap anywhere is the benign SCORE dual-registration (below).

**Verdict:** the 2P-mirror / town / night hypotheses in their "re-use the same sectors" form
are **rejected** — the unreachable half of the id space addresses 32 *distinct* tile files
that nothing else references.

**RESOLVED 2026-10-16 (spawn path found):** `FUN_00210158` (town/scene loader,
callers `FUN_002105b8` ×3 and `FUN_0026ca48`) spawns
`gs_tasks_spawn(ctx, 0x20dc50, id | 0x390000)` with id taken from a **scripted
scene→tile-id table** at vaddr `0x2be438`, stride 8, records
`{u32 sceneNamePtr, u16 tileId, u16 extra}` (tileId at +4; the loader reads
`short @ 0x2be43c + i*8`). Decoded ids: 11, 43, 23, 7, 19, 36, 35, 47, 64, 11,
39, … — ids 11/43/7/35/47 are **parity-invalid** (unreachable by
`FUN_0021c8d0`), loading only via scripted scene entry (towns/POIs; id 11 =
`023.BIN`, the 0x160-header tile). The id=64 entry is anomalous (indexes the
COURSE sub-slot of the physical descriptor run) [OPEN]. The `+6` u16 cluster
(0x0Bxx/0x0Axx) semantics remain [OPEN]. Net: the opposite-parity ids have a
real spawn path; parity-valid ids never hit by the car walk are simply
unreachable world space.

## Coverage

- Files matched: **363 / 376 (96.5 %)**; bytes **532,276,192 / 535,918,004 (99.32 %)**.
- Descriptors matched exactly by (LBA, sectorCount): **384 / 384**; unique winner every time
  (the 18-way/4-way FLD size-twins and the SCOREA size-twins are all separated by offset;
  byte-identical twins among FLD files remain interchangeable-content but occupy distinct
  LBAs).
- Per-directory: ACTION 21/21, CAR0..CAR4 150/150, CARS 4/4, COURSE 18/19, FLD 64/64,
  ITEM 12/12, SHOP 22/22, SOUND 29/30, SYS 43/44, top-level 0/10.
- Unclaimed 13: 11 boot/OS artifacts (`SYSTEM.CNF`, `IOPRP234.IMG`, 6 IRX drivers,
  `SNDMOD.IRX`, `SOUND/SNDMOD.IRX`, `SLUS_203.98` — all ISO-path/boot loaded) plus two
  content items: **`COURSE/C16.BIN`** and **`SYS/FONT.GSL`**.
- `C12..C20` question closed: **table F claims them** (records 76–81 = C12,C13,C14,C18,C19,
  C20; C15/C17 simply aren't on the disc), satisfying the `course-data-status.md` pointer.
- **`C16.BIN` is an orphan [PROVEN unreferenced]:** its LBA `0xE2B4` occurs nowhere in the
  ELF — no descriptor slot, no lui/ori/addiu constant, no raw dword anywhere in text or
  data. Its 788 sectors sit in the exact hole between the COURSE block end (C20 ends at
  `0xE2B4`) and the FLD block start (`0xE5C8`). Cut/spare track content, not streamed by
  this build.
- `SYS/FONT.GSL` orphan: Japanese font superseded by `SYS/FONTE.GSL` (B table entry 0); no
  code constant for its LBA either.

## Cross-table duplicates

21 files (SCORE.GSL + SCOREA00..15/17..20) are registered **twice**: in B (misc, scattered
type indices, loaded with the screens that need them) and in C (race-index order 0..20, slot
16 = generic `SCORE.GSL` since no SCOREA16 exists). Same (LBA, count) both times — harmless
double bookkeeping, not contention. This is the *only* duplicated file set in the region.

## Open items

- **C table consumer** — no code constants reference it; dead duplicate vs runtime-computed
  base [OPEN].
- **ACTION types 18..38** through the `0x50000` misc loader — inferred from physical
  adjacency/stride; confirm with one live town-demo load or by tracing task-spawn tag bytes.
- ~~Unreachable tile-id spawn path~~ — **RESOLVED 2026-10-16**: scripted scene table
  @`0x2be438` (fully decoded below, including the former id=64 and `+6` residuals).
- `functions.tsv` naming drift: `0x0020CC30 CheckInputMapping`, `0x0020CD10
  UpdateInputMapping`, `0x0020CDC0 DispatchInputMapping` are actually asset steps
  (car-model reload / reload-variant / shop-pack state machine driving table E and D bases)
  — rename in a docs-sync pass [OPEN].

Method artifacts: scan/match scripts run from `/tmp` (ELF phdr map + tag-run grouping + LBN
join); no game bytes committed.

## Scripted scene->tile table — residuals resolved

**Table boundary and format (PROVEN):** the contiguous record run is exactly
`0x2BE438..0x2BE537`, **32 entries × 8 bytes**.  It begins with the descending
`0x328770, 0x328760, …` pointer column and ends immediately before unrelated
float data at `0x2BE538`.  Each little-endian record is
`{ u32 namePtr, u16 tileId, u8 residentStart, u8 residentCount }`; the old
`extra` spelling below is the convenient packed display
`extra = residentStart | residentCount<<8`.

`FUN_00210158` reads those last two bytes separately (`+6` then `+7`), walks
`residentCount` records starting at `residentStart` through the per-scene
resident/progress lookup, and spawns `0x370400 | residentCount`.  Thus the
apparent `0x0Axx`/`0x0Bxx` cluster is **not a tile property or asset-pack
selector**: it is primarily a count of 10/11, paired with a resident-list
start index.  This resolves the `+6` field's role; the identities/meaning of
every individual resident/progress record remain outside this table.

For ids 0..63, filename is the proven FLD mapping
`id=(A<<4)|(B<<2)|swap2(C)`.  `bad` marks the ids that fail the car-walk
parity condition `id[4] == id[0]`.  The disc extract contains every resulting
`FLD/ABC.BIN`; the human scene strings are locations/POIs rather than alternate
disc filenames.  `SJIS` means the pointed string is non-ASCII Shift-JIS bytes;
it is deliberately not guessed as an English name here.

| # | tile id | FLD filename | name string at `namePtr` | extra | bad |
|---:|---:|---|---|---:|:---:|
| 0 | 11 | `023.BIN` | `Peach Town` | `0x0001` | yes |
| 1 | 43 | `223.BIN` | `Fuji City` | `0x0B1C` | yes |
| 2 | 23 | `113.BIN` | `Sandpolis` | `0x0B1D` | no |
| 3 | 7 | `013.BIN` | `Chestnut Canyon` | `0x0921` | yes |
| 4 | 19 | `103.BIN` | `Mushroom Road` | `0x0B11` | no |
| 5 | 36 | `210.BIN` | `White Mountain` | `0x000B` | no |
| 6 | 35 | `203.BIN` | `Papaya Island` | `0x0A1B` | yes |
| 7 | 47 | `233.BIN` | `LightHouse` | `0x0A1C` | yes |
| 8 | 64 | — (see anomaly) | `Cloud Hill` | `0x0B0E` | n/a |
| 9 | 11 | `023.BIN` | `???` | `0x0617` | yes |
| 10 | 39 | `213.BIN` | `Bridge` | `0x0004` | yes |
| 11 | 40 | `220.BIN` | `UFO` | `0x0004` | no |
| 12 | 22 | `111.BIN` | `Ruins` | `0x0004` | yes |
| 13 | 5 | `012.BIN` | `022` | `0x0105` | yes |
| 14 | 6 | `011.BIN` | SJIS | `0x0004` | no |
| 15 | 9 | `022.BIN` | `022` | `0x0006` | yes |
| 16 | 20 | `110.BIN` | `110` | `0x0002` | yes |
| 17 | 24 | `120.BIN` | `120` | `0x0001` | yes |
| 18 | 33 | `202.BIN` | `202` | `0x0103` | yes |
| 19 | 37 | `212.BIN` | `212` | `0x0004` | yes |
| 20 | 42 | `221.BIN` | `221` | `0x0001` | no |
| 21 | 45 | `232.BIN` | `232` | `0x0001` | yes |
| 22 | 64 | — (see anomaly) | SJIS | `0x1400` | n/a |
| 23 | 47 | `233.BIN` | SJIS | `0x0B00` | yes |
| 24 | 35 | `203.BIN` | SJIS | `0x0B00` | yes |
| 25 | 5 | `012.BIN` | SJIS | `0x0100` | yes |
| 26 | 19 | `103.BIN` | SJIS | `0x0B00` | no |
| 27 | 43 | `223.BIN` | SJIS | `0x0B00` | yes |
| 28 | 40 | `220.BIN` | SJIS | `0x0000` | no |
| 29 | 23 | `113.BIN` | SJIS | `0x0B00` | no |
| 30 | 9 | `022.BIN` | SJIS | `0x0000` | yes |
| 31 | 7 | `013.BIN` | SJIS | `0x0B00` | yes |

**Grid picture:** the 30 in-range entries cover town/POI placements across
`A=0..2` and include 21 parity-invalid uses (repeated ids counted):
`{5, 7, 9, 11, 20, 22, 24, 33, 35, 37, 39, 43, 45, 47}`.
Examples: `11 -> 023.BIN`, `43 -> 223.BIN`, `35 -> 203.BIN`, and
`47 -> 233.BIN`.  This is the concrete scripted route to the formerly
unreachable half of the FLD id space.

**id=64 anomaly (PROVEN, not a FLD tile):** both table entries with `tileId`
64 spawn tag `0x390040`.  Category 9 dispatch has no bounds check and computes
the descriptor as `0x29BB50 + (tag&0xFF)*12`; index 64 is the first physical
record after FLD 0..63, namely **`COURSE/C00.BIN`**.  It is then passed to
`loadFieldTile` with logical id 64 and slot 0, so this is an intentional (or at
least unguarded) cross-subtable load, not a nonexistent `FLD/` file.  More
generally, tags `0x390041..0x390066` would mechanically select the adjacent
COURSE/ACTION descriptor records because the same unchecked base/stride
continues through index 102; this table only demonstrates index 64.  No other
producer of a category-9 non-FLD index was established by this static pass.

**Residual OPEN:** precise gameplay semantics of each `residentStart` list and
why the two entries that select `COURSE/C00.BIN` do so still
need runtime/script evidence.  The table extent, all ids/names, parity route,
descriptor outcome, and byte-level `+6/+7` interpretation are resolved.
