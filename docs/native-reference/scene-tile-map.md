# Scene→tile map — authoritative town/tile identity (SLUS-20398)

Written 2026-10-16 to resolve the Peach Town tile-identity dispute
("Position A: Peach Town = tile id 11 / `FLD/023.BIN`" vs
"Position B: shipped table says Peach Town = scene 1, tile 0x2b = 43 and tile
0x0b = My Garage"). Method: host-side Python parsing of `SLUS_203.98`
(PT_LOAD deltas `0x1ff000` for both segments, re-verified from program headers)
plus ghidra-mcp disassembly; companion probe scripts under `/tmp/rt-scene/`.
ELF fileOffset = vaddr − 0x1ff000 throughout.

## Verdict (short form)

**Position B is right on town identity; Position A's grid machinery is right
but its town binding is wrong.** The shipped scripted scene table at
`0x2be438` says, byte-for-byte:

* **record 0 = "My Garage" — tile id 11 (`FLD/023.BIN`)**;
* **record 1 = "Peach Town" — tile id 43 / 0x2b (`FLD/223.BIN`)**.

So the app's render origin anchored at the tile whose id computes 11 is
anchored on the **My Garage / My City** tile, not Peach Town. Peach Town
lives on tile 43, which — by pure coincidence — shares tile 11's half-column
(`X0 = 0x0b000000`) but sits at row 5 instead of row 1
(`Z0 = 0x0a000000`, i.e. 4 tiles / 6400 units south of the garage tile).

Nav graph 0 (static directory `0x2bd578` pair 0) is **not** a town street
graph at all: the static selector is the COURSE/ACTION **disc-pack type
byte**, so graph 0 belongs to `COURSE/C00.BIN`. Peach Town's town street
graph lives in the **outer** directory `0x2d98a8` at index `T = 1`.

## Authoritative scene table @ `0x2be438` (PROVEN)

32 records × 8 bytes, `0x2be438..0x2be537`:

```
{ u32 namePtr, u16 tileId, u8 residentStart, u8 residentCount }
```

Boundary proof: the preceding table (NPC-name pointer pairs) ends with a zero
record at `0x2be430`:

```
002be430: 00 00 00 00
002be438: 70 87 32 00 0b 00 01 00     idx 0  My Garage     tile 0x0b
002be440: 60 87 32 00 2b 00 1c 0b     idx 1  Peach Town    tile 0x2b
002be448: 50 87 32 00 17 00 1d 0b     idx 2  Fuji City     tile 0x17
```

and the first byte after record 31 is unrelated float data (`0x2be538`,
an 8+ entry f32-pair quad table: `66 06 00 44 cd 0c d8 43` = (512.1, 432.1)).

Every record's `namePtr` points at the string it names; the eight zone-name
strings (`0x328700..0x328778`, sixteen bytes apart) are each referenced by
**exactly one** record and nowhere else in the ELF (whole-file LE pointer
scan):

```
00328770: 4d 79 20 47 61 72 61 67 65 00 ...   "My Garage"    ← idx 0
00328760: 50 65 61 63 68 20 54 6f 77 6e 00 ... "Peach Town"   ← idx 1
```

### Full table (decode: `id=(A<<4)|(B<<2)|swap2(C)`, `swap2: 0→0,1→2,2→1,3→3`)

| idx | vaddr | namePtr | name (string bytes) | tileId | A/B/C | disc file | resStart+Count |
|---:|---|---|---|---:|---|---|---|
| 0 | `0x2be438` | `0x328770` | My Garage | 11 / `0x0b` | 0/2/3 | `FLD/023.BIN` | 1+0 |
| 1 | `0x2be440` | `0x328760` | Peach Town | 43 / `0x2b` | 2/2/3 | `FLD/223.BIN` | 28+11 |
| 2 | `0x2be448` | `0x328750` | Fuji City | 23 / `0x17` | 1/1/3 | `FLD/113.BIN` | 29+11 |
| 3 | `0x2be450` | `0x328740` | Sandpolis | 7 / `0x07` | 0/1/3 | `FLD/013.BIN` | 33+9 |
| 4 | `0x2be458` | `0x328730` | Chestnut Canyon | 19 / `0x13` | 1/0/3 | `FLD/103.BIN` | 17+11 |
| 5 | `0x2be460` | `0x328720` | Mushroom Road | 36 / `0x24` | 2/1/0 | `FLD/210.BIN` | 11+0 |
| 6 | `0x2be468` | `0x328710` | White Mountain | 35 / `0x23` | 2/0/3 | `FLD/203.BIN` | 27+10 |
| 7 | `0x2be470` | `0x328700` | Papaya Island | 47 / `0x2f` | 2/3/3 | `FLD/233.BIN` | 28+10 |
| 8 | `0x2be478` | `0x328488` | Cloud Hill | 64 / `0x40` | — | `COURSE/C00.BIN` | 14+11 |
| 9 | `0x2be480` | `0x3345e8` | My City | 11 / `0x0b` | 0/2/3 | `FLD/023.BIN` | 23+6 |
| 10 | `0x2be488` | `0x3345e0` | `???` | 39 / `0x27` | 2/1/3 | `FLD/213.BIN` | 4+0 |
| 11 | `0x2be490` | `0x3345d8` | Bridge | 40 / `0x28` | 2/2/0 | `FLD/220.BIN` | 4+0 |
| 12 | `0x2be498` | `0x3345d0` | UFO | 22 / `0x16` | 1/1/1 | `FLD/111.BIN` | 4+0 |
| 13 | `0x2be4a0` | `0x3345c8` | Ruins | 5 / `0x05` | 0/1/2 | `FLD/012.BIN` | 5+1 |
| 14 | `0x2be4a8` | `0x3286f0` | LightHouse | 6 / `0x06` | 0/1/1 | `FLD/011.BIN` | 4+0 |
| 15 | `0x2be4b0` | `0x3345c0` | `022` (literal FLD name) | 9 / `0x09` | 0/2/2 | `FLD/022.BIN` | 6+0 |
| 16 | `0x2be4b8` | `0x3345b8` | `110` (literal) | 20 / `0x14` | 1/1/0 | `FLD/110.BIN` | 2+0 |
| 17 | `0x2be4c0` | `0x3345b0` | `120` (literal) | 24 / `0x18` | 1/2/0 | `FLD/120.BIN` | 1+0 |
| 18 | `0x2be4c8` | `0x3345a8` | `202` (literal) | 33 / `0x21` | 2/0/2 | `FLD/202.BIN` | 3+1 |
| 19 | `0x2be4d0` | `0x3345a0` | `212` (literal) | 37 / `0x25` | 2/1/2 | `FLD/212.BIN` | 4+0 |
| 20 | `0x2be4d8` | `0x334598` | `221` (literal) | 42 / `0x2a` | 2/2/1 | `FLD/221.BIN` | 1+0 |
| 21 | `0x2be4e0` | `0x334590` | `232` (literal) | 45 / `0x2d` | 2/3/2 | `FLD/232.BIN` | 1+0 |
| 22 | `0x2be4e8` | `0x3286e0` | クラウド (SJIS "Cloud") | 64 / `0x40` | — | `COURSE/C00.BIN` | 20+0 |
| 23 | `0x2be4f0` | `0x3286d0` | パパイヤ (SJIS "Papaya") | 47 / `0x2f` | 2/3/3 | `FLD/233.BIN` | 11+0 |
| 24 | `0x2be4f8` | `0x3286c0` | ホワイト (SJIS "White") | 35 / `0x23` | 2/0/3 | `FLD/203.BIN` | 11+0 |
| 25 | `0x2be500` | `0x334588` | いせき (SJIS "ruins") | 5 / `0x05` | 0/1/2 | `FLD/012.BIN` | 1+0 |
| 26 | `0x2be508` | `0x334580` | マロン (SJIS "Marron/chestnut") | 19 / `0x13` | 1/0/3 | `FLD/103.BIN` | 11+0 |
| 27 | `0x2be510` | `0x334578` | ピーチ (SJIS "Peach") | 43 / `0x2b` | 2/2/3 | `FLD/223.BIN` | 11+0 |
| 28 | `0x2be518` | `0x334570` | はし (SJIS "bridge") | 40 / `0x28` | 2/2/0 | `FLD/220.BIN` | 0+0 |
| 29 | `0x2be520` | `0x334568` | フジ (SJIS "Fuji") | 23 / `0x17` | 1/1/3 | `FLD/113.BIN` | 11+0 |
| 30 | `0x2be528` | `0x3286b0` | こうそく (SJIS "highway") | 9 / `0x09` | 0/2/2 | `FLD/022.BIN` | 0+0 |
| 31 | `0x2be530` | `0x334560` | サンド (SJIS "Sand") | 7 / `0x07` | 0/1/3 | `FLD/013.BIN` | 11+0 |

Records 22–31 are the Shift-JIS display-name duplicates of the corresponding
English scenes (27 ピーチ ↔ 1 Peach Town, both tile 43; 25 いせき ↔ 13 Ruins,
tile 5; 30 こうそく "highway" ↔ 15 `'022'`, tile 9; …) — an independent
cross-check on which tile each town owns.

### Consumers (instruction-level provenance)

* **`FUN_00210158`** indexes this table by the scripted scene byte at
  `param+0x23` (`0x210208..0x210220`: `sll v0,s5,3; at=0x2c0000-0x1bc8 =
  0x2be438; lb s4,0x7(v0); lb s7,0x6(v0)`), and at `0x2103d0..0x2103f4`
  spawns `gs_tasks_spawn(ctx, 0x20dc50, lh[rec+4] | 0x390000)` — the tag's
  `& 0xf0000` nibble is **0x90000**, i.e. the category-9 FLD loader, with
  descriptor index `tag & 0xff = tileId`.
* **AssetTaskStep `@0x20d120`, category-9 arm at `0x20d700..0x20d724`:**
  `andi v0,t0,0xff; a2 = idx*12; at = 0x2a0000-0x44b0 = 0x29bb50` — the
  descriptor index is used **unchecked and un-swizzled** against table F.
* **`FUN_0020e708`** (scene task init) reads the same record layout
  (`0x20e76c..0x20e79c` reads `+7` count into `task+0x26`), and — notable —
  at `0x20e750..0x20e764` **remaps scene id 0 → 9**
  (`lbu v0,0x23(s7); if (v0 == 0) sb 9, 0x27(s2)`). The "My Garage" record 0
  is therefore executed as the record-9 (My City) load; both records share
  tile 11 anyway.
* **`FUN_00211530`** reads the same `{+6,+7}` resident window (matches
  `car-data-status.md`).

## FLD filename ↔ tile id (and the `011.BIN` question)

Descriptor table F at `0x29bb50` has **103 records** of 12 bytes
`{tag 0x00333388, sectorOffset, sectorCount}`:
records 0..63 = FLD, 64..81 = COURSE (C00..C14, C18..C20 — C15/C17 are not
on the disc), 82..102 = ACTION A00..A20. Joined against
`.lbn_map.tsv` LBAs: **103/103 exact matches, zero ambiguous.**

Ordering proof — records 5 and 6 are swapped relative to naive filename
order:

```
0029bb8c: 88 33 33 00 bf 21 01 00 ed 05 00 00   rec 5  = LBA 74175 → FLD/012.BIN
0029bb98: 88 33 33 00 5e 1c 01 00 61 05 00 00   rec 6  = LBA 72798 → FLD/011.BIN
```

All 64 FLD records match the proven tile-id formula
`id = (A<<4)|(B<<2)|swap2(C)` and **none** match the naive `16A+4B+C`
wherever `C ∈ {1,2}` (64/64 vs 16/64). Hence:

* **`FLD/011.BIN` = tile id 6** (`(0<<4)|(1<<2)|swap2(1) = 4|2`), i.e. scene
  14 **LightHouse**'s tile. The naive reading (id 5) is exactly the
  swap2 pitfall; id 5 is `FLD/012.BIN` (Ruins).
* `FLD/023.BIN` = tile id 11 = scene 0 **My Garage** (and scene 9 My City).
* `FLD/223.BIN` = tile id 43 = scene 1 **Peach Town**.
* Scenes 15–21 carry literal FLD filenames as their name strings, and their
  stored tile ids beat-by-beat match the swap2 formula
  (`'202'`→33 not 34, `'212'`→37 not 38, `'221'`→42 not 41, `'232'`→45 not
  46, `'022'`→9 not 10): the stored u16 **is** the tile id, not the
  filename-order index.

The two relevant fixed-frame extents (proven `world-geography.md` grid):

| tile | A/B/C | b/row/col | `X0`, `Z0` | fixed extent | terrain units |
|---:|---|---|---|---|---|
| 11 (My Garage / My City) | 0/2/3 | 1/1/5 | `0x0b000000`, `0x02000000` | X `[0x0b000000,0x0d000000)` × Z `[0x02000000,0x04000000)` | X `[8800,10400)` × Z `[1600,3200)` |
| 43 (Peach Town) | 2/2/3 | 1/5/5 | `0x0b000000`, `0x0a000000` | X `[0x0b000000,0x0d000000)` × Z `[0x0a000000,0x0c000000)` | X `[8800,10400)` × Z `[8000,9600)` |

Note both tiles decode to the same half-column 11 — arithmetic coincidence
that plausibly seeded Position A (`X0 = 0x0b000000` ≠ tile id 11).

## Which nav graph is Peach's? — none of the static ones

* Static directory `0x2bd578` (24 pairs, 18/21/23 null, code aliases
  0x0b→6 and 0x10→15 in `FUN_0024fcd8` `0x24fdc8..0x24fdd8`) is indexed by
  the **race-record disc-pack type byte**: producer `FUN_0020f950`
  (`0x20fa4c lbu v0,0x0(v1)` → `0x20fa54 sb v0,0x22(s4)`), consumer
  `FUN_0024fcd8` (`lb v1,0x22(s3)` … `sll v1,a3,3; v0 = 0x2c0000-0x2a88 =
  0x2bd578`), and the same byte drives the `0x50000` misc loader against
  base `0x29be50` = table F record 64. Values 0..17 = C00..C14,C18..C20;
  18..38 = A00..A20 (39/39 LBN matches). **Static graph 0 = COURSE/C00.BIN's
  route graph** — a race course, not a town. Selector byte `-1` = none,
  `0x22` (34) = alternate path.
* Town street graphs live in the **outer** directory `0x2d98a8`, 32 u32 dir
  pointers indexed by `T = scene id` (spawn: `FUN_00210430` `0x2104d0..0x2104e0`
  builds `tag = (sceneId << 10) | 0x800000`; selector
  `(tag>>10)&0x1f`, per car slot). Relevant rows:

```
002d98a8: 58 95 2d 00 10 93 2d 00      T=0 → 0x2d9558, T=1 → 0x2d9310
002d9310: f0 5f 2c 00 d0 67 2c 00      T=1 car0 pair {0x2c5ff0, 0x2c67d0}
```

  T=0 and T=9 share dir `0x2d9558` (consistent with the scene 0→9 remap).
  Decoded bounds (segment records = 24 B, 4 × f32 endpoints):

| T | scene (from table above) | car0 segments | x range | z range |
|---:|---|---:|---|---|
| 0/9 | My Garage→My City | 30 | [1096, 1299] | [994, 1083] |
| 1 | **Peach Town** | 84 | [418, 552] | [443, 1084] |
| 2 | Fuji City | (car0 null) | — | — |
| 8 | Cloud Hill | (car0 null) | — | — |

* Cross-check via the per-scene spawn directory `0x2c2e78`
  (u32 per scene; scene 1 entry at `0x2c2e7c` = `0x2c2348`, matching
  `traffic-ai.md`): scene 1 (Peach) spawn record 0 = **(459, 30, 786)**
  — inside the T=1 street-graph box; scene 0/9 shared group `0x2c27d8`
  record 0 = **(1150, 25, 1082)** — inside the T=0/9 box and outside
  Peach's. Scene spawns and outer graphs bind by the same scene index as
  the `0x2be438` table. All coordinate frames are tile-local `[0,1600]²`
  f32, consistent.

So: graph 0 of the static directory (bounds `x=[358,822] z=[328,572]`) is
**not** Peach Town's route graph; it is C00's. If a Peach street graph is
wanted, the candidate is outer T=1 pair `{0x2c5ff0, 0x2c67d0}` (84 segments),
although binding an outer graph to a *name* still rests on the index chain
proven above rather than on a string literal.

## Corrections required (do NOT applied by this pass)

1. `world-geography.md`
   * Lines 119–133 (`## Worked example — Peach Town, id 11 / FLD/023.BIN`):
     the example's **name** is wrong. id 11 = **My Garage**; Peach Town =
     **id 43 / `FLD/223.BIN`**, extent X=`[0x0b000000,0x0d000000)` ×
     Z=`[0x0a000000,0x0c000000)`, units `[8800,10400) × [8000,9600)`.
   * Lines 135–157 (`## Navigation cross-check`): graph 0 is the
     **C00 pack route graph** (static selector = race-record pack-type byte);
     "candidate Peach Town static graph" should be removed; its centering
     on Peach's extent is meaningless. The scene-0/1 association and the
     derived `[9158,9622] × [1928,2172]` bounds correspond to
     `FLD/023.BIN` = My Garage/My City tile, not Peach.
   * Line 161–163 OPEN item "Static graph 0 = Peach": **resolved — false.**
2. `nav-tables.md` line 102–105 and line 169: "scene-table entry 0 as Peach
   Town, tile id 11" — wrong; entry 0 = My Garage. The 35-segment graph is
   C00's route graph.
3. `nav-index-binding.md` lines 60–65 and the outer-directory name column
   (lines 77–110): the table does **not** prove scene 0 = Peach Town; and
   the T→name column is shifted — correct names are T=0/9 My Garage→My
   City, T=1 Peach Town, T=2 Fuji City, T=3 Sandpolis, T=4 Chestnut
   Canyon, T=5 Mushroom Road, T=6 White Mountain, T=7 Papaya Island,
   T=8 Cloud Hill, T=10 `???`, T=11 Bridge, T=12 UFO, T=13 `'022'`-group:
   i.e. T = `0x2be438` record index (with record 14 = LightHouse). Their
   "T=7 LightHouse … T=13 '022'" column skips LightHouse/My City and is one
   off throughout — dir pointers themselves are correct.
4. `asset-map.md` lines 245–278 (`Scripted scene->tile table` name column):
   tile ids are all correct, but the **name column is mis-resolved**
   (row 0 labelled "Peach Town", row 1 "Fuji City", … row 7 "LightHouse",
   row 13/15 both `'022'`). Replace with the namePtr-resolved column above.
5. `nav-drive.md` line 59, `nav-walkable.md` line 29, `nav-route-consumer.md`
   line 21 / README lines 18, 31–32: "static graph 0 (candidate Peach
   Town)" labels should be renamed C00-pack route graph; "Peach Town"
   lap/waypoint claims pertain to that graph only if it is re-labelled.
6. `save-layout.md` line 67: "save-point location: 1 Peach Town … 9 My
   City" — numerically consistent with this table; worth cross-referencing
   here.

## Ledger

**PROVEN:** scene table location/format/extent (`0x2be438..0x2be537`, 32×8);
every namePtr resolution (exactly one reference each for the 8 zone strings,
whole-file scan); tile ids incl. swap2 ordering (scenes 15–21 literal-name
self-test; table F 103/103 LBN join with 5/6 swap evidence); category-9
dispatch chain (scene rec `+4` → `0x39xxxx` tag → `0x9`/`idx*12` → table F);
static nav selector = race pack type (producer/consumer instructions +
F+64 39/39 mapping); graph 0 = C00 pack; outer directory indexing by scene
id; scene 0→9 remap; spawn-directory points lying inside the matching outer
graph boxes for scenes 0/1/9.

**INFERRED:** town-name semantics of the SJIS short forms (translations are
direct kana readings); outer T=1 graph labelled "Peach Town street graph"
via the index chain (no string literal ties graphs to towns).

**OPEN:** semantic role of the `0x2be538` f32 quad table (8 quads ≈
zone regions; quad 7 z≈1053 is the only one overlapping a town graph's z
range — no consumer traced here); per-resident record contents; which race
record (menu name) selects each C00..C20 pack; why record 0 exists at all
given the 0→9 remap (vestigial "garage before city built" state
presumed, untested).
