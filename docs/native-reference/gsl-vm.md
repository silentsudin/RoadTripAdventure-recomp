# GSL container + native “VM” scope

Updated 2026-10-17 (wave1 B4). Ground truth: read-only disc
`~/Library/Application Support/RoadTripRecomp/disc` (49× `.GSL`), ELF
`SLUS_203.98` (vaddr → file = vaddr − `0x1FF000`),
`src/native/assets/hud_art.cpp`, prior
[`sys-gsl-packs.md`](sys-gsl-packs.md), [`hud-layout.md`](hud-layout.md),
[`race-hud-atlas.md`](race-hud-atlas.md).

**Headline:** a `.GSL` file is **not** a TOC + script bytecode container. It is a
serialized **PS2 DMA/GIF texture-upload chain**. There are no script units inside
GSL. HUD “scripting” lives in the ELF as C builders + ROM template tables that
*consume* textures previously uploaded from GSL. A native VM that reproduces a
SCORE HUD screen is therefore **(1) a GSL chain walker/decoder** + **(2) a thin
arena draw layer** driven by the already-proven race-HUD record grammar — not a
bytecode interpreter of bytes inside the `.GSL`.

---

## 1. Disc inventory (PROVEN)

49 files, all walk to an END tag. First 16 bytes are the **first DMA CNT tag**,
not a magic/version header.

| path | size | first 16 (hex) | units |
|---|---:|---|---:|
| `ITEM/METER0.GSL` | 42480 | `060812100000000010358800ecf91200` | 4 |
| `ITEM/METER1.GSL` | 39696 | `660912100000000008618800ecf91200` | 2 |
| `ITEM/METER10.GSL` | 40656 | `06081210000000000c358800ecf91200` | 6 |
| `ITEM/METER2.GSL` | 35312 | `060812100000000010358800ecf91200` | 4 |
| `ITEM/METER3.GSL` | 34288 | `06048710000000006cff120001000000` | 4 |
| `ITEM/METER4.GSL` | 33104 | `06048710000000006cff120001000000` | 8 |
| `ITEM/METER5.GSL` | 36320 | `060412100000000010b58700ecf91200` | 6 |
| `ITEM/METER6.GSL` | 45040 | `060812100000000014358800ecf91200` | 4 |
| `ITEM/METER7.GSL` | 33904 | `060412100000000090ad8700ecf91200` | 4 |
| `ITEM/METER8.GSL` | 41680 | `0608121000000000e0b08700ecf91200` | 6 |
| `ITEM/METER9.GSL` | 33904 | `060412100000000098ad8700ecf91200` | 4 |
| `ITEM/PARTS.GSL` | 276240 | `8600121000000000d4448700ecf91200` | 122 |
| `SYS/EFFECT.GSL` | 34224 | `26001210000000006c358700ecf91200` | 72 |
| `SYS/FONT.GSL` | 99088 | `06188710000000006cff120001000000` | 2 |
| `SYS/FONTE.GSL` | 99088 | `06188710000000006cff120001000000` | 2 |
| `SYS/LOGO.GSL` | 148752 | `0624121000000000a8b48b00ecf91200` | 2 |
| `SYS/NOTE.GSL` | 239968 | `060912100000000034558800ecf91200` | 6 |
| `SYS/OPTION2.GSL` | 34064 | `0608121000000000b4b08700ecf91200` | 2 |
| `SYS/POSE.GSL` | 101808 | `06088710000000006cff120001000000` | 8 |
| `SYS/PUTI.GSL` | 244592 | `060912100000000070268700ecf91200` | 204 |
| `SYS/S00.GSL` | 144368 | `2600121000000000707e900074f61200` | 172 |
| `SYS/S01.GSL` | 78576 | `2600121000000000a089ab0074f61200` | 124 |
| `SYS/S02.GSL` | 320160 | `670812100000000074418800ecf91200` | 18 |
| `SYS/SCORE.GSL` | 72400 | `06088710000000006cff120001000000` | 6 |
| `SYS/SCOREA00..20.GSL` (20 files; **no SCOREA16**) | 67056–639120 | mostly `06088710…` | 4–26 |
| `SYS/SELECT.GSL` | 129776 | `c60612100000000064058800ecf91200` | 6 |
| `SYS/SORA.GSL` | 116336 | `060c121000000000482a8700ecf91200` | 4 |
| `SYS/STAMP.GSL` | 278160 | `060912100000000060248700ecf91200` | 202 |
| `SYS/TITLE.GSL` | 386064 | `0646121000000000acf48f00ecf91200` | 4 |
| `SYS/WORLDMAP.GSL` | 143760 | `0623121000000000888d8b00ecf91200` | 2 |

**Corpus total: 1142 upload units / 49 files** (re-walked this pass; matches
`sys-gsl-packs.md`).

Siblings that share *related* packet grammar but are **not** `.GSL`:
`SYS/PUTI.BIN` (chunked T8 uploads, first16 starts `860b1210…` CNT), shop/car
packs (see `sys-gsl-packs.md` §4). `*.E3D` / `COIN.BIN` / `OPTION.BIN` /
`HG2.ICO` are different containers.

---

## 2. Container grammar (PROVEN)

No magic, no version dword, no unit-offset table. The file *is* the DMA chain.

### 2.1 Chain walk (exactly `SendTexturePacketsDMA` / `loadStep`)

Decompiled walk (`build/decomp/fn/0020e030.c`, also inlined in `loadStep`
`0x0020d120`):

```
cur = buffer_start   // runtime: 0x00100000 after CD read
loop:
  b = *(u8*)(cur + 3)
  b == 0x70 ('p')  -> END; stop (do not consume trailer)
  b != 0x10        -> fatal("GSL DMA Tag Error!") @ vaddr 0x002EA490
  qwc              = *(u16le*)cur
  // optional: rebase BITBLTBUF.DBP at cur+0x20 (see §5)
  cur             += (qwc + 1) * 16
```

Then `sendTexturePackets(buf)` @ `0x00207B50`: `FlushCache`; DMAC ch2 (GIF)
`QWC=0`, `TADR=buf`, `CHCR=0x105` — hardware walks the same CNT chain.

### 2.2 Unit layout (offsets from unit start `T`)

| offset | size | content |
|---|---|---|
| `T+0` | 16 | DMA **CNT** tag: `u16 QWC`, byte3 = `0x10` (ID=1). Qword1 = PC-tool junk (ignored; never REF) |
| `T+16` | 16 | Tool junk (C++ object-header remnant). Part of GIF payload stream; field-masked |
| `T+32` | 64 | Four GIF A+D quads `{u64 value, u32 reg}`: regs **`0x50 BITBLTBUF`**, **`0x51 TRXPOS`**, **`0x52 TRXREG`**, **`0x53 TRXDIR`** |
| `T+96` | 16 | GIF **IMAGE** tag, `FLG=2`, `NLOOP = image_bytes/16` |
| `T+112` (`0x70` from unit start) | `NLOOP×16` | Raw texel/CLUT bytes |

**Invariant (1142/1142):** `qwc == 6 + NLOOP` ⇔ `qwc×16 == 96 + NLOOP×16`.

Bitfields (little-endian u64s, same as `hud_art.cpp`):

| field | source | bits |
|---|---|---|
| PSM | BITBLTBUF (`T+32`) | 56..61 |
| DBP | BITBLTBUF | 32..45 |
| DBW | BITBLTBUF | 48..53 |
| width | TRXREG (`T+48`) | 0..11 |
| height | TRXREG | 32..43 |
| NLOOP | IMAGE tag (`T+80`) | 0..14 |
| FLG | IMAGE tag | 58..59 (=2) |

### 2.3 Sampled offset tables

#### `SYS/SCORE.GSL` (72400 B, 6 units) — race HUD atlas

| u | file off | qwc | size | PSM | WxH | DBP | NLOOP | pixels @ |
|---|---:|---:|---:|---|---|---:|---:|---:|
| 0 | `0x00000` | 2054 | `0x08070` | T4 | 256×256 | `0x2c36` | 2048 | **`0x00070`** |
| 1 | `0x08070` | 38 | `0x00270` | CT16 | 16×16 | `0x2cb6` | 32 | `0x080e0` |
| 2 | `0x082e0` | 2054 | `0x08070` | T4 | 256×256 | `0x2cb8` | 2048 | `0x08350` |
| 3 | `0x10350` | 38 | `0x00270` | CT16 | 16×16 | `0x2d38` | 32 | `0x103c0` |
| 4 | `0x105c0` | 262 | `0x01070` | T8 | 64×64 | `0x2d3a` | 256 | `0x10630` |
| 5 | `0x11630` | 70 | `0x00470` | CT32 | 16×16 | `0x2d4a` | 64 | `0x116a0` |
| END | `0x11aa0` | 2 | — | — | — | — | — | — |

Matches `hud_art.cpp` / `race-hud-atlas.md`: **unit 0 image at file `0x70`**, PSMT4
256×256, CLUT unit 1 = 512 B CT16. END + 32 B tool trailer (not walked).

#### `ITEM/METER0.GSL` (42480 B, 4 units) — dial ring page

| u | file off | qwc | PSM | WxH | DBP | pixels @ |
|---|---:|---:|---|---|---:|---:|
| 0 | `0x00000` | 2054 | T8 | 256×128 | `0x0000` (rebased +`0x237a` at load) | `0x00070` |
| 1 | `0x08070` | 70 | CT32 | 16×16 | `0x0080` | `0x080e0` |
| 2 | `0x084e0` | 486 | T4 | 128×120 | `0x0084` | `0x08550` |
| 3 | `0x0a350` | 38 | CT16 | 16×16 | `0x00a4` | `0x0a3c0` |
| END | `0x0a5c0` | | | | | |

#### `ITEM/METER10.GSL` (40656 B, 6 units) — speed-digit page (`hud-layout.md` §1.2)

| u | file off | PSM | WxH | notes |
|---|---:|---|---|---|
| 0 | `0x00000` | T8 | 256×128 | dial + digit row v=102 |
| 1 | `0x08070` | CT32 | 16×16 | |
| 2 | `0x084e0` | T4 | 128×32 | tex entry 1 family |
| 3 | `0x08d50` | CT16 | 16×16 | |
| 4 | `0x08fc0` | T4 | 128×48 | tex entry 2 family |
| 5 | `0x09c30` | CT16 | 16×16 | |
| END | `0x09ea0` | | | |

#### Script-heavy *looking* files (still texture-only)

| file | units | shape |
|---|---:|---|
| `SYS/STAMP.GSL` | 202 | T8 192×192 head + 100× {T4 64×64 + CT32 16×2} |
| `SYS/PUTI.GSL` | 204 | T8 192×192 + T4 320×128 + 100× {T4 64×47 + CT32 16×2} |
| `SYS/S00.GSL` | 172 | many T4 32×32 / 64×64 + CT32 16×2 (scene icons) |
| `SYS/SELECT.GSL` | 6 | T4 256×216, 192×896, 200×150 + small CT32s |
| `SYS/FONTE.GSL` | 2 | T4 **512×384** @ `0x70` + CT16 @ `0x180e0` (live font) |
| `SYS/POSE.GSL` | 8 | T8 256×128 + T4 256×256 + 2× T4 256×128 + CLUTs |

**Layout holds across all 49 sampled files** (CNT `0x10`, END `0x70`,
`qwc==6+NLOOP`, image at `T+0x70`).

### 2.4 Post-END trailer (PROVEN presence, WEAK semantics)

Every file has **exactly 48 bytes from END tag start** remaining (16 B END + 32 B
after). The walker stops at byte3=`0x70` and never reads the trailer. Contents
look like another tool-serialized DMA/GIF stub (`01 80 87 00 … 3f 00…`); **not
script bytecode**. Role: OPEN (likely packer footer / host heap remnant).

---

## 3. Unit classes (PROVEN)

GSL units are **only** GS image uploads. Classification is by PSM + geometry,
not by a type flag in a TOC:

| class | how recognized | examples |
|---|---|---|
| **Indexed texture** | PSM `0x13` T8 or `0x14` T4; large W×H | SCORE 256×256 T4, METER 256×128 T8, FONTE 512×384 T4 |
| **CLUT / palette** | PSM `0x00` CT32 or `0x02` CT16; 16×2 / 16×16 | always immediately follows its texture in the chain |
| **Direct-colour texture** | PSM CT24 rare | 2× in `S00.GSL` only |
| **Font atlas** | same as indexed texture; geometry 512×384 T4 | `FONT`/`FONTE` — not a separate unit type |
| **Script unit** | — | **does not exist in .GSL** |

Pairing rule (corpus): texture unit → next unit is its CLUT (1:1 for all but the
two CT24 S00 uploads). `hud_art.cpp` assumptions validated:

- `kCnt=0x10`, `kEnd=0x70`, `kUnitHeaderBytes=112` (`T+0`..`T+111`)
- unit 0 pixels at **file offset `0x70`** when the file starts with a unit
- `qwc == 6 + nloop`, IMAGE FLG==2, regs 0x50..0x53

---

## 4. Texture-unit upload flow (PROVEN)

```
task sets tcb+0x1c = category|index
    │
    ▼
loadStep 0x0020D120
    state0: cdReadEntry* → buffer 0x00100000
    state1: walk CNT tags; optional DBP rebase on u64 at tag+0x20
            mask 0xffffc000ffffffff, bits32–45 += page_base
    │
    ▼
sceDmaSync(ch1); sendTexturePackets(0x00100000) @ 0x00207B50
    FlushCache
    GIF CHCR=0x105, TADR=buffer   // hardware consumes file bytes
```

| category (bits 16–19 of request) | files | DBP addend |
|---|---|---|
| `0x40000` | most SYS packs (SCORE*, STAMP, NOTE, FONTE via table, …) | none (absolute) |
| `0x10000` / `0x20000` | TITLE / LOGO | `+0x1d8e` |
| `0x60000` | PUTI.BIN chunks | `+0x237a` |
| `0xA0000` | `ITEM/METER0..10` | `+0x237a` |
| `0x80000` | `ITEM/PARTS` | `+0x2c7e` |

SCORE pages land at absolute DBP `0x2c36` / `0x2cb8` / `0x2d3a` — matches
`hudTexTable` `0x29F5F8` TEX0 in `hud-layout.md` §1.1.

Decode for native (already in `hud_art.cpp` / `sys-gsl-packs.md`):

- T4: low nibble first; T8: 1 byte/index
- CLUT CSM1 swizzle: phys = `(i&7)|((i&8)<<1)` (T8) or T4 variant in hud_art
- CT16 RGBA5551; entry 0 transparent; TEXA-style nonzero α
- Race banner/timer colours need **CSA 1** (CLUT base entry 56 empirical) —
  `race-hud-atlas.md` PROVEN against oracle pixels

---

## 5. What is *not* a GSL script VM

### 5.1 Town-shop event script — `InterpretScriptCommands` @ `0x0023AD00`

**This is the only jump-table “opcode VM” found under script/interp symbols.**
It interprets **town/shop event control bytes**, not GSL, not draw ops.

| item | value | confidence |
|---|---|---|
| Entry | `0x0023AD00` .. `0x0023B167` | PROVEN (symbols + decomp) |
| Caller | dialogue/shop task `0x0023B168` | PROVEN (callgraph) |
| Dispatch | `lbu` opcode; `op-1`; `sltiu op, 0x1d`; `sll×4`; `lw` from table **`0x002EC750`**; `jr $v1` | PROVEN (ELF bytes) |
| Exit | opcode `≥ 0x20` returns current pointer (payload/text follows) | PROVEN (decomp) |
| Unknown ops | `printf("Unknown pre-control code %d in town T%02d shop %d\n")` then skip | PROVEN |

#### Opcode draft (town-shop event script)

Operand widths from decomp of each case; roles from callees. Confidence
**PROVEN** where decomp + table target verified; **STRONG** where role is the
named callee’s documented purpose.

| op | operands (after opcode byte) | apparent role | conf | case vaddr |
|---|---|---|---|---|
| `0x01` | `u8 flag, u8 branchIdx` | if `TestFlagBitFc98(flag)` → jump table\[branchIdx]; else skip 2 | PROVEN | `0x0023AD50` |
| `0x02` | `u8 flag, u8 branchIdx` | if flag **clear**, branch; else cont+2 | PROVEN | `0x0023AD70` |
| `0x03` | `u8 a, u8 b` | `GetFlaggedSlotCount(a,b,0)` → conditional branch | PROVEN | `0x0023AD90` |
| `0x04` | `u8 branchIdx` | if `TestStampFlag(shop+0x23,+0x21)` branch | PROVEN | `0x0023ADA8` |
| `0x05` | `u8 idx, u8 branchIdx` | if `TestFlagFc68(idx)` branch | PROVEN | `0x0023ADC8` |
| `0x06` | `u8 bit, u8 branchIdx` | if `TestStateBitFc78(bit)` branch | PROVEN | `0x0023ADE8` |
| `0x07` | `u8 branchIdx` | if `CheckSlotPresence()` branch (“enough teammates?” per game_state) | STRONG | `0x0023AE08` |
| `0x08` | `u8 branchIdx` (+ optional skip) | `MatchSlotPairState` / `TestFlagFcb8` pair gate | PROVEN | `0x0023AE20` |
| `0x09`,`0x0A`,`0x0C` | — | fall through to unknown-op printf path | PROVEN | `0x0023B0E0` |
| `0x0B` | `u8 result, u8 branchIdx` | if `GetOrSetPresidentRaceResult(-1)==result` branch (0 retired / 1 lost / 2 won) | PROVEN | `0x0023AEC0` |
| `0x0D` | `u8 flag, u8 branchIdx` | if `TestFlagFca8Clear(flag)` branch | PROVEN | `0x0023AEE0` |
| `0x0E` | 6-byte span | if flag 0x50 set, reload script ptr from task table; else skip 6 | PROVEN | `0x0023AF00` |
| `0x0F` | `u8 flag` | `SetFlagBitFc98` | PROVEN | `0x0023AFF8` |
| `0x10` | `u8 flag` | `ClearFlagBitFc98` | PROVEN | `0x0023B010` |
| `0x11` | `u8 a, u8 b` | `ClearFlaggedSlotBit(a,b,0)` | PROVEN | `0x0023B028` |
| `0x12` | none | set task result fields (done code 10) | PROVEN | `0x0023B048` |
| `0x13` | `u8 code` | `RecordTaskResult(code)` | PROVEN | `0x0023B060` |
| `0x14` | `u8 a, u8 b` | close stamp + write shop ids; result code 12 | PROVEN | `0x0023B078` |
| `0x15` | `u8 bit` | `SetStateBitFc88` | PROVEN | `0x0023B0B0` |
| `0x16` | `u8 branchIdx` | if `TestProgressFlagForPair` branch | PROVEN | `0x0023AE58` |
| `0x17` | `u8 flag` | `SetProgressFlagAndQueue` | PROVEN | `0x0023B0C8` |
| `0x18` | `u8 branchIdx` | if money-ish word in `[54000, 54000+0x23281)` cont else branch | STRONG | `0x0023AE78` |
| `0x19` | `u8 flag, u8 branchIdx` | if `TestProgressFlag(flag)` branch | PROVEN | `0x0023AEA8` |
| `0x1A` | `u8 n, u8 branchIdx` | if `n ≤ 100 - CountBitsFca8Fcb0(0)` branch | PROVEN | `0x0023AF38` |
| `0x1B` | `u8 branchIdx` | president/state gate (`DAT_0177FDB1==3`, bit 100 / flag 0x57) | STRONG | `0x0023AF60` |
| `0x1C` | `u8 id, u8 branchIdx` | if shop id `+0x23 == id` branch | PROVEN | `0x0023AFA0` |
| `0x1D` | `u8 a, u8 b, u8 branchIdx` | if `TestObjectAttribute(0x177F760,a,b)` branch | PROVEN | `0x0023AFC0` |

Branch target: `param_3 = *(byte**)(branchIdx * 4 + *(int*)(task+0x20))` —
a **pointer table owned by the shop task**, not offsets into a GSL.

**Not used for HUD drawing from SCORE.GSL.**

### 5.2 Race-HUD element “draw ops” — closest thing to a HUD VM

`raceHudDraw` @ `0x00233860` rebuilds an arena from element builders; textures
come from `hudTexTable` `0x29F5F8` (SCORE pages already in VRAM).

ElementA @ `0x0022EE70` — 44 B records, type field word0 (hud-layout §1.1):

| type | role | conf | evidence |
|---|---|---|---|
| 1 | centred sprite batch from 32 B records `{x,y,u,v,w,h,z,flags}` | PROVEN | decomp: bbox half-extents then `uiArenaSprites` |
| 2 | rotated quad (needle); angle from `value`, scale word10 | PROVEN | decomp + hud-layout |
| 3 | proportional fill `value/max` | PROVEN | hud-layout (ElementA idx countdown bars) |

These are **in-memory structs filled by C updaters**, not bytecode streamed from
GSL. ROM templates at `0x2EBD00..0x2EC040` supply default UV rects
(`hud-layout.md`).

### 5.3 Template `0x3380B` (minimap GS state) — not a script opcode

| fact | evidence | conf |
|---|---|---|
| Data word `TEST_2 = 0x0003380B` in `mapStatePacket` @ `0x002A43B0` | ELF bytes at file `0xA53B0`; symbols.tsv | PROVEN |
| Sent by `vifDirect(0x2a43b0, 3)` at start of every map draw | `gs_map_draw_functions_{town,race,scissored}` decomps | PROVEN |
| Callers | town: `0x242438`; race: `0x2427D0` (from `raceHudTask`); scissored: `0x242A88` | PROVEN |
| Code hit at `0x260CEC` | false positive: MIPS `movn $a3,$zero,$v1` encodes as word `0x0003380B` inside `roulettePlaceBallOnWheel` | PROVEN (disasm) |

Map path then uploads matrices and `drawModelParts` (PATH1 geometry) — **no
GSL script walk**. Bezel sprites still come from METER/UI atlases via
`townMapBezel` / uiArena*.

---

## 6. Minimum-viable native VM (SCORE HUD screen)

Scope to pixel-match the driving race-HUD strip documented in
`race-hud-atlas.md` + timer/rank from `hud-layout.md` §1.1.

### 6.1 Must implement

1. **GSL walker** — §2.1/§2.2; reject non-`0x10`/`0x70`; assert `qwc==6+NLOOP`.
2. **Unit decoder** — T4 + CT16 (and T8 + CT32 for METER later); CSM1; CSA base
   entry select (0 vs 56 for SCORE banks).
3. **SCORE load** — take units 0+1 (page0 atlas + CLUT); optionally 2+3 (page1).
4. **Draw list (static MVP)** — hardcode the oracle rects from `race-hud-atlas.md`:
   - T-circle CSA1 `u,v,w,h = 0,110,36,34` → dest UI …
   - OTALTIME CSA1 `30,80,130,18`
   - Timer digits 16×16 @ v=64, u=d×16, CSA1; separators `'`/`"` @ v=48
   - Rank digits / oval CSA0 per atlas doc
5. **Coordinate space** — author 640×448 (NTSC); present 640×480 with +16 Y
   centring (`hud-layout.md` / native-2d).

### 6.2 Deferred (not required for SCORE strip MVP)

- Full `raceHudDraw` element graph / 0x8CB0 object
- ElementA types 2–3 animation
- Minimap PATH1 + `0x3380B` packet
- Town-shop `InterpretScriptCommands`
- DBP rebase categories other than absolute SCORE
- Live digit values (can stub `00'00"00`)

### 6.3 Suggested native module split

| module | responsibility |
|---|---|
| `gsl_pack` (or extend pack_catalog) | walk + unit descriptors only (no bytes in git) |
| `hud_art` (exists) | SCORE/METER decode → RGBA |
| `race_hud_mvp` | blit atlas rects with CSA; no PS2 DMA |

---

## 7. Open questions + next probes

| # | question | what was tried | next probe |
|---|---|---|---|
| 1 | Exact GIF visibility of junk at `T+16` and IMAGE hi-qword | corpus parse only | hardware/emu TTE trace of one unit |
| 2 | Post-END 32 B footer meaning | present on 49/49; not walked | compare footer across SCORE twins; search ELF for trailer pattern |
| 3 | Which task issues SCORE load request `0x40000|idx` | asset-map / loadStep categories known | xref `sysFileTable` `0x29AF58` SCORE entries from spawners |
| 4 | Full ElementA type-3 fill math + type-2 angle fold constants | hud-layout text; partial decomp | finish `0x22EE70` decomp + `DAT_00332E34` |
| 5 | Shop script pointer-table layout at `task+0x20` | branch uses `branchIdx*4` | dump one live shop script from disc (non-GSL) if present |
| 6 | Any non-texture bytes *inside* a unit payload | NLOOP spans match w×h×bpp for all but 4 T4 round-ups | none expected |
| 7 | “Draw-template script” separate from ElementA | grepped symbols for template/opcode/GSL; only mapStatePacket + town script | search ROM for other `jr` tables near uiArena call sites if new HUD modes appear |

---

## 8. Evidence index

| claim | source |
|---|---|
| 49 GSL, 1142 units, chain grammar | this pass python walk of disc; `sys-gsl-packs.md` |
| Unit 0 @ `0x70` SCORE T4 256×256 | disc bytes; `hud_art.cpp`; `race-hud-atlas.md` |
| Loader walk + fatal string | `0020e030.c`, `0020d120.c`, string vaddr `0x2EA490` |
| `sendTexturePackets` GIF kick | `00207b50` disasm: ch2, CHCR `0x105` |
| Town script JT `0x2EC750`, ops 1–0x1d | ELF table words + `0023ad00.c` |
| `0x3380B` map packet only | ELF data `0x2A43B0`; decomp map drawers; false+ at `0x260CEC` |
| Race HUD draw graph | `00233860.c`, `0022ee70.c`, callgraph |
| CSA 0/1 colour banks | `race-hud-atlas.md` pixel match |
