# Adventure save layout — SLUS-203.98

Updated 2026-10-16. This is an offset/semantic reference for importing the
original Adventure progress file into the native host-slot store.  It deliberately
contains no game-byte dumps.

## Record and card container

| item | value | status / evidence |
|---|---:|---|
| record size | `0x3448` (13,384) bytes | **PROVEN.** `FUN_00228708` calls `sceMcWrite(fd, image, 0x3448)`; `FUN_00228510` calls `sceMcRead(fd, image, 0x3448)`. |
| RAM base selector | global pointer at EE `0x0177F760` (`gs_progress_ram_address`) | **PROVEN.** `FUN_00229F70` selects its value from `PTR_DAT_002ED724`; all accesses below are relative to the selected image. |
| RAM image index stride | `0x3448` | **PROVEN.** `FUN_002277B0(slot)` computes `slot * 0x3448`, zeroes exactly that range, then installs defaults. |
| byte order | little-endian for multibyte values | **PROVEN.** R5900 program language is `r5900:LE:32:PS2`; direct u16/u32 stores establish the integer fields. |
| save filename | `/BASLUS-20398/BASLUS-20398` | **PROVEN.** Literal opened by read (`FUN_00228510`) and write (`FUN_0022A168` → `FUN_00228708`) paths. |
| card directory | `/BASLUS-20398` | **PROVEN.** `FUN_00228708` creates it with `sceMcMkdir`. |
| ancillary files | `/BASLUS-20398/icon.sys`, `/BASLUS-20398/choroQ.ico` | **PROVEN.** The same task creates and writes the former (`0x3C4` bytes from `0x29ED20`) and latter (`0x8158` bytes from `0x100000`) before the record. They are not part of the `0x3448` record. |

`src/native/gameplay/memcard.h` intentionally models only two raw `0x3448`
host-slot images; it does not model either icon file.

### Original I/O path

1. `FUN_002277B0 InitSaveSlotDefaults` clears a selected `0x3448` RAM image and
   writes defaults. `FUN_00229FD8` invokes it for indices 0 and 1.
2. `FUN_00228510` opens the fixed record path read-only, issues `sceMcRead` into
   the selected image, then `FUN_002283E0` waits for completion.
3. `FUN_0022A168` opens the same path write-only and hands off to
   `FUN_00228708`; its write state issues `sceMcWrite(..., 0x3448)`.

The task parameter packs **memory-card unit in byte 0** and **RAM-image/player
index in byte 1**.  `FUN_00228708` uses byte 0 for `sceMcMkdir`/`sceMcOpen` and
byte 1 to select the `0x3448`-strided source image; its debug line is `SLOT %d
PLAYER %d`. Thus the two native host slots correspond to the two default RAM
images (indices 0/1), not to two differently named files in a single original
card directory. Original saves are selected by card unit plus this image index;
the observed on-card pathname is fixed.

## Field map

Offsets are absolute within the `0x3448` record. Widths are byte widths unless
noted. “Writer/evidence” names the gameplay/default code which establishes the
interpretation; a direct field writer is stronger than a layout-only correlation.

| offset | width / encoding | meaning | writer / evidence | status |
|---:|---|---|---|---|
| `0x00D` | 6 × u8 | equipped parts (engine is at `0x00E`) | gameplay save-state field map; part indices feed garage/race state | INFERRED |
| `0x0508` | 16-byte bitmap | Quick-Pic photos; photo *n* = bit *n-1* | photo code `0x23C5C0` sets and `0x23D8E8` tests this region | PROVEN |
| `0x0518` | 2 × u64 bitmap | notebook stamps 1–100 at bits 0–99 | stamp award `0x23C8A8` uses the earned set | PROVEN |
| `0x0528` | u32 flags | town-entered/story-related bits | state correlations exist, but individual bit meanings have not all been traced | INFERRED |
| `0x0538` | 16-byte flags | story flags; low u32 is current scene; WGP stage *n* uses bit `0x65+n-1` | script flag consumers and WGP stage code | PROVEN (flag set); scene role INFERRED |
| `0x0548` | 16-byte bitmap | missing Choro Q coins; found = `100 - popcount` | status display consumes it as a missing bitmap | PROVEN |
| `0x05A8` | 128 × u8 | mailbox: low 7 bits mail id, bit 7 read | mail handling; President body also occupies this area | PROVEN |
| `0x0628` | u32 | received-mail count, maximum 128 | mail counter code (`0x23DF38`) | PROVEN |
| `0x062C` | 19-byte NUL string | player name | new-game naming path | PROVEN |
| `0x063F` | 18-byte safe NUL string span | currency-name text | licence at `0x651` overlaps the historical 21-byte description, so importer must not overwrite `0x651` | PROVEN (start/overlap); length 18 is conservative |
| `0x0651` | u8 enum | licence: 0 none, 1 B, 2 A, 3 Super A | race-menu rank comparisons | PROVEN |
| `0x0654` | u32 | money; new game 1000; cap 999999 | `FUN_00229FD8` seeds 1000, spend/budget code caps it | PROVEN |
| `0x0658` | u32 metres | odometer/mileage | summary divides by 1609 for miles | PROVEN |
| `0x0674`, `0x0676` | u8 each | World Grand Prix teammates; 0 = none | WGP script opcode checks both | PROVEN |
| `0x0BD8` | 10 × u32 bitmaps | unvisited house doors per location; set bit = not visited | `FUN_002277B0` writes new-game masks; door traversal clears bits | PROVEN |
| `0x0FF0` | 35 × u8 | Adventure race place minus one; `0xFF` = not raced | race-table result consumers | PROVEN |
| `0x1040` | 78 × u32 frames | best lap; `0x7fffffff` = none | `FUN_002277B0` initializes sentinel | PROVEN |
| `0x1178` | 78 × u32 frames | best total time; `0x7fffffff` = none | `FUN_002277B0` initializes sentinel | PROVEN |
| `0x12C8` | u32 metres | White Mountain ski-jump record | ski-jump result save field | PROVEN |
| `0x12DA` | u16 bitmap | towns not yet reached; new game `0x03ff` | `FUN_002277B0` seeds `0x03ff` | PROVEN |
| `0x12DC` | u8 | save-point location: 1 Peach Town … 9 My City | load/save-point selection | PROVEN |
| `0x12DD` | u8 enum | radio: 0 off, 1 Peach FM, 2 E-Radio | pause/radio consumers | PROVEN |
| `0x12E2` | u16 | last photo index, zero-based | photo state | PROVEN |
| `0x12E8` | 10 × u16 button masks | controller remap | `FUN_00229F20` directly initializes offsets `0x12E8..0x12FA` | PROVEN |
| `0x3300` | u32, followed by unpinned entries | queued-stamp count (up to eight u16 entries) | stamp queue code | PROVEN (count); entries OPEN |
| `0x3310` | u32, followed by unpinned records | WGP stages-run/result records | WGP standings consumers | PROVEN (count); records OPEN |

### Options: sound and rumble

**OPEN.** This pass did not prove sound-volume, rumble-enable, or other global
options as offsets inside the `0x3448` Adventure image. Do not manufacture them
from the controller-remap fields. Preserve all unrecognised bytes when importing
an image; a future trace must follow the options-menu writers to the active
progress pointer before adding such mappings.

## Checksum / integrity verdict

**No in-record checksum or CRC is proven; none is currently required for import.**

* `FUN_00228708` writes the complete image directly with one `sceMcWrite`.
* `FUN_00228510` reads the complete image directly; `FUN_002283E0` rejects a
  short read (`< 0x3448`) but contains no checksum/CRC calculation or comparison.
* No checksum/CRC-named string or call was found in the card read/write task
  cluster, and no tail field has been identified as an integrity value.

This is a bounded static result, not proof that no other code ever checks save
semantics. Treat bytes `0x0000..0x3447` as payload and retain unknown ranges.

## Migration specification

1. Obtain the **`BASLUS-20398` file itself**, not `icon.sys` or `choroQ.ico`.
2. Require `file_size == 0x3448` for the native two-slot format. The original
   loader only rejects short reads, but exact size prevents silently truncating
   an extended/corrupt host extraction.
3. Copy the complete record verbatim into the target host slot first. Decode and
   expose the table fields in little-endian form; do not rebuild an image from
   only known fields, since much of the record remains unpinned.
4. Validate migration-critical ranges:
   - money `0x654` ≤ 999999 (clamp/report otherwise);
   - licence `0x651` ∈ 0..3;
   - radio `0x12DD` ∈ 0..2;
   - location `0x12DC` ∈ 1..9 when nonzero is required by the destination;
   - `towns_unvisited & ~0x03ff == 0`;
   - mail count ≤ 128;
   - race-result bytes are `0xFF` or a valid zero-based place for their race;
   - best-time entries are `0x7fffffff` or plausible positive frame counts.
5. Preserve every other byte—including all unpinned option candidates, queue
   entries, WGP records, padding, and future-discovered data—unchanged.

## Evidence ledger

### PROVEN

* Container pathname/directory, icon sidecars, exact write length, read target,
  RAM-image stride, and two default image initializations (`0x2277B0`, `0x228510`,
  `0x228708`, `0x229FD8`).
* ELF segment conversion was verified with Python against the supplied ELF:
  both `PT_LOAD` segments have `vaddr - file_offset = 0x1ff000` (not `0x200000`).
* Money, licence, collectibles, story/WGP flags, race data, location/radio, and
  controller remap mappings listed as PROVEN above.

### INFERRED

* Equipped-part category semantics at `0x00D`.
* The exact per-bit town meaning at `0x0528`; the scene interpretation of the
  low word at `0x0538`.
* The native two-host-slot correspondence to the original pair of RAM images:
  original card-unit selection and RAM-image selection are distinct axes.

### OPEN

* Exact sound/rumble/options offsets and encodings.
* Complete semantics for queue/WGP variable records and all otherwise unknown
  bytes.
* Whether a semantic validator exists outside the observed load/write task
  cluster; no checksum was found here.

## Migration readiness

**80% for lossless two-slot ingestion; 65% for fully typed semantic migration.**
The record boundary, path, byte order, major progress/unlock/collectible fields,
and safe lossless policy are established. Remaining risk is unpinned settings
and variable record bodies, which must be carried through verbatim.

## Native original-card migration importer (AUTHORED, 2026-10-16)

`src/native/gameplay/save.{h,cpp}` exposes these host-only APIs:

* `scanP2RoadTripSaves(span)` finds the **CONFIRMED** game directory/file pair
  below in a P2 card image and returns the raw progress file.
* `migrateFromP2Image(span, slot)` validates native host slot `0` or `1`, finds
  that pair, and parses its exact `0x3448` payload into `SaveData`.
* `migrateFromSaveFile(span)` imports an extracted payload; the `path` overload
  additionally requires basename `BASLUS-20398`.

The importer preserves `SaveData::raw` byte-for-byte, including every unpinned
range. Existing `migrateToNative` normalization is applied only to its typed
view (money, enum/range caps); callers that serialize the normalized `SaveData`
intentionally receive that existing native-valid policy. No PS2 or game bytes
are copied into this repository.

### Game-specific identity: CONFIRMED from SLUS-203.98

The ELF uses one fixed save pathname, not per-slot filenames:

| literal | ELF vaddr | use / status |
|---|---:|---|
| `/BASLUS-20398/BASLUS-20398` | `0x002EB5B1` | loader pathname — **CONFIRMED** |
| `/BASLUS-20398` | `0x002EB691` | created directory — **CONFIRMED** |
| `/BASLUS-20398/icon.sys` | `0x002EB6D1` | icon sidecar — **CONFIRMED**, ignored by importer |
| `/BASLUS-20398/choroQ.ico` | `0x002EB751` | icon sidecar — **CONFIRMED**, ignored by importer |
| `SLOT %d PLAYER %d` | `0x002EB67C` | card-unit and RAM-image axes — **CONFIRMED** |

These addresses use the verified conversion `vaddr = fileOffset + 0x1FF000`.
The `slot` parameter maps only to native's two host slots; it does not invent
different original per-slot filenames.

### P2 container: AUTHORED public-spec reader

The P2 scanner is an **AUTHORED** read-only implementation of the public Ross
Ridge/myMC PS2 memory-card format description: `Sony PS2 Memory Card Format`
superblock, little-endian geometry, two-level indirect FAT, 512-byte directory
entries, and data-only (`512`) or raw NAND (`528`, with a 16-byte spare area)
pages. It follows the root directory and accepts only an existing
`BASLUS-20398` directory containing an existing regular `BASLUS-20398` file.
It rejects unsupported geometry, truncated/cyclic chains, wrong title, wrong
record size, and invalid native slot with typed `SaveErrorCode` values.

P2 NAND spare-area ECC is **not validated**: it is public container-level ECC,
not an Adventure record checksum, and no physical card sample exists here to
validate the accepted/rejected spare-byte policy. The importer strips raw spare
bytes before filesystem decoding. There is likewise no `ChecksumMismatch`
record error because the bounded game-code result above found no game checksum
or CRC in the save read/write task (`FUN_00228510`, `FUN_00228708`,
`FUN_002283E0`).

### Synthetic validation only: INFERRED/AUTHORED fixture coverage

`tests/native_assets/test_save.cpp` constructs a synthetic P2 image using the
public layout, the ELF-confirmed names, an indirect FAT, directory chains, and
a synthetic `0x3448` record. It verifies scan → import → complete raw-record
comparison and direct extracted-file import, plus invalid slot, magic, title,
and size errors. **No original card/save bytes were available**, so these facts
remain unvalidated until a real save sample exists: actual card geometry/FAT
variants; real directory timestamps/mode variations; raw NAND ECC behavior;
and whether any out-of-cluster semantic validator runs elsewhere in the game.
