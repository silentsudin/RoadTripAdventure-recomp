# TSQ / TVB music data (Road Trip, SLUS-20398)

Source: byte analysis of `SOUND/` from a user-extracted disc. Parsers live in
`src/native/assets/music_seq.{h,cpp}` (tests: `tests/native_assets/test_music_seq.cpp`).

## BGM.TVB — tone bank with embedded sample pool

- Header: 256 little-endian u32 entries starting at file offset 0.
  - Entries 0–1 are zero; entries 2..255 are a **monotonic, 16-byte-aligned
    offset table** (`0x980, 0x8A50, 0x9C10, 0x10790, …`, last `0x48C00`).
  - Entry 256 begins unrelated metadata — the offset table is exactly 256
    entries (254 sample slots + 2 leading zeros).
- `BGM.TVB` size `0x49400`; sample pool spans `[0x980, 0x48C00)` **inside the
  TVB itself** — the bank is self-contained (it does NOT index the big
  `1CH/2CH/3CH*.VAG` pool files).
- Pool entries have **no `VAGp` magic**: the samples are headerless PS1-style
  4-bit ADPCM blocks (16-byte frames: one shift/filter byte, one flags byte,
  then 14 packed-nibble bytes = 28 PCM samples). Decodeable with the raw-block
  helper in `audio/vag.h`; the TVB range determines the end, not a VAG stream
  end flag.

## TSQ — sequence files (BGM_01..09, ACTION, CQ_MAIN, SELECT, …)

- Layout: 4-byte header, then a table of `{u16 segment, u16 fileOffset}`
  pointer records. `segment` is normally `0x0090`; BGM files carry one leading
  `0x0000` record. The pointer table ends at the lowest track target offset
  (offsets are not sorted).
- Track counts: BGM-family files = **50 tracks**, boundary `0xCC`;
  ACTION / CQ_MAIN = **99 tracks**, boundary `0x190`.
- Track event streams are raw bytes: BGM tracks commonly start
  `F0 FF FF 00`, ACTION tracks `E9 F0 F9 0B`.
- **Event opcode semantics**: a corpus-derived, confidence-scored grammar is
  documented below.  The parser still exposes bounded raw spans, so sequenced
  playback and exact timing/register behavior remain open TODOs.

## TSQ event grammar (analysis)

This is a byte-level grammar recovered from the Road Trip disc corpus (all
`SOUND/*.TSQ` files), not a claim that the present bounded `TsqRawEvent` spans
are each one playable stream.  In particular, adjacent short ACTION phrases
can share the span chosen from the next pointer; `FF` terminates the first
phrase even where more bytes physically follow.  A simple decoder using this
table recognizes 1,094 of 1,112 flat stream spans without an unknown byte. The
other 18 are BGM-family outer records whose `0xD0`-ish target begins a second
track-pointer layer rather than an event stream (for example, `BGM_02.TSQ`
`0x00D0`: `80 00 C0 00 80 00 9C 01 ...`).

### Core record model — high confidence

Events are one status byte plus the operands shown below.  `00..7F` are
one-byte delta times (in driver ticks); `80..DF` are one-byte note-on keys
`status & 0x7F`.  A note therefore remains active across following delta and
control events until `F0` (the decoder should not invent a MIDI-style
note-off key).  This interpretation consumes the common melody pattern
`key, delta, F0, delta` without special cases.

| Bytes | Meaning | Confidence | Road Trip evidence |
|---|---|---:|---|
| `00..7F` | advance by that many sequence ticks (`00` is explicitly present; its special runtime effect remains unverified) | high | `ACTION.TSQ:0x0197` = `0B 00`, two deltas after `F9`; `BGM_02.TSQ:0x019C` = `... 96 10 F0 27 ...`, where `10` and `27` are intervening time advances. |
| `80..DF` | note on, key = low 7 bits | high | `ACTION.TSQ:0x01A0` `A1 64 F0`: key 33, wait 100, release.  The multi-note phrase at `0x029F` is `A5 2D B1 16 F0`, keys 37 and 49 separated by deltas 45 and 22. |
| `E0 vv` | per-track volume, raw 8-bit value | high | `ACTION.TSQ:0x019C` `E0 B4`; the same phrase repeats with `E0 B4` at `0x01A8`.  Values are consistently one operand (`E0 64` at `0x029B`).  The exact raw-to-SPU gain curve is unproven. |
| `E1 ll rr` | left/right volume balance (pan plus possible gain) | high | `ACTION.TSQ:0x0199` `E1 40 00` and `0x01A5` `E1 00 40` form opposite hard balances; centered `E1 40 40` occurs at `0x0297`. |
| `E2 pp` | program/sample slot selection | high | `ACTION.TSQ:0x019E` `E2 01` immediately precedes the key; `0x02A1` uses `E2 0F`.  Observed program operands stay within the populated TVB range (e.g. ACTION reaches `0x18` at `0x0364`), consistent with TVB slots 0–29. |
| `E4 lo hi` | little-endian SPU pitch-register value / pitch bend | medium-high | `BGM_02.TSQ:0x0785` starts the repeated `E4 00 01 6F` pattern (pitch value `0x0100`, then delta `0x6F`).  Two operands are structurally certain; conversion to cents depends on the selected key/sample. |
| `E5 lo hi` | pitch-register operation that also appears to retrigger/update the voice | medium | `BGM_03.TSQ:0x0776` has `E5 83 06` in a verified inner BGM track.  Operand width is well-supported; the retrigger/ADSR side effect needs driver tracing. |
| `E8` / `E9` | likely reverb enable / disable | medium | `ACTION.TSQ:0x0194` begins `E9 F0 ...` (disable then note release); `ACTION.TSQ:0x02ED` has `E8 E0 5A` (enable followed by volume).  `E6 mm` and `E7 dd` are plausible reverb mode/depth controls in the Tamsoft family, but their apparent occurrences in this corpus are currently in unclassified pointer/outer areas, so they are not assigned here. |
| `EA lo hi` | fine pitch tuning / scale | medium | `BGM_02.TSQ:0x019B` has `EA EE 10` (`0x10EE` little-endian) between program setup and the first key; two-byte width is certain, musical-unit conversion is inferred. |
| `F0` | release/all-notes-off for this sequence track; no operand | high | `ACTION.TSQ:0x01A2` follows key `A1` and its `64`-tick delta, then is immediately followed by `FF`; `ACTION.TSQ:0x02A3` follows two keys.  It is a zero-operand opcode, not `F0 FF` as one terminator. |
| `F1 aa` | possible one-byte control, semantics unknown | very low | The apparent candidates (`BGM_02.TSQ:0x011A` and `ROOM_1.TSQ:0x115E`) fall in inner pointer-table material; do not implement this opcode without a driver trace. |
| `F8 lo hi` | signed little-endian relative jump; target is measured after its two operands | high | `ACTION.TSQ:0x02A5` is `F8 F3 FF`: next PC `0x02A8` plus signed `-13` = `0x029B`, exactly the `E1 40 40` phrase setup.  `0x02BE` `F8 F0 FF` similarly targets `0x02B1`.  Revisited targets are loop points. |
| `F9` | zero-operand control, semantics unknown | low | `ACTION.TSQ:0x0196` is followed by valid deltas `0B 00`; treating either as an operand misaligns the following `E1 40 00`. |
| `FF` | end of track/phrase | high | `ACTION.TSQ:0x01A3` ends the complete phrase beginning at `0x0194`; another complete phrase follows physically at `0x01A4`, demonstrating why raw pointer-bounded spans must still stop decoding at `FF`. |

`E3 lo hi` is only seen once by a naive flat scan, in the BGM outer metadata
area (`BGM_11.TSQ:0x010A`), not in a verified event track.  Treat it as
unassigned rather than a real event.  Likewise, this analysis deliberately
does **not** assign meanings to `EB..EF`, `F2..F7`, or `FA..FE`: none is needed
to decode the observed event paths, and a byte in an unreachable/outer region
is not evidence of an opcode.

### Fully traced short phrases

* `ACTION.TSQ:0x0194..0x01A3`:
  `E9 | F0 | F9 | 0B | 00 | E1 40 00 | E0 B4 | E2 01 | A1 | 64 | F0 | FF`.
  This simultaneously fixes the zero operand widths of `E9`, `F0`, and `F9`,
  the two operands of `E1`, the one operands of `E0`/`E2`, key `A1`, its
  100-tick duration, and end marker.
* `ACTION.TSQ:0x0291..0x02A7`: after setup, `A5 2D B1 16 F0 35 F8 F3 FF` is
  two notes with intervening waits, release, a 53-tick wait, and the signed
  backwards jump described above.  It rules out an SSQ-like multi-byte
  note-on record for this data.
* `BGM_02.TSQ:0x0190..0x01AD` (inside the BGM inner track table):
  `E1 40 40 E0 32 E2 12 EA EE 10 96 10 F0 27 E0 34 9D 0F F0 1F ...`.
  This corroborates the same control/note/delta/release grammar in a long BGM
  and shows program `0x12`, pitch-scale `0x10EE`, keys `0x16`/`0x1D`, and
  repeated release timing.

All of the above has since been PROVEN against `SNDMOD.IRX` itself
(`sndmod-irx.md`): tick rate, opcode dispatch/widths (incl. `E3` tempo = 2
operands, `F1` = priority change, `F9` = external channel, `E6/E7/E8/E9` =
reverb mode/depth/on/off), gain math, and the pitch table. Remaining open
work is only bit-accurate SPU ADSR/reverb curves.

## Native offline-render constants (now PROVEN against SNDMOD.IRX)

`assets/tsq_sequencer.{h,cpp}` provides a deliberately device-independent PCM
preview, not an emulation of the SNDMOD driver: **60 ticks/sec** (proven —
`thread_sddr` ticks once per `WaitVblankEnd`), headerless TVB PCM assumed
naturally **44100 Hz**, **key 48 = sample natural pitch** (proven — `freqtbl`
at IRX 0x8A9C is 4096 at key 48 and doubles every 12 keys, i.e.
`4096·2^((key-48)/12)`),
E4/E5/EA signed `value/4096` semitone modifiers (structurally confirmed),
linear E0 `value/127` and E1 `ll/64`,`rr/64` gains (proven up to the global
SPU master scalar). See `sndmod-irx.md`; remaining open items are only the
bit-accurate ADSR/reverb curves.
