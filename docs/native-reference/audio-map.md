# Audio context map — Road Trip (SLUS-20398)

The authentic audio context map for the native recomp: which `SOUND/` asset each
game situation plays, and how the original selects it. This is the reference the
native BGM/SE selection should match.

Everything below is either **PROVEN** (read directly from the ELF/instrument or
from a disc byte range whose LBN resolves to a named file), **INFERRED** (a
mechanism is PROVEN but the human-facing label — which mode, which station name —
is a reading of caller/consumer semantics), or **OPEN** (not settled).

Derivation method, in one line: `gs_music_play_bgm(id)` forwards the packed id to
the IOP driver together with the file record at `gs_music_file_table[(id>>16)&0xff]`;
the record's start-LBA resolves through the disc LBN map to a `SOUND/` filename.
The id constants at every call site were recovered by scanning the ELF for
`jal 0x00258EC8` and reading the `lui a0,0xaXX` / delay-slot `ori a0,a0,0xPPP`
pair that materialises `a0`.

Related docs: `asset-map.md` (Table A), `tsq-tvb-format.md` (TSQ/TVB containers),
`sndmod-irx.md` (TSQ driver + command grammar), `functions/sound-rpc.md` (RPC
wrappers), `../game-mechanics/sound-and-daynight.md` and
`../game-mechanics/menus-and-save.md` (contexts).

---

## 1. `SOUND/` inventory

30 files, 149,868,892 bytes total.

| class | files | count | bytes | notes |
|---|---|---:|---:|---|
| Streaming VAG (L/R pairs) | `1CH_L/R.VAG`, `2CH_L/R.VAG`, `3CH_L/R.VAG` | 6 | 148,644,320 | `VAGp` v0x20, **12 000 Hz**, headerless-block ADPCM. Each side ≈ 24.7 MB ≈ **60 min**. Internal VAG names: 1CH = `us_cq_l`/`us_cq_r`, 2CH = `2ch_l`/`2ch_r`, 3CH = `3ch_l`/`3ch_r`. **PROVEN.** These are the radio streams (see §5). |
| TVB tone banks | `BGM.TVB`, `ACTION.TVB`, `CQ_MAIN.TVB` | 3 | 839,248 | Self-contained 256-slot sample pools (offset table + headerless PS1 4-bit ADPCM). **PROVEN** (`tsq-tvb-format.md`). |
| TSQ sequences | `BGM_01..BGM_12`, `ACTION`, `CQ_MAIN`, `ROOM_1`, `REPLAY`, `SELECT`, `ENDING`, `DEMO`, `FANFARE` | 20 | 274,752 | Segment pointer table + track event streams. BGM-family = 50 tracks; ACTION/CQ_MAIN = 99. **PROVEN.** |
| IOP driver | `SNDMOD.IRX` | 1 | 110,572 | TSQ/voice driver, radio + synthesized engine. **PROVEN.** |

Whole-file sizes: VAG 148,644,320 · TVB 839,248 · TSQ 274,752 · IRX 110,572.

There are **no ASCII asset names inside the ELF or the IOP modules** — names exist
only on the disc directory and in the LBN map, so a file is identified by
resolving its start-LBA, never by an in-binary string.

---

## 2. How the original selects BGM (mechanism — PROVEN)

`gs_music_play_bgm @ 0x258EC8` (decompiled):

```c
void gs_music_play_bgm(uint32_t id) {
    FUN_00257fd8(0);                                   // RPC cmd 0xF  (arg 0)
    FUN_00257f30(0, id);                               // RPC cmd 0xE  (slot 0, id)
    FUN_00257a28((id >> 8) & 0xff,                     // RPC cmd 7   (param PP, …)
                 &gs_music_file_table + ((id >> 16) & 0xff) * 3);
}
```

- `FUN_00257fd8` / `FUN_00257f30` / `FUN_00257a28` are the cmd-0xF / cmd-0xE /
  cmd-7 RPC wrappers documented in `functions/sound-rpc.md`.
- **Packed id layout — PROVEN.** Every observed id is `0x0A II PP 01`:
  - byte 3 (`0x01`) — constant.
  - byte 2 (`PP`) — forwarded verbatim to the driver as cmd-7 param. Driver-side
    meaning **OPEN** (a song/sub-bank selector; see §8).
  - byte 1 (`II`) — **file index** into `gs_music_file_table`.
  - byte 0 (`0x0A`) — constant.
- `gs_music_file_table @ 0x29ADF0` — **PROVEN.** 29 records × 12 bytes
  (`0x29ADF0..0x29AF4C`), each `{u32 field0 = 0x00333388, u32 startLBN, u32 sizeSectors}`.
  `field0` is a constant pointer into a caption-string pool; `FUN_00257a28`
  `strcpy`s it as the record "name" but the load path uses the **LBN**, so
  `field0`'s driver role is **OPEN** (likely vestigial on this disc).

### Boot-time bank registration — PROVEN

`InitSoundRpcDriver @ 0x256AF8` also pre-registers the banks (cmd 6 = TVB, cmd 7 =
TSQ), before any gameplay:

| slot | TVB (cmd 6) | TSQ (cmd 7) |
|---:|---|---|
| 0 | `CQ_MAIN.TVB` | `CQ_MAIN.TSQ` |
| 1 | `BGM.TVB` | `ROOM_1.TSQ` |
| 2 | `BGM.TVB` | — |
| 3 | `ACTION.TVB` | `ACTION.TSQ` |

and registers three **stereo VAG pairs** via cmd 0x32 (`FUN_002575e0`, params
`slot*2` and `slot*2+1`):

| slot | L | R |
|---:|---|---|
| 0 | `1CH_L.VAG` | `1CH_R.VAG` |
| 1 | `3CH_L.VAG` | `3CH_R.VAG` |
| 2 | `2CH_L.VAG` | `2CH_R.VAG` |

(Code order in the ELF is slot 0, then 2, then 1.)

---

## 3. `gs_music_file_table` — index → disc file (PROVEN)

`startLBN` and `sizeSectors` read from the table at `0x29ADF0`; filenames resolved
through the disc LBN map (`disc/.lbn_map.tsv`), matching `asset-map.md` Table A.

| idx | file | startLBN | sectors | bytes |
|---:|---|---:|---:|---:|
| 0 | `CQ_MAIN.TVB` | 192028 | 147 | 299392 |
| 1 | `CQ_MAIN.TSQ` | 192026 | 2 | 2823 |
| 2 | `ACTION.TVB` | 240747 | 118 | 239824 |
| 3 | `ACTION.TSQ` | 240865 | 1 | 905 |
| 4 | `1CH_L.VAG` | 228654 | 12064 | 24706368 |
| 5 | `1CH_R.VAG` | 192175 | 12064 | 24706368 |
| 6 | `2CH_L.VAG` | 240904 | 12073 | 24724272 |
| 7 | `2CH_R.VAG` | 179788 | 12073 | 24724272 |
| 8 | `3CH_L.VAG` | 204239 | 12155 | 24891520 |
| 9 | `3CH_R.VAG` | 216475 | 12155 | 24891520 |
| 10 | `BGM.TVB` | 191861 | 147 | 300032 |
| 11 | `ROOM_1.TSQ` | 240718 | 16 | 31915 |
| 12 | `BGM_01.TSQ` | 216448 | 14 | 27157 |
| 13 | `BGM_02.TSQ` | 216462 | 4 | 7180 |
| 14 | `BGM_03.TSQ` | 192008 | 8 | 14591 |
| 15 | `BGM_04.TSQ` | 192016 | 10 | 18466 |
| 16 | `BGM_05.TSQ` | 228630 | 10 | 19840 |
| 17 | `BGM_06.TSQ` | 228640 | 14 | 27460 |
| 18 | `BGM_07.TSQ` | 240734 | 8 | 15218 |
| 19 | `BGM_08.TSQ` | 240742 | 5 | 8312 |
| 20 | `BGM_09.TSQ` | 240866 | 11 | 20947 |
| 21 | `BGM_10.TSQ` | 240877 | 5 | 9023 |
| 22 | `BGM_11.TSQ` | 240882 | 4 | 7586 |
| 23 | `BGM_12.TSQ` | 216466 | 9 | 16853 |
| 24 | `REPLAY.TSQ` | 240900 | 4 | 7422 |
| 25 | `SELECT.TSQ` | 240886 | 2 | 2233 |
| 26 | `ENDING.TSQ` | 240888 | 12 | 24346 |
| 27 | `DEMO.TSQ` | 252977 | 5 | 10081 |
| 28 | `FANFARE.TSQ` | 252982 | 2 | 2394 |

The native resolver in `src/native/audio/music_library.h` keys on exactly this
`(id>>16)&0xff` file index, so the mapping above is the contract it must satisfy.

---

## 4. Context → sequence

### 4a. Static `gs_music_play_bgm` call sites (35 — PROVEN id, INFERRED label)

Every `jal 0x00258EC8` in the ELF, with the id materialised into `a0` and the
containing function. "Context" is the human reading of the caller.

| # | call site | container function | id | idx | file | inferred context |
|---:|---|---|---:|---:|---|---|
| 1 | `0x0020A1A8` | `UpdateMenuOptionTask` (`0x20A150`) | `0x0A180201` | 24 | `REPLAY.TSQ` | replay of an Adventure action from the options/menu flow |
| 2 | `0x0020F7AC` | `UpdateRaceGameTask` (`0x20F3B8`) | `0x0A180201` | 24 | `REPLAY.TSQ` | post-race replay |
| 3 | `0x0022B19C` | `FUN_0022AEB0` (shop/area entry) | `0x0A0B0101` | 11 | `ROOM_1.TSQ` | room theme, sub-song 1 |
| 4 | `0x0022B1C8` | `FUN_0022AEB0` | `0x0A0B0103` | 11 | `ROOM_1.TSQ` | room theme, sub-song 3 |
| 5 | `0x0022B294` | `FUN_0022AEB0` | `0x0A0B0104` | 11 | `ROOM_1.TSQ` | room theme, sub-song 4 |
| 6 | `0x0022B2A8` | `FUN_0022AEB0` | `0x0A0B0102` | 11 | `ROOM_1.TSQ` | room theme, sub-song 2 |
| 7 | `0x0022CE38` | `RunCourseSelectHubTask` (`0x22CD38`) | `0x0A0B0102` | 11 | `ROOM_1.TSQ` | course-select hub room |
| 8 | `0x0022DEA4` | `FUN_0022DDD0` (race BGM chooser) | *dynamic* | — | race table | per-course race music (§4b) |
| 9 | `0x0022DEC4` | `FUN_0022DDD0` | `0x0A170201` **or** `0x0A160201` | 23 / 22 | `BGM_12.TSQ` / `BGM_11.TSQ` | special race modes (two-entry call site) |
| 10 | `0x0022DED8` | `FUN_0022DDD0` | `0x0A0D0201` | 13 | `BGM_02.TSQ` | default race fallback |
| 11 | `0x00245888` | `drawCarouselScene` (`0x2455C0`) | `0x0A190201` | 25 | `SELECT.TSQ` | Quick-Race car-selection carousel |
| 12 | `0x00251E24` | `UpdateGolfMiniGame` (`0x251B60`) | `0x0A0D0201` | 13 | `BGM_02.TSQ` | golf mini-game |
| 13 | `0x00252300` | `UpdateGolfMiniGame` | `0x0A0B0102` | 11 | `ROOM_1.TSQ` | golf mini-game room/hub return |
| 14 | `0x00254498` | `TuningGarageSceneTask` (`0x254430`) | `0x0A0D0201` | 13 | `BGM_02.TSQ` | garage / tuning scene |
| 15 | `0x00254DFC` | `CarShopVisitSceneTask` (`0x254D20`) | `0x0A100201` | 16 | `BGM_05.TSQ` | car-shop visit scene |
| 16 | `0x00259654` | `StreetSceneTask` (`0x2595F0`) | `0x0A0D0201` | 13 | `BGM_02.TSQ` | street / arrival scene |
| 17 | `0x0025A3A0` | stunt-card task (`0x25A120`) | `0x0A100201` | 16 | `BGM_05.TSQ` | stunt-card mini-game |
| 18 | `0x0025A7CC` | stunt-card task (`0x25A120`) | `0x0A100201` | 16 | `BGM_05.TSQ` | stunt-card mini-game (2nd entry) |
| 19 | `0x0025DC1C` | stamp-collection task (`0x25DBB8`) | `0x0A100201` | 16 | `BGM_05.TSQ` | stamp-collection mini-game |
| 20 | `0x0025E2E4` | balloon-pop task (`0x25E278`) | `0x0A100201` | 16 | `BGM_05.TSQ` | balloon-pop mini-game |
| 21 | `0x0025E688` | goal-plane task (`0x25E630`) | `0x0A100201` | 16 | `BGM_05.TSQ` | goal-plane mini-game |
| 22 | `0x0025EBD0` | tug-of-war task (`0x25EB70`) | `0x0A100201` | 16 | `BGM_05.TSQ` | tug-of-war mini-game |
| 23 | `0x0025F70C` | finish-zone task (`0x25F6A0`) | `0x0A100201` | 16 | `BGM_05.TSQ` | finish-zone mini-game |
| 24 | `0x0025FC48` | finish-zone variant (`0x25FBF8`) | `0x0A100201` | 16 | `BGM_05.TSQ` | finish-zone mini-game (variant) |
| 25 | `0x0025FF4C` | split-camera task (`0x25FF00`) | `0x0A100201` | 16 | `BGM_05.TSQ` | dual-camera mini-game |
| 26 | `0x00260690` | goal-area task (`0x260638`) | `0x0A100201` | 16 | `BGM_05.TSQ` | goal-area mini-game |
| 27 | `0x00260EAC` | slot-machine task (`0x260DC0`) | `0x0A100201` | 16 | `BGM_05.TSQ` | slot-machine mini-game |
| 28 | `0x002626D0` | stunt/sequence task (`0x262678`) | `0x0A100201` | 16 | `BGM_05.TSQ` | stunt/sequence mini-game |
| 29 | `0x00263200` | race-course task (`0x2631A0`) | `0x0A100201` | 16 | `BGM_05.TSQ` | race-course mini-game |
| 30 | `0x0026374C` | `SeqIntroFadeTask` (`0x263710`) | `0x0A100201` | 16 | `BGM_05.TSQ` | mini-game intro fade |
| 31 | `0x00263930` | chase/goal task (`0x2638C0`) | `0x0A100201` | 16 | `BGM_05.TSQ` | chase/goal mini-game |
| 32 | `0x00263EB0` | target/goal task (`0x263E68`) | `0x0A100201` | 16 | `BGM_05.TSQ` | target/goal mini-game |
| 33 | `0x00264310` | `SeqLongFadeTask` (`0x2642B8`) | `0x0A100201` | 16 | `BGM_05.TSQ` | mini-game long fade |
| 34 | `0x0026CC4C` | `StartAreaIntroDayCycle` (`0x26CB60`) | `0x0A1C0201` **or** `0x0A1A0201` | 28 / 26 | `FANFARE.TSQ` / `ENDING.TSQ` | area-intro (first run) / day-cycle restart |
| 35 | `0x0026D2E4` | `StartDayCycleStepTask` (`0x26D2C0`) | `0x0A1A0201` | 26 | `ENDING.TSQ` | area day-cycle step entry |

Two rows realise **two ids at one call site** because the id is materialised in a
preceding branch (`#9`, `#34`); a naive "nearest preceding `lui a0`" scan
mis-attributes these, so they are called out explicitly.

### 4b. Race music — `gs_music_race_bgm_table @ 0x29F5B0` (PROVEN)

`FUN_0022DDD0` (decompiled) chooses race BGM from two mode bytes:

```c
if (mode[+0x20] == 0x1a)                       bgm = 0x0A170201;  // BGM_12
else if (mode[+0x22] < 0x12)                   bgm = gs_music_race_bgm_table[mode[+0x22]];
else if (mode[+0x22] == 0x1c)                  bgm = 0x0A160201;  // BGM_11
else                                           bgm = 0x0A0D0201;  // BGM_02
```

`gs_music_race_bgm_table` is 18 × u32 (72 bytes, `0x29F5B0..0x29F5F8`):

| mode[+0x22] | id | idx | file |
|---:|---:|---:|---|
| 0, 1 | `0x0A0C0201` | 12 | `BGM_01.TSQ` |
| 2, 3 | `0x0A130201` | 19 | `BGM_08.TSQ` |
| 4, 5 | `0x0A100201` | 16 | `BGM_05.TSQ` |
| 6 | `0x0A120201` | 18 | `BGM_07.TSQ` |
| 7 | `0x0A140201` | 20 | `BGM_09.TSQ` |
| 8, 9, 10 | `0x0A110201` | 17 | `BGM_06.TSQ` |
| 11 | `0x0A120201` | 18 | `BGM_07.TSQ` |
| 12 | `0x0A140201` | 20 | `BGM_09.TSQ` |
| 13, 14 | `0x0A0F0201` | 15 | `BGM_04.TSQ` |
| 15, 16 | `0x0A150201` | 21 | `BGM_10.TSQ` |
| 17 | `0x0A0E0201` | 14 | `BGM_03.TSQ` |

So race music draws only from `BGM_01..BGM_10`.

### 4c. Grouped verdicts

| game situation | sequence file | evidence |
|---|---|---|
| Per-course Adventure/Arcade race music | `BGM_01..BGM_10` (race table, by mode byte) | §4b — **PROVEN** mechanism, per-mode labels INFERRED |
| Special race modes (mode `+0x20==0x1a`, `+0x22==0x1c`) | `BGM_12`, `BGM_11` | §4b — PROVEN |
| Hub / town / shop / room areas | `ROOM_1.TSQ` (sub-songs PP 1–4) | §4a #3–#7 — PROVEN id, area label INFERRED |
| Car-selection carousel | `SELECT.TSQ` | §4a #11 |
| Race / action replay | `REPLAY.TSQ` | §4a #1, #2 |
| Garage & tuning scene | `BGM_02.TSQ` | §4a #14 |
| Street / arrival scene | `BGM_02.TSQ` | §4a #16 |
| Golf mini-game | `BGM_02.TSQ` | §4a #12 |
| **Carnival/stunt mini-games** (stamps, balloons, slot, tug-of-war, split-cam, finish zones, stunt cards, race-course, fades) | `BGM_05.TSQ` | §4a #15, #17–#33 (18 sites) — the minigame theme |
| Day-cycle area intro (first entry) | `FANFARE.TSQ` then `ENDING.TSQ` on restart | §4a #34 (two ids) |
| Day-cycle step entry | `ENDING.TSQ` | §4a #35 |

**No static caller** (in the table, never played by `gs_music_play_bgm` — **OPEN**):
`DEMO.TSQ` (idx 27), `CQ_MAIN.TSQ` (idx 1), `ACTION.TSQ` (idx 3). `CQ_MAIN` and
`ACTION` are attached as **banks at boot** (slots 0 and 3, §2), so they are
probably driven by a slot/state path rather than `gs_music_play_bgm`; `DEMO` has
no observed path at all.

---

## 5. Radio station → stream verdict

**The radio is the streaming VAG pairs, not a TSQ sequence.**

PROVEN:
- Save field `radio_station` at `[progress]+0x12DD` is a u8 enum; it is mirrored at
  EE `0x01780A3D` (`DAT_01780a3d`), with the station *volume* in the next byte
  `0x01780A3E`.
- `ApplySoundVolumes` (`0x2590B0` → `FUN_002590b0`) sets master volume, then if the
  station > 0 sends **RPC cmd 0x33** (`SoundRpcCmd33PlaySe`, `FUN_00258a48`) with
  argument `station - 1`, followed by **RPC cmd 0x34** (`FUN_00258af0`); if the
  station is 0 it calls `FUN_00258b30` (stop). (`0x2590B0` is reached from
  `UpdateTownExplorationTask 0x2105B8`, i.e. the radio only runs in town.)
- Boot registers exactly three stereo VAG pairs on driver slots 0/1/2
  = `1CH` / `3CH` / `2CH` (§2).

INFERRED:
- The `station - 1` index selects stream slot 0/1, so:
  - station 1 = **PEACH FM** → slot 0 = `1CH_*.VAG` (VAG name `us_cq_*`),
  - station 2 = **E-RADIO** → slot 1 = `3CH_*.VAG` (VAG name `3ch_*`),
  - slot 2 = `2CH_*.VAG` is registered but **no station value reaches it** →
    unused/hidden on this disc.
- Station display names ("PEACH FM", "E-RADIO") are the game's strings; the
  file↔station binding above is the consistent reading of the boot slot order and
  the `station-1` index but is not itself printed by the code.

The native `src/game/GameOptions.cpp` already models `0 off / 1 PEACH FM /
2 E-RADIO` against `progress+0x12DD`, matching this.

---

## 6. SFX / engine / UI verdict

**SOUND-effect samples are not standalone `SOUND/` files.** They are resolved
driver-side (SNDMOD) from the loaded banks; the EE only sends a packed id.

PROVEN (send path):
- `PlayUiSound @ 0x258E50` = `FUN_00258308(id | 0x40400000)` → **RPC cmd 0xD**
  ("play"); a zeroed masked arg routes to **cmd 1** (stop-all). See
  `functions/sound-rpc.md`.
- Voice/SE indices are handed to the driver by index (e.g. cmd 0x33
  `SoundRpcCmd33PlaySe`), so the driver owns id→sample resolution: **wiring-in
  PROVEN, id→sample mapping OPEN.**

PROVEN (specific effect ids, from decompiled producers):
- Vehicle tyre/skid: `UpdateCarTireSound 0x217C68` plays `SOUND id = table[0x29CBA8] + surfaceBit`
  and stops with `id + 0x8000` (the `+0x8000` bit is the driver's loop/stop flag).
- Continuous horn/boost: `UpdateContinuousSoundEffect 0x217F10` plays `0x10/0x11`
  and `0x8010/0x8011`.
- Event: `PlayEventSoundStub 0x22C630` plays `0x8035`.
- Options cursor: `FUN_002137b0` (radio/SE options) plays `0x1a` (next) and `0x1b`
  (prev).

INFERRED (UI id inventory): a scan of the 256 `jal 0x00258E50` call sites recovers
these distinct ids: `0x01,0x02,0x03,0x1a,0x1b,0x1c,0x1d,0x1f,0x21,0x24,0x25,0x26,
0x2e,0x2f,0x34,0x35,0x65,0x136,0x1a4,0x302,0x303,0x307,0x30a,0x30f,0x314,0x31b,
0x31c,0x31e,0x328` plus the `0x80xx` vehicle ids. The `0x1xx/0x3xx` ids cluster in
menu/minigame code (plausibly per-menu confirm/cancel/point). Frequency alone
(`0x1a` ×37) does not establish meaning.

**Engine sound is synthesised by SNDMOD** (the driver's `sddr_Engine*` /
`vol_Engine` regenerates it), so no `SOUND/` file backs the engine note —
**PROVEN** that no engine sample file exists; the generator is an IOP code path,
not an asset.

`ACTION.TVB` is the bank most associated with gameplay SFX/instruments, and
`BGM.TVB`/`CQ_MAIN.TVB` back the music programs, but the exact bank each SE id
draws from is **OPEN**.

---

## 7. Ledger

| claim | status | basis |
|---|---|---|
| 30 `SOUND/` files, class counts/sizes (§1) | PROVEN | disc listing + VAG headers |
| VAG streams are `VAGp` v0x20, 12 kHz, names `us_cq/2ch/3ch` | PROVEN | VAG header |
| `gs_music_play_bgm(id)` → cmd 0xF/0xE/7 with table lookup | PROVEN | decompile `0x258EC8`, `0x257A28` |
| id = `0x0A II PP 01`, `II` = file index | PROVEN | call-site scan + decompile |
| `gs_music_file_table @ 0x29ADF0`, 29×12B | PROVEN | memory read + Table A |
| index→file mapping (§3) | PROVEN | LBN resolution |
| boot registers CQ_MAIN/BGM/ACTION banks + 3 VAG pairs | PROVEN | decompile `0x256AF8` |
| 35 `gs_music_play_bgm` call sites and their ids (§4a) | PROVEN id | ELF scan |
| per-context labels (race/golf/minigame/garage…) | INFERRED | caller identity/description |
| race BGM table `0x29F5B0`, 18 ids (§4b) | PROVEN | memory read + decompile `0x22DDD0` |
| mini-games all use `BGM_05`; menus/garage/street use `BGM_02` | INFERRED | call-site grouping |
| radio = streaming VAG pairs, station-1 index, slot 0/1 = 1CH/3CH | INFERRED | decompile `0x2590B0`/`0x2137B0` + §2 |
| station names PEACH FM / E-RADIO | PROVEN (enum) | `save-layout.md`, `GameOptions.cpp` |
| SE send path = cmd 0xD, driver-side sample resolution | PROVEN (path) | decompile `0x258E50`/`0x258308` |
| specific SE ids (tyre `0x29CBA8+bit`, `0x10/0x11`, `0x8035`, `0x1a/0x1b`) | PROVEN | decompiles |
| full SE id→sample/bank map | OPEN | driver-internal |
| record `field0` (`0x00333388`) role | OPEN | constant; load uses LBN |
| `PP` (id byte 2) semantics | OPEN | differs per call site; driver-side |
| `DEMO/CQ_MAIN/ACTION.TSQ` play path | OPEN | no `gs_music_play_bgm` caller |
| engine sound source | PROVEN (no file) | SNDMOD synth |

---

## 8. What remains OPEN, and what would close it

1. **`PP` (id byte 2) semantics.** It takes 0x01–0x04 (`ROOM_1` varies 1–4; most
   others are 0x02) and is passed to cmd 7, so it looks like a sub-song/bank
   selector in the driver. *Close with:* the `cmd 7` handler in SNDMOD (symbol
   table is available per `sndmod-irx.md`), or an SNDMOD debug trace of the
   `tsqtbl` index.
2. **SE id → sample/bank map.** *Close with:* the SNDMOD `reqse`/`tsqtbl` handler
   plus a bank-by-bank sample dump, or an SPU-voice trace while triggering each id.
3. **`DEMO.TSQ` / `CQ_MAIN.TSQ` / `ACTION.TSQ` play path.** *Close with:* the
   cmd-0xE handler in SNDMOD (what a slot 0/3 play does without a `gs_music_play_bgm`
   id), or a boot-to-demo trace.
4. **Record `field0` role.** *Close with:* reading the cmd-7 payload consumer in
   SNDMOD; static reading says it is unused for loading.
5. **Radio station ↔ VAG slot binding for an absolute certainty** (currently
   INFERRED from slot order + `station-1`). *Close with:* an SNDMOD trace showing
   which registered VAG slot the cmd-0x33 index plays, or the disc's audio
   content (Peach FM vs E-Radio jingles) matched to `us_cq` vs `3ch`.

---

## 9. Evidence index

- ELF `SLUS_203.98` (`VA = fileOffset + 0x1FF000`):
  - `gs_music_play_bgm @ 0x258EC8`; `FUN_00257A28 @ 0x257A28` (cmd 7);
    `FUN_00257F30 @ 0x257F30` (cmd 0xE); `FUN_00257FD8 @ 0x257FD8` (cmd 0xF).
  - `gs_music_file_table @ 0x29ADF0` (29×12B); `gs_music_race_bgm_table @ 0x29F5B0` (18×u32).
  - `InitSoundRpcDriver @ 0x256AF8`; `FUN_002575E0 @ 0x2575E0` (cmd 0x32 VAG register).
  - `FUN_0022DDD0 @ 0x22DDD0` (race BGM chooser).
  - `PlayUiSound @ 0x258E50` → `FUN_00258308 @ 0x258308` (cmd 0xD); `FUN_00258A48 @ 0x258A48` (cmd 0x33).
  - `ApplySoundVolumes @ 0x2590B0`; options/radio handler `FUN_002137B0 @ 0x2137B0`.
  - `UpdateCarTireSound @ 0x217C68`, `UpdateContinuousSoundEffect @ 0x217F10`,
    `PlayEventSoundStub @ 0x22C630`.
- Disc: `~/Library/Application Support/RoadTripRecomp/disc/SOUND/*` and
  `disc/.lbn_map.tsv`.
- Cross-references: `asset-map.md` (Table A), `tsq-tvb-format.md`, `sndmod-irx.md`,
  `functions/sound-rpc.md`, `../game-mechanics/sound-and-daynight.md`,
  `../game-mechanics/menus-and-save.md`, `save-layout.md`.
- Reproduce the call-site scan: search the ELF for `jal 0x00258EC8`
  (bytes `b2 63 09 0c`) and read the preceding `lui a0,0xaXX` / delay-slot
  `ori a0,a0,0x201` pair.
