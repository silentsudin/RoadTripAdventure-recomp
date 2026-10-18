# W3-05 event bridge: verified dialects and coordinator integration

Module: `src/native/gameplay/event_bridge.{h,cpp}`. Test:
`tests/native_w2/event_bridge_test.cpp`, target `w3_script_bridge_test`.

## Grounding / corrections to the wave-3 premise

**Do not map 0x8A to radio or 0x8B to cutscene.** Neither mapping is
established by the nominated sources. At `0x23B140`, the town prefix VM tests
`byte < 0x20`; all higher bytes return to the text renderer. That renderer
treats high bytes as wide-character leads. Scanning dialogue text for these
bytes would create spurious gameplay events. Existing `ScriptVM` continues to
own actual prefix flag mutations (0x0F/0x10 etc.; see `script-vm.md`).

The separate pad-pattern interpreter, inside the function indexed as
`UpdateMenuInputState` (`0x20A660`) in `functions.tsv`, has these records:

| Opcode | Total width | Native record | Proof |
|---|---:|---|---|
| 80 | 1 | SmallOn | branch 20A724, handler 20A760 |
| 81 | 1 | SmallOff | branch 20A72C, handler 20A770 |
| 82 | 2 | LargeValue, raw u8 payload | 20A780..20A798 saves incremented pointer and reads literal |
| 83 | 1 | LargeOff | 20A744..20A74C routes to 20A7A0 |
| 84 | 1 | LargeInitial | 20A740 routes to 20A7B0; reads strength set by trigger |
| 85..8B | 1 | Hold, value = original opcode | default 20A7E0..20A7E4 writes byte to hold counter |
| FF | 1 | End | 20A750 routes to 20A7C8; clears script pointer/motors |

The other default bytes are also one-byte Hold records. `LargeInitial.value`
is **UNKNOWN** (`valueKnown=false`): its strength comes from runtime state,
not from an operand. No subtitle, flag-bank, radio-station or camera operand
exists in these pad records. Native subtitle/flag events are explicit producer
records only; flag bank/meaning is host-owned, not inferred.

Live pointer table `0x29AD60` yields eight pattern starts `0x2EA3D0`,
`0x2EA3D8`, `0x2EA3E0`, `0x2EA3E8`, `0x2EA3F0`, `0x2EA3F8`, `0x2EA400`,
`0x2EA408`. The test extracts each eight-byte span at runtime and replays to
FF (36 records total). Independent sequence pin at `0x2EA3D8..0x2EA3DD`:
SmallOn, LargeInitial, Hold(2), SmallOff, Hold(6), End. Literal intensity pin
at `0x2EA408`: LargeValue(127), Hold(30), End at `0x2EA40B`.
No extracted stream bytes are shipped in this module.

**Countdown remains ungrounded.** `0x21CE48..0x21CF7C` performs integer
ratio divisions and compares to 8201/5000/8000, with index increments/decrements
through a signed-halfword table. This is consistent with ratio/gear selection,
not a 3-2-1-GO timing gate. The neighbouring function-index labels do not
override instruction evidence. No countdown timing law or ElementA animation
has been fabricated here. The four-vec3 camera claim at 0x29CE00 was not used
or verified. These are open coordinator RE tasks, not passing deliverables.

Reproduce instruction grounding:
`python3 tests/native_w2/event_bridge_grounding.py` (read-only, needs Capstone).
Code uses per-word byte reversal then MIPS64 big-endian disassembly; data uses
little-endian `struct`. Important additional correction: this ELF's `.data` is
0x29A880, so `.data+0x7FF0` is **not** GP. `.reginfo` explicitly gives
GP=0x33AAF0, corroborated by `gp-0x5450 = 0x3356A0` at 20A6E4. Consult that
register value rather than the incorrect shortcut for this image.

## Queue contract

Fixed 64-record FIFO; each record owns up to 256 payload bytes. Overflow rejects
the new event without moving the bytecode cursor. `pump` has a default 64-record
step budget, emits type/opcode/payload/width/address, and advances the caller's
persistent stream cursor only after successful enqueue. `drain` removes before
callbacks and dispatches only its initial queue size: callback-produced events
wait for the next drain, recursive drains return zero. Single-threaded; callbacks
must not throw. `onEvent` observes all records; specific callbacks are additional
notifications, so do not apply the same side effect in both callbacks.

This is a **record decoder, not a second rumble timing interpreter**. Pumping a
whole pattern does not mean all motor changes should execute in one frame.
`audio_graph::RumbleInterpreter` already implements the actual hold counter,
strength/gate state and 60-Hz timing. Use it for device playback; use this queue
for decoded observations, or a host scheduler that respects Hold records. In
particular Hold(0) is not a terminator. FF is the only end record.

## Exact coordinator integration (main.cpp was not changed)

1. Include `gameplay/event_bridge.h`. Own one `events::EventBridge` and
   `events::Callbacks` beside the session/radio/rumble objects, outside the frame
   loop. Keep one cursor per active pad stream; reset it **only on a new trigger**.
   Load live patterns through `audio_graph::readElfAudioTables`, not embedded
   ROM arrays. Pump only proven pad records with `Dialect::Rumble`.
2. Feed explicit native events when gameplay actually produces them:
   `bridge.enqueue(Type::Radio, station)`, `Countdown, start ? 1 : 0`,
   `CameraPreset, k`, `Subtitle, 0, rawTextBytes`, and `FlagSet/FlagClear, id`.
   These calls do **not** assert a ROM opcode mapping. Check the returned bool;
   retain/retry producer input on queue-full rather than dropping/replaying it.
   Do not feed raw prefix or text bytes into the rumble decoder.
3. In `src/native/app/main.cpp`, inside the fixed-step gameplay body, place VM/
   native event production and `bridge.drain(callbacks)` immediately **before**
   `session.update(menuInput, testSocket.lockstep() ? &input : nullptr)`
   (currently near line 2115). This applies flags/camera state before the
   snapshot, and station requests before the existing EXPLORE
   `radio.onTownFrame()` block (near 2134). Do not drain again per render pass.
   If script tasks instead run inside `GameLoop`, put the drain immediately
   after `tasks_.tick()` and before physics/snapshot in both tick overloads;
   choose one integration location, not both.
4. `callbacks.onRadio` should validate station 0..2 then call
   `radio.requestStation(static_cast<rt::native::gameplay::RadioStation>(station))`
   (`gameplay/radio.h:137`; the module callback deliberately uses u32).
   `onCountdown(bool)` and `onCameraPreset(u32)` are exposed, but their actual
   host consumers/timing remain coordinator work. Camera k must be validated
   against a verified preset table. `onEvent` can queue subtitle bytes for the
   existing dialogue layer and route flags to the proper ScriptVM host/save
   bank; never guess a flag bank from the numeric id alone.
5. For motors, keep the existing device null checks and gameplay-exit stop.
   Use `audio_graph::RumbleInterpreter::trigger/tick/motors` once per 60-Hz step,
   then `rumbleToSdl`; do not apply a predecoded pattern in one burst of callbacks.
   The existing main.cpp `rumbleChannel.tick()` block is near line 2195.
6. If `PumpResult.logOnce`, log status/opcode/address once (the bridge dedupes
   reason/dialect/opcode over its lifetime). 85..8B in the pad dialect are known
   one-byte Hold records and can advance normally. A truncated 82 has proven
   width two but must **not** read/skip beyond the supplied buffer: stop and
   request the missing byte, or abandon that stream. An unsupported town high
   byte has **UNKNOWN width**, so stop/return it to the text VM, not skip one or
   swallow arbitrary operands. QueueFull/StepLimit are resumable, not malformed.

Host CMake source/test registration is present in one isolated appended block.
For Android integration, the coordinator should move `gameplay/event_bridge.cpp`
into the staged-source list before the Android early `return()`; the appended
host block intentionally leaves the shared staged list untouched in this worker.
