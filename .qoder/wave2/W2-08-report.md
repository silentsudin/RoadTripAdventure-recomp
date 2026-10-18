# W2-08 (worker W2-08b) — save format v2

W2-08 SCORE roundtrip=6/6 migration=8/8 fuzz_runs=1000 fuzz_recover_violations=0 elf_pins=16/16 deterministic=1

Run: `cmake -S src/native -B build/w-W2-08b -DRT_NATIVE_ASSETS_BUILD_TESTS=ON && cmake --build build/w-W2-08b --target w2_save_roundtrip_test -j8 && build/w-W2-08b/w2_save_roundtrip_test`
(exit 0; `RT_ELF=/nonexistent` → prints `NO_ELF`, elf_pins=0/0, exit 0). Second line `W2-08 DETAIL recovered=997 unrecoverable=3 strict_miss=0 trunc=219/219`.
`gameplay/save_v2.cpp` is already in the CMake staged-source list (no CMake edit needed).

## IMPORTANT: the brief's premises did not match the repo
* `docs/native-reference/save-blocks.md` did not exist (I created it containing only the v2 section + a note).
* `save.{h,cpp}` has **no** FNV-1a header checksum, no per-record crc32, no "6 slots × 2048 B + 64 B header".
  The real v1 is the raw **0x3448 (13,384) byte** progress block, 2 host slots (`memcard.h`), no magic/version/checksum (`save-layout.md`).
* ELF 0x1F7610 / 0x1F7E48 / 0x1F7F18 lie **below** the first PT_LOAD (vaddr 0x200000 ↔ file 0x1000), so they are not code
  addresses in this ELF. The real card I/O is `MemcardReadSlot 0x228510`, `MemcardCreateSlotTask 0x228708`, `0x2283E0`;
  `SystemSavedataTask 0x213638` was not used.
I therefore followed `.qoder/wave2-plan-sonnet.md` §2.8 (container around the unchanged 0x3448 block, v1 = raw block) and
authored the header/crc layout myself (crc32 of TLV at hdr+16, file crc at hdr+28, per-record crc in a record table). Please confirm this is acceptable.

## Design (details in docs/native-reference/save-blocks.md "v2")
Header(32) | table 14×{id,flags,crc32} | image 0x3448 | TLV. Strict `verifyV2` = fileCrc + all record crcs + TLV crc.
Lenient `loadV2`: bad record → that record's new-game default slice, other records survive; bad TLV dropped;
unusable header → full new-game defaults. `migrateV1ToV2` preserves the 13,384 bytes exactly. Unknown TLV tags preserved.

## Results
* roundtrip 6/6: new-game, mid-game, full-stamps, max-money, longest-names, all-0xFF; odd ones carry TLV with an unknown tag; re-serialization byte-identical.
* migration 8/8: 6 saves → v2, image identical, FNV-1a-64 golden of v2 bytes pinned in the test; wrong-size input refused; single damaged record (id 7) resets only that record.
* fuzz 1000 seeded iterations (1–4 byte XOR flips anywhere in the file; every 10th restricted to the image): strict verify caught 1000/1000; 997 recovered / 3 header-unusable→defaults; 0 violations of: money ≤ 999999, licence/radio/location/mail/towns/photo ranges (via `migrateToNative`), untouched records identical, damaged records == default and listed in `resetRecords`, recovered state re-serializes to a strict-valid file. 219/219 truncations rejected.
* deterministic=1: whole suite run twice, transcript hash equal.

## ELF pins (file offset = vaddr − 0x1FF000; raw little-endian words)
| vaddr | raw word | constant |
|---|---|---|
| 0x2277C4 | 24063448 | `addiu a2,zero,0x3448` memset size in InitSaveSlotDefaults = kSaveSize |
| 0x2285D4 | 24063448 | sceMcRead size (MemcardReadSlot) |
| 0x228A38 | 24063448 | sceMcWrite size (MemcardCreateSlotTask) |
| 0x23DC88 | 3C03000F | money cap hi (SpendTimerBudget) |
| 0x23DC8C | 3463423F | money cap lo → 0xF423F = 999999 = kMoneyCap |
| 0x229FFC | 240303E8 | new-game money 1000 |
| 0x2279FC | 241603FF | towns_unvisited 0x3FF |
| 0x2279B0 | 34A5DFFF | door mask idx1 0x3DFFF (lui 0x227990 = 3C050003) |
| 0x2279B4 | 34C6FBFF | door mask idx2 0x7FBFF |
| 0x2279C8 | 354AFFDF | door mask idx9 0x7FFFDF |
| 0x2279E4 | 24100FBF | door mask idx4 0xFBF |
| 0x2279E8 | 2411017F | door mask idx8 0x17F |
| 0x2279D0 | 240D005A | default byte 0x5A at image 0x12DE |
| 0x227804 | 24030040 | remap[0] = 0x40 |
| 0x227818 | 240A0100 | 0x100 constant (remap[9]) |
| 0x2EB5B0 | (26-byte string, CRC-32 3A7AD250, NUL at +26) | `/BASLUS-20398/BASLUS-20398` save path (hash only, no bytes committed) |

## Files
New: `src/native/gameplay/save_v2.{h,cpp}`, `tests/native_w2/save_roundtrip_test.cpp`, `docs/native-reference/save-blocks.md` (new file, v2 section only), this report. No existing file modified. Build dir `build/w-W2-08b`.

## Open items
1. Confirm the v2 layout (brief described a different, non-existent v1/v2 layout) — if a 6-slot/2048-B format is really wanted, the premise needs a source.
2. Race-results default: decomp stores 0xFF at 78 bytes from 0xFF0 (3×26), but `save.h` documents 35 bytes (`kRaceResultCount`). v2 default follows the decomp; the typed model should be reconciled.
3. `newGameImage()` is authored from the decomp word/byte stores of 0x2277B0/0x229FD8; the currency-name default (copied from ELF data at 0x3335F8..) and any unmapped stores remain zero, so it is not a byte-exact `InitSaveSlotDefaults` image. Record defaults for opaque records 0/12 are zero.
4. "owned flags boolean": this save has no byte-per-flag arrays (all flags are bitmaps), so only range clamps from `migrateToNative` are asserted.
5. Atomic-write / TLV semantics for native options (volumes, rumble) not implemented (offsets OPEN per save-layout.md); only the generic TLV container exists. Integration into memcard.cpp is a coordinator step.
