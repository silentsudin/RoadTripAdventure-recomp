# Audio runtime (W2-09) — voice graph, SNDMOD sequence player, rumble opcodes

Module: `src/native/gameplay/audio_graph.{h,cpp}` (namespace `rt::native::gameplay::audio_graph`),
test `tests/native_w2/audio_graph_test.cpp` (`w2_audio_graph_test`). Pure host C++; no SDL, no guest
memory. ELF: `vaddr − 0x1FF000 = file offset`. `SNDMOD.IRX` LOAD segment: `file offset = vaddr + 0xA0`.
The brief referenced `docs/native-reference/audio-sequences.md`; **that file does not exist** in the
repo. The sequence grounding used instead is `audio-map.md`, `tsq-tvb-format.md`, `sndmod-irx.md`, plus a
fresh disassembly of `SNDMOD.IRX` (`play`, `step_t`, `key_on`, `volume`, `panpot`, `ext_chan`, …).

## 1. Voice graph (`AudioGraph`)

* 24 voices: 0..3 BGM (slot == voice), 4/5 radio L/R, 6..23 SE and sequencer notes (AUTHORED layout; same
  24-voice total as `engine.h`).
* **Per-voice pan is honoured everywhere** (`VoiceParams::pan`, `setPan`). This closes the `engine.h:110` /
  `mixer.h:56` gap (`playBgm` had no pan, so the right half of a stereo pair could not be routed):
  `playRadioPair(L, R)` binds L → voice 4 pan −1, R → voice 5 pan +1 (looping), atomically.
* Pan law: `panLinear` is bit-identical to `audio::PanLaw` (41-point sweep checked); `panConstantPower` is
  offered for hosts that want −3 dB centre; `balanceFromE1` models SNDMOD `E1 ll rr` (`/64`).
* Resampling: Q16.16 phase, linear interpolation, deterministic. `srcRate == outRate` is an exact copy.
  Pitch ratio = `freq / 4096` (SPU register 0x1000 = natural rate) via `pitchQ16`.
* Fades: gain at frame *i* of a ramp is `start + (target−start)·i/N`; frame *N* is exactly `target`.
  `crossfade(from,to,N)` = both ramps, `from` retired at frame *N* (sum of equal DC sources is constant ±1 LSB).
* Allocation (AUTHORED; the real SNDMOD policy is driver-internal): first free SE voice by index; when full,
  steal the lowest `(priority, serial, index)` victim if `victim.priority ≤ new priority`, else drop.
  Reproduced by an independent reference model in the test over 1995 random requests.

## 2. Sequence player (`SeqPlayer`) — what `SNDMOD.IRX` actually does

All rows re-read from the IRX this pass (addresses = IRX vaddr):

| Fact | Evidence |
|---|---|
| Per tick: `cnt = (s16)(cnt − 1)`; the stream is read only when `cnt == −1` | `play 0x5294`: `addiu −1` `0x52C4`, `sh` `0x52CC`, compare with −1 `0x52D8/0x52DC` |
| Delta byte `D` (`00..7F`) stores `D − 1` (`step_t 0x47F0`, `0x4810`) → next event after **exactly D ticks**; `D = 00` stores −1, the next tick makes it −2, so the track **stalls 65535 ticks** (s16 wrap). Not seen in any of the 622 tracks | `0x4810`, test vector "delta 0 resumes at 65536" |
| Note-on `80..DF` → `key_on(key & 0x7F)`: `freq = freqtbl[key]`; if `bend (+0xC) ≠ 0`: `freq = freq·bend >> 12` | `key_on 0x4BAC`: `lhu 0x8A9C[key]` `0x4BE4`, mult/`sra 12` `0x4C28..0x4C40` |
| `E0 vv` vol (1 op), `E1 ll rr` pan (2 ops), `E2 pp` program (1 op, `lb`, `\| chan<<8`), `E3 tempo` consumes 2 bytes and **stores nothing**, `E4/E5/EA` u16, `E6/E7` 1 op, `E8/E9` 0 op, `F0` key-off, `F1 a` priority, `F8` s16 jump after operands, `F9` fork | dispatch immediates `0x53D8..0x5620` (test checks the full set), handler bodies |
| **`EA` stores the u16 as the Q12 bend multiplier (+0xC); `E5` stores the u16 as the absolute freq (+0xE) and re-triggers the voice; `E4` stores freq (bend applied) without retrigger.** This corrects `sndmod-irx.md`/`tsq-tvb-format.md` ("E5 = bend multiplier") | `pitch_cent 0x48BC` (`sh +0xC`), `key_pitch 0x4A40` (`sh +0xE`, then `chgfreq/chgvol/chgtone` + `sb 2`), `pitch 0x5184` |
| Every opcode ≥ `0xE0` not in the list (`FF`, `EB..EF`, `F2..F7`, `FA..FE`) falls into `data_end 0x4940` and **stops the track**; none occurs in the real data | `play` default arm `0x5640` |
| `F9 aa bb` = `ext_chan`: copy the track to the lowest-priority slot among 8..0x2F whose priority ≤ caller's, new pc = pc(after operands) + s16, countdown 0 | `ext_chan 0x45D4`, loop bound `slti 0x30` `0x466C`, `sh $zero,0x14` `0x47D8`. ACTION.TSQ `0x194`: `F9 0B 00` forks the mirrored phrase at `0x1A4`, both stop at tick 100 (checked on disc data) |
| Volume: `volL = ((vol·panL)·master) / 256`, `volR` likewise, s16, truncating toward zero; global mono flag → both = `(L+R)/2` | `volume 0x4F74`: mult `0x4FD8/0x4FF4`, `+0xFF`/`sra 8` `0x5004/0x5008`; mono fold `0x509C..0x50B0`; `panpot 0x4DAC` identical |
| `freqtbl` (IRX vaddr `0x8A9C`, 72 × u16) = **floor**(4096·2^((k−48)/12)) — exact for 72/72 (round() fails 27 entries) | read from the IRX and compared in the test |

OPEN / AUTHORED: SPU gain scale (`vol/16383`, `kSeqMasterRef = 0x100`: `255·64·0x100>>8 = 16320 ≈ full
scale`; the real master at `+0x18` was not traced), default track state before the first `E0`
(`vol = 0x7F`), the order forks run relative to their parent inside one tick (same tick here), E4/E6/E7
effects on the SPU (stored/ignored), TVB sample→voice binding (host provider `SeqSampleProvider`).

### Track tables (found this pass)

* Flat files (`ACTION`, `CQ_MAIN`): `{u16 seg 0x90, u16 off}` table at `0x04` (25 / 50 distinct tracks).
* BGM family (`BGM_01..12`, `ROOM_1`, `REPLAY`, `SELECT`, `ENDING`, `DEMO`, `FANFARE`): entry 0 is `{0, T}`
  (`T = 0xD0`); all other first-layer entries point at `0xCC` (`F0 FF`, an empty track). The real tracks are
  `{u16 0x0080, u16 rel}` entries at `T`, **targets `= T + rel`** (e.g. `BGM_02`: `0xD0 + 0xC0 = 0x190`, which
  is the `E1 40 40 E0 32 E2 12 …` phrase quoted in `tsq-tvb-format.md`); `seg 0` entries are unused slots.
  This resolves the "outer record" remark in `tsq-tvb-format.md`.
* Corpus result: 20 files, **622 tracks, 0 unknown opcodes, 0 out-of-range pcs, 0 runaway, 0 zero-delta
  stalls**, 99 end on `FF`, 523 loop via `F8`, 27 forks (3600-tick horizon per track).

## 3. BGM / SFX / radio selection helpers

* `unpackBgmId` / `packBgmId(file, pp, song)`. **Correction to `audio-map.md` §2:** the byte that differs
  between the four ROOM_1 call sites (`0x0A0B0101..0104`) is the **low** byte (the whole id goes to cmd 0xE);
  `PP` (cmd 7 param) is 1 for ROOM_1 and 2 for every other id. INFERRED: low byte = song within the TSQ,
  PP = bank slot (boot registers BGM.TVB/ROOM_1.TSQ on slot 1 and BGM.TVB on slot 2, `audio-map.md` §2).
* `raceBgmId(mode20, mode22, table)` is `FUN_0022DDD0` (`0x22DE70..0x22DEC4`); the table is the 18 ids
  at `0x29F5B0` (authored mirror = 18/18 equal to the ELF). Negative `mode[+0x22]` indexes below the table in
  the original (`slti` is signed); native falls back to `BGM_02`.
* `uiSoundArg(id) = id | 0x40400000` (`0x258E50`); `decodeSfxId` splits the `0x8000` stop/loop bit.
  The id → sample map stays driver-internal (OPEN, unchanged).
* `radioRows(station)`: INFERRED binding 1 → file rows 4/5 (1CH), 2 → 8/9 (3CH), as in `radio.h`.
* Re-scan note: `jal 0x258E50` has 256 sites and `jal 0x258EC8` has 35 (both re-counted). A heuristic scan
  for immediate `a0` finds 42 distinct ids, 16 sites unresolved; 6 ids of the `audio-map.md` list
  (`0x01 0x02 0x03 0x65 0x136 0x1A4`) are not recovered by it, 19 immediates are not in the list (e.g. `0x14`,
  confirmed by `rumble-haptics.md`). Treat the `audio-map.md` UI-id list as non-exhaustive.

## 4. Rumble opcodes (`RumbleInterpreter`)

Data-only: effect scripts are authored as op lists (`rumbleEffectSteps`), assembled to the 8-byte records, and
the test proves they equal ELF `0x2EA3D0 + 8n` byte for byte (8/8); `setTable(readElfAudioTables(...).rumble)`
swaps in the ELF's own records.

Interpreter (`0x20A660..0x20A878`, re-disassembled): per port, if the effect pointer is null skip; if the hold
counter ≠ 0 decrement it and wait unless it reached 0; then loop: `0x80` small on, `0x81` small off, `0x82 lit`
large = literal, `0x83` large off, `0x84` large = stored strength, `0xFF` motors off + pointer cleared, **any other
byte = hold literal** (`0` continues immediately). A hold `N` set on frame *F* lasts frames *F..F+N−1*;
the next ops run on *F+N*. `trigger()` = `0x20ACB8`: gate `(playerByte & master) ≠ 0`, strength 0 silent,
strength ≥ 0x100 → 0xFF (negative values store the low byte, `sb`), unconditional overwrite, hold cleared so
playback starts on the next `tick()`. Native bounds: effect ≥ 8 and port ≥ 2 are rejected (the original has no check).
`rumbleToSdl` gives `SDL_RumbleGamepad(low = large·257, high = small ? 0xFFFF : 0)` arguments; the host calls
it every frame it wants the new level (the module does not time-out; durations are the script).

Coordinator wiring (not done here): own one `RumbleInterpreter`, `setGate()` from the options bytes,
`trigger()` from the collision/landing/crash sites in `rumble-haptics.md`, `tick()` once per 60 Hz frame,
forward `motors(p)` to SDL. This module is independent of the earlier `gameplay/rumble.*` grader (which
authors single-level bursts); it adds the original per-frame bytecode path.

## 5. Reproduce

```sh
cmake -S src/native -B build/w-W2-09 -DRT_NATIVE_ASSETS_BUILD_TESTS=ON -DCMAKE_BUILD_TYPE=Release
cmake --build build/w-W2-09 --target w2_audio_graph_test
RT_ELF="$HOME/Library/Application Support/RoadTripRecomp/disc/SLUS_203.98" build/w-W2-09/w2_audio_graph_test
```

`src/native/audio/audio_runtime.cpp` is a one-line shim that lets the existing EXISTS-guard link the module;
delete it when `gameplay/audio_graph.cpp` is registered explicitly.
