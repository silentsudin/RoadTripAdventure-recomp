# Save blocks

> This file did not exist when W2-08b ran (the task brief cited a "§2" with a
> 6-slot / 2048-byte / FNV-1a / hdr+16 crc layout; no such document, constant or
> code exists in the repo). The only grounded v1 is the raw 0x3448-byte block in
> [save-layout.md](save-layout.md) / `src/native/gameplay/save.{h,cpp}`. This file
> currently holds only the authored v2 section below.

## v2

Authored native container (`src/native/gameplay/save_v2.{h,cpp}`); the 0x3448 progress block is carried unchanged.
v1 = the raw 13,384-byte block (no magic/version/checksum, as the PS2 file).

File = Header(32) | RecordTable(8 × 14) | Image(0x3448) | TLV(tlvSize); little-endian.

| offset | size | field |
|---:|---:|---|
| +0 | 4 | magic `RTN2` |
| +4 | 2 | version = 2 |
| +6 | 2 | recordCount = 14 |
| +8 | 4 | imageSize = 0x3448 |
| +12 | 4 | tlvSize |
| +16 | 4 | tlvCrc32 (CRC-32/IEEE of the TLV bytes) |
| +20 | 4 | saveSeq (1 after v1 migration) |
| +24 | 4 | reserved = 0 |
| +28 | 4 | fileCrc32 (whole file, this field zero) |

Record table entry: `u16 id, u16 flags=0, u32 crc32(image slice)`. Records partition the image:

| id | image range | content |
|---:|---|---|
| 0 | 0x0000–0x0508 | core + parts |
| 1 | 0x0508–0x05A8 | photos / stamps / story flags / coins-missing |
| 2 | 0x05A8–0x062C | mail inbox + count |
| 3 | 0x062C–0x0674 | name, currency, licence, money, mileage |
| 4 | 0x0674–0x0A50 | WGP + misc A |
| 5 | 0x0A50–0x0BD8 | WGP + misc B |
| 6 | 0x0BD8–0x0FF0 | unvisited-door masks + masks |
| 7 | 0x0FF0–0x1040 | race results |
| 8 | 0x1040–0x1178 | best lap ×78 |
| 9 | 0x1178–0x12B0 | best total ×78 |
| 10 | 0x12B0–0x12E8 | ski / towns / location / radio |
| 11 | 0x12E8–0x12FC | controller remap |
| 12 | 0x12FC–0x3300 | tail A (unpinned) |
| 13 | 0x3300–0x3448 | stamp queue / WGP records |

**Recovery.** Strict `verifyV2` needs fileCrc, every record crc and the TLV crc to match. Lenient `loadV2`
resets only records whose crc fails to that record's new-game default (the matching slice of `newGameImage()`),
drops a TLV block with a bad crc/framing, and returns new-game defaults if magic/version/layout/length are
unusable. Unknown TLV tags are preserved byte-for-byte. `toSaveData` applies `migrateToNative` clamps
(money ≤ 999999 etc.) on read; stored payload bytes are never rewritten by migration.

**Defaults** (ELF `InitSaveSlotDefaults` 0x2277B0, `InitProgressRamDefaults` 0x229FD8; offset = EE addr − 0x177F760):
money 1000 (0x654); name "UnnamedRacer" (0x62C); coins-missing 100 bits set (0x548); door masks 0xBDC.. =
0x3DFFF,0x7FBFF,0x1FFFFF,0xFBF,0xF,0xFFFFF,0x3FFFF,0x17F,0x7FFFDF; race results 78 × 0xFF from 0xFF0
(decomp: 3 × 26 byte stores — save.h documents 35, see open item); best lap/total 78 × 0x7FFFFFFF;
0x7FFFFFFF sentinels at 0xA50/0xA54 +0x20k (k<10), 0xB78/0xB7C +0xC·k (k<8) with u16 0x00FF at 0xB82+0xC·k, and
0x12B0..0x12C4; 0x7F ×10 at 0x12D0; towns 0x3FF (0x12DA); radio 2 (0x12DD); 0x5A at 0x12DE; remap
0x40,0x80,0x20,2,4,1,8,0x10,8,0x100 (0x12E8). Unmodelled bytes (e.g. currency-name default copied from ELF
data at 0x3335F8, many 0xC1C..0xD48 flag words beyond those listed) are zero except the u32 `1`/mask words
listed in `save_v2.cpp` `kWordStores`.
