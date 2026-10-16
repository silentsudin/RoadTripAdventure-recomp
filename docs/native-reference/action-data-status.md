# ACTION/*.BIN data status (Road Trip, SLUS-20398)

First-pass container grammar, 2026-10-16 (inline python probes over the user
disc extract; scripts were throwaway in-session, semantic constants only here —
no game bytes committed). Completes the container trilogy next to
`field-topology-status.md` (FLD) and `course-data-status.md` (COURSE).

## Inventory + disc identity (PROVEN)

21 files `ACTION/A00.BIN..A20.BIN` (32 KB..4.4 MB). Joined against the ELF's
ROM descriptor table via `.lbn_map.tsv` exact LBAs: **records 82..102 of the
`0x29bb50` table map to A00..A20 in order, 21/21 exact
`(sectorOffset, sectorCount)` matches, offsets strictly ascending from sector
`0xFC`** — ACTION content sits at the very front of the data zone (right after
the boot island), and the `0x29bb50` table is physically
FLD(0..63) + COURSE(64..81) + ACTION(82..102) as `asset-map.md` reports.

Misc-loader type space (`FUN_0020c188 FieldTileMiscLoader`, category `0x50000`,
descriptor `0x29be50 + type*12`; `0x29be50 − 0x29bb50 = 64 records`):
type `0..17` → COURSE C00..C14+C18..C20, type **`18..38` → ACTION A00..A20**
(record 64+type). Index identity `type − 18 = file number` — PROVEN by the
record join; the runtime meaning of each index (which scripted sequence loads
which pack) is [OPEN] (likely town/event-scoped, see `SpawnTownEntities`
`0x00210158` which also spawns `0x3704xx` misc packs).

## Header grammar (PROVEN shape, variant semantics partial)

`u32[8+]`: `[0]` header size ∈ {0x20, 0x30}; `[1]=offA`, `[2]=offB`, `[3]=offC`;
`[4]` declared size.

| variant | files | size semantics |
|---|---|---|
| hdr `0x20`, words 5+ unused | A02 A04 A10 A11 A15 A20 | `w4 == physical`, `offC < w4` |
| hdr `0x20`, `w5 == physical` | A07 A08 A19 | `w4 < w5 == phys` (trailing padding beyond declared size) |
| hdr `0x30`, extra boundaries w5/w6(/w7) | A00 A01 A05 A06 A13 A16 A18 | one of `w6`/`w7` equals physical size (A06 needs `w7`; w6 = an interior boundary `0xe2930` beyond `w4`); same family as the COURSE `0x30` variant |
| hdr `0x20`, **`w4 == 0`** | A03 A09 A12 A14 A17 | `phys == offC − 8` exactly in all five — `offC` is a past-the-end pointer including an 8-byte DMA terminator qword not present in the file [OPEN: intended size source for these five] |

`offA` is always 0x800-sector-aligned (0x13800..0x9b800) — unlike FLD's small
`offA`; ACTION carries a large pre-chain blob.

## Region roles (inferred where marked)

- **`[hdrsz, offA)`** — zero MSCALF occurrences (WorldStrip VIF call) in capped
  scans → not the vertex-chain region; consistent by loader-family analogy with
  FLD's GIF IMAGE upload/palette blob [INFERRED: BITBLT grammar untested here].
- **At `offA`** — FLD-style directory, then VIF chains: MSCALF counts (cmd
  `0x15`, imm 8) in `[offA, offA+0x80000)` = 193..2481 per file (scan-capped) —
  per-cell/per-entity chain streams run from `offA` to `offB`.
- **At `offB`** — first word is `0x608` in **21/21** files: PMP model block,
  new-style magic only (FLD uses 0x600/0x608; ACTION never 0x600).
- **At `offC`** — trailing table with the COURSE prologue family:
  i16 `(48, 0, 80, 0, ...)` in 10 files, `(16, 0, X, ...)` in 6, garbage in the
  five `w4==0` files (their table, if any, starts elsewhere) — same
  `0x10000000`-template record family as COURSE (route semantics pending the
  COURSE parse pass).

## Open items

- `w4 == 0` variant: intended total size + why `offC` overshoots by 8.
- Texture upload grammar inside `[hdrsz, offA)` — run `fieldTextureCatalog()`
  rules (A+D BITBLTBUF/TRXPOS/TRXREG/TRXDIR + IMAGE payloads) over ACTION and
  check the GS bank rule vs FLD's `0x285e + lastDigit*0x420`.
- Chain directory: verify 65-entry directory + 60 B vertex records corpus-wide
  (`extractCellStrips` assumptions) and material header coverage.
- Per-index semantics (what A00..A20 *are*: pedestrian/effect/prop groups per
  town/event) — needs consumer tracing through the misc-loader completion path.
- Trailing-table schema shared with COURSE — cross-link when
  `course-routes.md` lands.
