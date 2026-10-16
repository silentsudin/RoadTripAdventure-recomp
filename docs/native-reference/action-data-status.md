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
| hdr `0x20`, **`w4 == 0`** | A03 A09 A12 A14 A17 | `phys == offC − 8` exactly in all five; size comes from the ROM descriptor's `sectorCount` (see Open items) — `offC` is a past-the-end pointer including an 8-byte DMA terminator qword not present in the file |

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

- ~~`w4 == 0` variant: intended total size + why `offC` overshoots by 8.~~
  **RESOLVED 2026-10-16 (size source):** all five `w4 == 0` files are covered
  by their ROM descriptor records — `sectorCount × 2048 ≥ physical` with
  `(physical + 2047) / 2048 == sectorCount` exact (the same join that matched
  21/21). Load size therefore comes from the `0x29bb50` descriptor, making the
  in-file size word redundant for ACTION. Remaining nuance [OPEN-lean]:
  `offC == phys + 8` likely counts an 8-byte DMA terminator qword the loader
  materializes in memory (see the FLD loader's buffer-relativize step) rather
  than a byte physically stored in the file.
- ACTION's pre-chain blob is not in the FLD GIF-upload grammar.  Identify its
  producer/consumer (or a separate texture-upload path); no GS page-placement
  rule can be inferred from it.
- Decode the `MSCALF 6` VIF program and its 21-/51-vector input batches before
  treating every ACTION V3 payload as a 60-byte WorldStrip vertex stream.
- Resolve ACTION material headers' TEX0 pages to the loader's texture source;
  they establish sampler classes but have no matching in-file GIF IMAGE upload.
- Per-index semantics (what A00..A20 *are*: pedestrian/effect/prop groups per
  town/event) — needs consumer tracing through the misc-loader completion path.
- Trailing-table schema shared with COURSE — cross-link when
  `course-routes.md` lands.

## Grammar validation (python pass)

Read-only Python pass, 2026-10-16, over the 21-file user disc extract
`ACTION/A00.BIN..A20.BIN`.  The probe was kept under `/tmp`; it checked the
whole `[headerSize, offA)` interval for FLD's aligned GIF packet grammar
(packed A+D `BITBLTBUF`/`TRXPOS`/`TRXREG`/`TRXDIR`, followed by FLG=IMAGE),
then followed all directory-targeted DMA/VIF chains through their terminators.

### Pre-chain GIF uploads: absent

There are **0 / 0** byte-exact FLD IMAGE transfers in ACTION.  In particular,
there are zero complete A+D setup groups, so there are no decoded
`(tbp0/tbw/psm/w/h)`, `cbp`, or IMAGE payload-byte values from this region.
The FLD bank rule `0x285e + lastDigit*0x420` is therefore **untestable**, not
merely mismatched: ACTION supplies no candidate destination page at all.

| file | `[hdrsz,offA)` bytes | FLD-style uploads | IMAGE bytes |
|---|---:|---:|---:|
| A00 | `0x14fd0` | 0 | 0 |
| A01 | `0x4ffd0` | 0 | 0 |
| A02 | `0x6a7e0` | 0 | 0 |
| A03 | `0x297e0` | 0 | 0 |
| A04 | `0x937e0` | 0 | 0 |
| A05 | `0x277d0` | 0 | 0 |
| A06 | `0x15fd0` | 0 | 0 |
| A07 | `0x9b7e0` | 0 | 0 |
| A08 | `0x85fe0` | 0 | 0 |
| A09 | `0x7cfe0` | 0 | 0 |
| A10 | `0x237e0` | 0 | 0 |
| A11 | `0x51fe0` | 0 | 0 |
| A12 | `0x9afe0` | 0 | 0 |
| A13 | `0x2dfd0` | 0 | 0 |
| A14 | `0x3dfe0` | 0 | 0 |
| A15 | `0x137e0` | 0 | 0 |
| A16 | `0x717d0` | 0 | 0 |
| A17 | `0x767e0` | 0 | 0 |
| A18 | `0x1a7d0` | 0 | 0 |
| A19 | `0x4ffe0` | 0 | 0 |
| A20 | `0x6b7e0` | 0 | 0 |
| **total** | **`0x6b64f0`** | **0** | **0** |

This falsifies the earlier loader-family inference that this large region was
an FLD-like texture/palette blob.  It does not establish that it contains no
texture data in another encoding.

### `offA` directory and VIF chains: FLD primary grammar holds

At `offA` in **21/21** files, the first 65 little-endian u32s are nonzero,
16-byte-aligned, ascending relative offsets; entry 0 is `0x190` in every
file.  All 1,365 targets decode as VIF DMA chains: 1,366 byte-3 `0x10` CNT
frames (one A18 chain spans two frames), followed by 1,365 byte-3 `0x60` RET
terminators.  Thus the prompt's `0x10` is a CNT tag here, not a DMA REF, and
no `0x70` END terminator is required by this corpus.

The normal batch is exact FLD WorldStrip shape:

```text
UNPACK V4-32 (0x6c), NUM=1, @TOP+0
UNPACK V3-32 (0x68), NUM=5*n, @TOP+1
MSCALF 8
```

There are **77,474** `MSCALF 8` calls; every one follows that form, so its V3
payload is `5*n*12` bytes = **60*n bytes**, preserving the five-vector
`(x,height,y) | colA | colB | colC | (s,t,alphaAdd)` record contract.  The
directories have variable numbers of live chains (for example A02/A04/A05/
A09/A16/A17/A18/A20 have 65); empty targets are still valid CNT/RET chains.

### Secondary VIF-program variant

Ten files also select VU entry 6, so the 60-byte conclusion is deliberately
limited to `MSCALF 8`:

| class | files | exact post-tag V3 input | calls |
|---|---|---|---:|
| WorldStrip | all A00--A20 | `NUM=5*n`, `MSCALF 8` | 77,474 |
| alternate | A02 A04 A05 A09 A11 A13 A16 A17 A20 | `NUM=21`, `MSCALF 6` | 3,049 |
| alternate (A06 form) | A06 | `NUM=51`, `MSCALF 6` | 16 |

The alternate counts total **3,065**.  Both 21 and 51 are divisible by three,
not five; their semantic record stride remains OPEN pending the VU program-6
decode.  This is independent of the container header `0x20`/`0x30` variant:
both header forms occur in the two VIF classes.

### TEX0 classes from chain material headers

Although ACTION has no FLD-style staged IMAGE pages, its `UNPACK V3-32
@TOPS+0x192` material headers contain **5,198** TEX0 A+D records.  Decoding
TEX0 as `tbp0@0:14, tbw@14:6, psm@20:6, tw@26, th@30, cbp@37:14` gives these
corpus classes (counts are material-header occurrences):

| PSM | distinct `(w,h): count` | total |
|---|---|---:|
| `0x14` PSMT4 | 32x16:74, 32x32:178, 32x64:25, 32x128:1, 64x32:175, 64x64:1,654, 64x128:15, 64x256:4, 64x512:3, 128x32:1, 128x64:395, 128x128:1,403, 128x256:72, 256x64:1, 256x128:30, 256x256:205, 512x512:22 | 4,258 |
| `0x13` PSMT8 | 32x16:82, 32x128:1, 64x32:48, 64x64:281, 64x128:35, 64x256:64, 64x512:1, 128x64:19, 128x128:115, 256x128:81, 256x256:134, 512x256:6 | 867 |
| `0x01` PSMCT24 | 32x128:9, 64x64:55, 128x64:6, 128x128:3 | 73 |

All 34 observed `(psm,w,h)` classes have power-of-two dimensions.  PSMT4 and
PSMT8 therefore hold as sampler-format classes relative to FLD, but ACTION's
larger 512-pixel dimensions and PSMCT24 material use are additional observed
cases; their TEX0 TBP0/CBP values cannot be validated against an ACTION
in-file upload catalog yet.

### Verdict

| FLD assumption | ACTION result | evidence |
|---|---|---|
| pre-`offA` is A+D BITBLT/TRX/IMAGE staging | **BREAK** | 0 complete groups / 0 IMAGE transfers in 21 files |
| FLD GS bank base/`0x420` digit rule | **UNTESTABLE** | no ACTION upload DBP exists under that grammar |
| 65-entry relative directory at `offA`; entry 0=`0x190` | **HOLD** | 21/21 files, 1,365 targets |
| CNT inline frame plus RET chain termination | **HOLD, terminology corrected** | `0x10` CNT: 1,366; `0x60` RET: 1,365; no required `0x70` |
| V3-32=`0x68`, V4-32=`0x6c` | **HOLD** | all parsed batch inputs use these opcodes |
| `MSCALF 8` five-vector / 60-byte vertex stride | **HOLD, scoped** | 77,474 calls, each `V4(1), V3(5*n)` |
| every VIF batch is WorldStrip | **BREAK** | 3,065 `MSCALF 6` batches in the ten-file alternate class |
