# HANDOFF W4-08 — script/GSL VM opcode canon + "ElementA" semantics

Files (mine only): `src/native/gameplay/script_ops.{h,cpp}`, `tests/native_w8/{script_ops_test.cpp,CMakeLists.txt}`,
this file, and ONE line in `src/native/CMakeLists.txt` (the `add_subdirectory(.../tests/native_w8 ...)` line directly
above `add_executable(w3_enhancements_test`, inside `if(RT_NATIVE_ASSETS_BUILD_TESTS)`).
`script_ops.cpp` is **not** added to `rt_native_assets`; the test compiles it directly, so the coordinator can add it to
the library source list whenever a consumer exists (it has no dependencies beyond the STL).

Test target: `w8_script_ops_test` — `cmake -S src/native -B build/native-w408 -DRT_NATIVE_ASSETS_BUILD_TESTS=ON`
then `cmake --build build/native-w408 --target w8_script_ops_test`, run `build/native-w408/native_w8/w8_script_ops_test`
from the repo root. Part 1 needs nothing; part 2 (ELF/IRX cross-checks) prints `SKIP` and still exits 0 when the images are
absent. A standalone configure also works: `cmake -S tests/native_w8 -B build/native-w408-t`.

## 1. Findings that change the brief's vocabulary (read first)

**There are three unrelated byte-stream languages, not one.** Opcode numbers overlap across them, so the table is
per-`Dialect`:

| Dialect | Interpreter | Image | Where the brief's facts live |
|---|---|---|---|
| `TownPrefix` | `InterpretScriptCommands` `0x0023AD00` | SLUS_203.98 | widths of 03/08 (corrected), `>=0x20` = text |
| `PadPattern` | per-frame loop inside `0x0020A660` | SLUS_203.98 | `0x85..0x8B` rumble holds |
| `TsqTrack` | `play 0x5294` in `SNDMOD.IRX` (IOP) | SNDMOD.IRX (file off = vaddr + 0xA0) | **EA / E5 / F9 / delta-0 stall** |

1. **EA / E5 / F9 / "delta 0 stalls" are TSQ-sequencer opcodes (SNDMOD.IRX), not road-section elements and not in the
   rumble or town streams.** Every one of those facts is already stated in `audio-runtime.md` §2 and re-proven below.
   No "road section element" opcode stream exists in anything I found; the label in the brief is a mislabel.
2. **"ElementA" has exactly one referent in the ELF: the HUD builder `raceHudElementA` `0x0022EE70`** (44-byte arena
   records, `type` 1/2/3: centred sprite batch / rotated quad / proportional fill). It is a C struct builder, not a byte
   stream (`build/decomp/fn/0022ee70.c`), so it has **no opcode table**. It shares only a name with the TSQ facts.
   I did not implement it.
3. `gsl-vm.md` §5.1 draft widths: confirmed wrong. Op `03` = **4** bytes, op `08` = **3** bytes; op `0E` is 7 bytes (six
   operand bytes, not "a 6-byte span"). `script-vm.md` §3 already carries the right values; `gsl-vm.md` §5.1 does not.
4. **`0x85..0x8B` are rumble HOLD bytes** (one byte each, hold = the byte value in frames, i.e. 133..139), via the
   pad-interpreter default arm `0x20A7E0`. They are not radio/cutscene triggers in any dialect:
   `TownPrefix`: every byte `>=0x20` ends the prefix (`0x23B140`); `TsqTrack`: they are note-on keys 5..11 (`80..DF`).
   **Caveat:** none of the 8 shipped pad records (`0x2EA3D0..0x2EA40F`) contains `0x85..0x8B`; the largest hold used is
   30. The ruling is about interpreter behaviour, not about a shipped record using them.
5. Delta byte 0 in TSQ delays the next event by **65536** ticks (not 65535): see §4.

## 2. API summary (`script_ops.h`)

```cpp
enum class Dialect { TownPrefix, PadPattern, TsqTrack };
struct OpRow { uint8_t op; uint8_t width; const char* name; uint32_t grounding_addr; };   // width 0 = UNKNOWN
std::span<const OpRow,256> opTable(Dialect);  const OpRow& lookup(Dialect, uint8_t);
WalkResult walkRecord(Dialect, std::span<const uint8_t>, const RecordFn& = {});            // deterministic
// WalkStatus: Exhausted | Terminated(FF consumed) | TextStart | UnknownWidth | Truncated | Stopped
// WalkResult { status, records, consumed, remaining }  -- remaining begins AT the unknown/truncated byte
decodePad(Record) -> PadRecord{kind,value,valueKnown};  padIsHold / padIsHoldHighRange
decodeTsq(Record) -> TsqRecord{kind,b0,b1,u16,s16};      tsqTicksUntilNextEvent / tsqStoredCountdown / tsqRelativeTarget
```
Rules the walker obeys: dispatch by table width only; an unknown-width byte (town `00 09 0A 0C 1E 1F`, TSQ
`EB..EF F2..F7 FA..FE`) stops the walk and is *not* consumed; a record that would run past the buffer returns `Truncated`
(never over-reads); town branch ops are walked on their not-taken (skip) path — the executor is `gameplay/script_vm`.
No countdown opcode is invented anywhere.

## 3. Evidence reproduced this pass (all from the ELF/IRX, capstone 5.0.7, LE words byte-reversed → MIPS big-endian decode)

* **PadPattern** `0x20A660..0x20A87C` re-disassembled. Dispatch: `0x20A710 beq v1,0x82 → 0x20A780`; `0x20A718 slti v1,0x83`
  (`<0x83`: `0x20A724` ==0x80 → `0x20A760`, `0x20A72C` ==0x81 → `0x20A770`, else → `0x20A734 b 0x20A7E0`);
  `>=0x83`: `0x20A740` ==0x84 → `0x20A7B0`, `0x20A744 slti 0x84` (=0x83) → `0x20A7A0`, `0x20A750` ==0xFF → `0x20A7C8`,
  else `0x20A758 b 0x20A7E0`. Default arm `0x20A7E0 lbu v1,(v0)` / `0x20A7E4 sb v1,(t1)` stores the **byte itself** into the
  hold counter; the loop re-enters at `0x20A708` only when that value is 0 (`0x20A7F4 beqz`). Hence every byte other than
  `80..84, FF` (and `82`'s literal) is a hold, including `0x00` and `0x85..0xFE`. The 8 records (36 opcodes) walk cleanly and
  all end on `FF`.
* **TsqTrack** `play 0x5294` dispatch `0x5368..0x5640` re-disassembled; each handler's operand reads counted from its
  `lbu/lb + addiu 1` pairs (`volume 0x4F74` 1, `panpot 0x4DAC` 2, `tone_chg 0x4D34` 1, `tempo 0x5110` 2 and stores nothing,
  `pitch 0x5184` 2, `key_pitch 0x4A40` 2, `pitch_cent 0x48BC` 2, `pri_chg 0x49B4` 1, `data_jump 0x482C` 2, `ext_chan 0x45D4` 2,
  `E6/E7` 1 inline `lb` at `0x54E0/0x5520`, `E8/E9` inline `sb 1/0,0x17(track)`).
* **TownPrefix**: jump table `0x2EC750` words equal the case vaddrs (checked by the test when the ELF is present); skip arms
  that fix widths: `0x23AFD8 addiu s0,s0,3` (03, 1D → 4), `0x23AE4C addiu s0,s0,2` (08 → 3), `0x23AF0C addiu s0,s0,6` (0E → 7);
  `s0 = opcode+1` on handler entry (`0x23AD34`).
* **Corpus checks** (scratch tool `build/scratch/w4-08/corpus.cpp`, links the real `script_ops.cpp`; untracked):
  * Town: 3-level pointer walk reproduces **21 towns / 307 level-2 tables / 2478 distinct script starts**; walking each with
    `TownPrefix` gives **2476 TextStart (clean) / 2 UnknownWidth** (starts `0x333F40`, `0x333FC0`, both zero bytes — the
    "points at zero bytes" references from `script-vm.md` §6), 1429 records. Mutation: with the gsl-vm.md draft widths
    (03=3, 08=2) the same corpus gives **72 unclean** — the corrected widths are what keeps the corpus decodable.
  * TSQ: 622 track starts across 20 `SOUND/*.TSQ` files (matches `audio-runtime.md`'s 622) + 28 F9 fork targets = 650 linear
    walks: **498 stop on an `F8` loop, 152 end on `FF`, 0 UnknownWidth, 0 Truncated, 0 delta-0, 0 `F3`**. Opcode histogram of
    reached records: nonzero deltas 84491, notes 40636, E0 20639, E1 1477, E2 1497, E4 10303, E5 330, E8 422, E9 81, EA 399,
    F0 28399, F8 498, F9 28, FF 152 (E3/E6/E7/F1 never reached).

## 4. Countdown / delta-0, reconciled

`step_t 0x47F0..0x4814`: `a1 = (u8)D; sh (a1-1), 0x14(track)`. `play 0x52C4..0x52DC`: each tick `cnt = (s16)(cnt-1)`; stream is
read only when `cnt == -1`. Therefore `D in 1..0x7F` → next event `D` ticks later; `D == 0` stores `0xFFFF` and the counter must
wrap the whole s16 range: **65536 ticks later** (= 65535 ticks beyond the 1-tick minimum, which is probably the origin of
"stalls 65535"). `tsqTicksUntilNextEvent()` encodes this and the test checks it against an independent counter model for all
128 delta values. After a delta `play` returns for that tick (`0x5384 j 0x5660`); note-on/E*/F* arms loop (`j 0x5658`).
`F9` child: countdown forced to 0 (`0x47D8`) → reads on its next tick; child pc = parent pc-after-operands + s16 (`0x47CC`).

## 5. What remains UNPROVEN (stated, not guessed)

* **F3.** Interpreter behaviour is **proven**: `0xF3` is not compared anywhere in the `play` dispatch (`0x5368..0x5640`), so it falls
  to the default arm `0x5640 → data_end 0x4940` (clears the track active byte; if the voice channel matches, also resets the voice)
  = stops that track, identical to `FF`. What is **not** provable from the binary: any *intended* meaning of F3 in the authoring
  tool. It never occurs as an opcode in the 650 walked tracks (57 raw `F3` bytes exist in the TSQ corpus; the one place a naive
  scan hits it deliberately is `ACTION.TSQ:0x2A5 F8 F3 FF`, where `F3 FF` are the little-endian operand `-13` of `F8`). The table
  therefore lists F3 as UNKNOWN width (walker stops) with the grounding addr `0x5640`, not as a named opcode.
* **"ElementA countdown".** Two different things, neither resolved:
  (a) the TSQ per-track tick countdown — **fully resolved** (§4);
  (b) the HUD `raceHudElementA` type-3 fill (`value/max`, `+1` per call to `max`; start-bars described in `hud-layout.md` as 30 frames) —
  its role as the 3-2-1-GO timer is **unproven**: who writes `max`/`value`, and whether anything else gates the start, was not
  traced here, and `event-bridge.md` already records that `0x21CE48..0x21CF7C` is ratio/gear logic, not a countdown. No
  countdown opcode exists in `TownPrefix` or `PadPattern`.
* `E3` tempo: operands consumed, **nothing stored** by `0x5110` (so tempo is inert in this driver revision); whether another path
  reads them was not searched.
* `PadPattern 0x84` strength is runtime state (trigger arg, `gp-0x5450+player`), so `valueKnown=false` — no value is invented.
* `TownPrefix` unknown-op swallow rule (`0x23B0E0`) has data-dependent length, so it has no width here (walker stops).

## 6. Proposed doc text

### 6a. `docs/native-reference/script-vm.md` — replace §3 heading text and append §3b/§3c

Keep the existing §3 town table (it is correct against the ELF — verified again: all 26 live widths, skip arms at
`0x23AFD8/0x23AE4C/0x23AF0C`). Add under it:

> Canonical machine-readable form: `src/native/gameplay/script_ops.{h,cpp}` (`Dialect::TownPrefix`, 256 rows, one grounding vaddr
> each; `tests/native_w8/script_ops_test.cpp` checks the rows against `0x2EC750`). Corpus: 2476/2478 distinct starts decode
> to `>=0x20`; the 2 exceptions are zero-byte starts.

Replacement width table (total bytes incl. opcode; case vaddr = jump-table word at `0x2EC750`):

| op | w | name | case vaddr | | op | w | name | case vaddr |
|---|---|---|---|---|---|---|---|---|
| 01 | 3 | if_flag_set_branch | 23AD50 | | 11 | 3 | clear_flagged_slot_bit | 23B028 |
| 02 | 3 | if_flag_clear_branch | 23AD70 | | 12 | 1 | parent_done_result10 | 23B048 |
| 03 | **4** | if_slot_count_branch | 23AD90 | | 13 | 2 | record_task_result | 23B060 |
| 04 | 2 | if_stamp_flag_branch | 23ADA8 | | 14 | 3 | warp_town_shop | 23B078 |
| 05 | 3 | if_flag_fc68_branch | 23ADC8 | | 15 | 2 | set_state_bit_fc88 | 23B0B0 |
| 06 | 3 | if_state_bit_branch | 23ADE8 | | 16 | 2 | if_progress_for_pair_branch | 23AE58 |
| 07 | 2 | if_slot_presence_branch | 23AE08 | | 17 | 2 | set_progress_flag_queue | 23B0C8 |
| 08 | **3** | slot_pair_three_way | 23AE20 | | 18 | 2 | if_word14_out_of_range_branch | 23AE78 |
| 09 0A 0C | — | unknown (report, data-dependent swallow) | 23B0E0 | | 19 | 3 | if_progress_flag_branch | 23AEA8 |
| 0B | 3 | if_president_result_branch | 23AEC0 | | 1A | 3 | if_bits_left_branch | 23AF38 |
| 0D | 3 | if_flag_fca8_clear_branch | 23AEE0 | | 1B | 2 | license_gate | 23AF60 |
| 0E | **7** | task_indexed_jump (6 idx bytes) | 23AF00 | | 1C | 3 | if_town_equals_branch | 23AFA0 |
| 0F | 2 | set_flag_fc98 | 23AFF8 | | 1D | 4 | if_object_attr_branch | 23AFC0 |
| 10 | 2 | clear_flag_fc98 | 23B010 | | 00 1E 1F | — | unknown (as above) | 23B0E0 |
| | | | | | `>=20` | — | text; ends the prefix | 23B140 |

New §3b — the second VM that shares no opcodes with §3 (per-frame pad-pattern interpreter, inside `0x20A660`):

| byte | w | meaning | handler | dispatch compare |
|---|---|---|---|---|
| 80 | 1 | small motor on | 20A760 | 20A724 |
| 81 | 1 | small motor off | 20A770 | 20A72C |
| 82 | 2 | large motor = literal operand (so `82 FF` is value 255, not End) | 20A780 | 20A710 |
| 83 | 1 | large motor off | 20A7A0 | 20A744/20A748 |
| 84 | 1 | large motor = strength given to the trigger call (runtime state, not an operand) | 20A7B0 | 20A740 |
| FF | 1 | end: motors off, effect pointer cleared | 20A7C8 | 20A750 |
| `00..7F`, **`85..FE`** | 1 | **hold**: hold counter := the byte; `0` continues the same frame | 20A7E0 (default arm) | falls through 20A734 / 20A758 |

Ruling: **`0x85..0x8B` are ordinary hold bytes (133..139 frames).** They are not radio or cutscene triggers in any of the three
dialects. None occurs in the 8 shipped records at `0x2EA3D0 + 8n` (36 opcodes, max hold 30).
Also correct `rumble-haptics.md`: "literals `0x01..0x7F` = frame hold" → every byte except `80..84`/`FF` (incl. `00`, `85..FE`).

New §3c — SNDMOD.IRX track VM (`play 0x5294`; addresses are IRX vaddrs, file offset = vaddr + 0xA0):

| byte | w | name / effect | handler |
|---|---|---|---|
| `00..7F` | 1 | delta D: next event D ticks later (**D=0 → 65536**) | 47F0 |
| `80..DF` | 1 | note-on key = b&0x7F; `freq = freqtbl[key]` (`*bend>>12` if +0xC≠0) | 4BAC |
| E0 | 2 | volume | 4F74 |
| E1 | 3 | pan L,R | 4DAC |
| E2 | 2 | program | 4D34 |
| E3 | 3 | tempo — operands consumed, nothing stored | 5110 |
| E4 | 3 | u16 → freq (+0xE), bend applied, **no** retrigger | 5184 |
| E5 | 3 | u16 → **absolute freq** (+0xE) and **retrigger** (when track channel matches the voice) | 4A40 |
| E6 / E7 | 2 | reverb mode / depth | 446C / 4518 |
| E8 / E9 | 1 | reverb on / off | 554C / 5570 |
| EA | 3 | u16 → **Q12 bend multiplier** (+0xC) | 48BC |
| F0 | 1 | key off | 4CE4 |
| F1 | 2 | priority | 49B4 |
| F8 | 3 | jump: pc = (pc after operands) + s16 | 482C |
| F9 | 3 | **fork**: copy track to a free slot 8..0x2F, child pc = (pc after operands) + s16, child countdown 0 | 45D4 |
| FF | 1 | data_end (track stops) | 4940 |
| `EB..EF F2..F7 FA..FE` (incl. **F3**) | — | not dispatched → default arm 5640 → data_end (track stops); no data uses them | 5640 |

### 6b. `docs/native-reference/gsl-vm.md` §5.1

Replace the opcode-draft table rows for `0x03` ("`u8 a, u8 b`") with "`u8 slot, u8 bit, u8 idx` (width 4)", `0x08` with "`u8 idxA, u8 idxB`
(width 3; three-way: MatchSlotPairState → table[idxA]; elif TestFlagFcb8 → skip; else table[idxB])", `0x0E` with "six idx bytes, width 7",
and add: "Widths above are superseded by `script-vm.md` §3 / `src/native/gameplay/script_ops.cpp` (checked against the ELF)." In §5.2 add a
sentence: "ElementA is a struct builder, not an opcode stream; the sequencer opcodes EA/E5/F9 belong to `SNDMOD.IRX` (`audio-runtime.md`)."

### 6c. `event-bridge.md`

The 85..8B row is confirmed and can cite `script_ops.cpp` PadPattern row `0x20A7E0`. The "countdown remains ungrounded" paragraph stays
accurate; add that the TSQ tick countdown (SNDMOD) is resolved (§4 above) and unrelated to a 3-2-1-GO gate.

## 7. Integration notes for the coordinator

* To use the walker from the app: add `gameplay/script_ops.cpp` to the `rt_native_assets` source list (and the Android staged list
  before its early `return()` if needed). It has no dependencies.
* `event_bridge`'s `Dialect::Rumble` decoder and `script_ops` `PadPattern` give the same count on the 8 records (36 opcodes; the bridge's per-record decode was not diffed); `event_bridge` may be
  re-pointed at `script_ops` later but nothing requires it.
* Not touched: `main.cpp`, any `docs/native-reference/*`, `src/native/media/*`, race_hud*, hud_layout.h, hud_live.h, `tests/native_w2/*`.
