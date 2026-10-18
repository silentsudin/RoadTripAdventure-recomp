# Rumble / haptics — grounded model for the native rebuild

`src/native/gameplay/rumble.{h,cpp}` + `tests/native_assets/test_rumble.cpp` (this file
documents them). Retention-audit row "Rumble / haptics" was **MISSING**; the points
below close it with the collision-rumble family from `README.md:132`
(`HandleCarCollisionResponse 0x00217830`) plus the pad-effect machinery around it.

All ELF facts were disassembled directly from SLUS_203.98 with capstone
(`vaddr = fileOffset + 0x1FF000`, verified in [save-layout.md](save-layout.md)).
R5900 multimedia words appear as `.word`; `daddu $r, $x, $zero` (0x02-reg moves) and
delay-slot argument setup were decoded by hand. Lengths below say "1: ..." where a
signal value is a single instruction pair quoted in prose; no game data is
reproduced in the module (envelope semantics only).

---

## 1. Evidence ledger

### Primitive — the rumble trigger entry

| vaddr | what | verdict |
|---|---|---|
| `0x0020ACB8` | `rumbleTrigger(state* a0, int player a1, int effectId a2, int strength a3)`; no row of its own in functions.tsv (callee address only) | CONFIRMED |
| `0x0020ACBC` | `lbu a1, 0xE(a0)` — master rumble byte of the gate struct | CONFIRMED |
| `0x0020ACC4` | `lbu v0, 0xC(a0+a1)` — per-player enable byte | CONFIRMED |
| `0x0020ACC8` | `v0 &= a1`; `beqz v0 → return` — rumble allowed only when `(playerByte & masterByte) != 0` | CONFIRMED |
| `0x0020ACD4` | `beqz a3 → return` — strength 0 is silence (the deadzone) | CONFIRMED |
| `0x0020ACDC`–`0x0020ACE4` | `a2 <<= 2`; `v0 = *(0x2A0000 + a2*4 - 0x52A0)` — effect-pointer table at `0x0029AD60` indexed by effect id `a2` | CONFIRMED |
| `0x0020ACF4` | `slti a0, a3, 0x100` + `movz a3 = 0xFF` on false — strengths saturate at 255 | CONFIRMED |
| `0x0020AD04` | `sb a3, gp-0x5450+player` — per-player strength byte | CONFIRMED |
| `0x0020ACFC` | `sb zero, gp-0x5458+player` — per-player hold counter cleared (playback starts next frame) | CONFIRMED |
| `0x0020AD0C` | `sw v0, gp-0x7798+player*4` — per-player effect pointer | CONFIRMED |

Arbitration note: the primitive **overwrites unconditionally** — any new event
replaces the playing one (last caller wins). No priority order exists in the
original; the native priority shield is AUTHORED (below).

### Effect table and envelopes

| vaddr | what | verdict |
|---|---|---|
| `0x0029AD60` | 8-entry pointer table, entry *n* → record *n* (`0x2EA3D0 + 8n`) | CONFIRMED |
| `0x2EA3D0`–`0x2EA410` | 8-byte per-effect scripts, bytecode walked one opcode per frame | CONFIRMED |

Decoded scripts (semantics only; the module stores no verbatim record bytes):

| id | decoded script | motors | total frames @60Hz | original callers |
|---:|---|---|---:|---|
| 0 | end | off | 0 | none observed |
| 1 | small on; large = **event strength**; hold 2; small off; hold 6; end | both, then large only | 8 | wheel landing (`0x0021B360`) |
| 2 | small on; large = **event strength**; hold 4; end | both | 4 | **none observed** (adopted for native scrapes) |
| 3 | small on; hold 1; small off; large = 0xFF; hold 2; end | click then max | 3 | impulse codes 2/8/10 |
| 4 | large = 0xFF; hold 1; small on; hold 1; end | max then click | 2 | impulse codes 1/4/5 |
| 5 | small on; large = 0xFF; hold 2; end | both, max | 2 | impulse codes 3/6/7/9 |
| 6 | large = **event strength**; hold 4; small on; hold 2; small off; hold 2; end | scaled + click | 8 | car↔car `0x0021793C`/`0x002179A0`, object↔car `0x00253C9C` |
| 7 | large = 0x7F; hold 30; end | fixed 127/255 | 30 | options-screen test buzz `0x00208EA0`/`0x00208EB4` |

### Per-frame interpreter (the "haptics device driver")

| vaddr | what | verdict |
|---|---|---|
| `0x0020A660` | per-frame pad-input update that also services rumble (per player, `slti … 2` loop bound ≡ 2 ports) | CONFIRMED |
| `0x0020A6B0`–`0x0020A6C4` | hold counter (gp-0x5458+player) decrements once per frame; 0 → execute next opcode | CONFIRMED |
| `0x0020A708`–`0x0020A7E0` | opcode dispatch: `0x80` small on, `0x81` small off, `0x82 <lit>` large = literal, `0x83` large off, `0x84` large = stored event strength (gp-0x5450+player read at `0x0020A7B4`), and **every other byte except `0xFF`** is a frame hold via the default arm `0x0020A7E0` (`lbu` then `sb` to the counter): this includes `0x00` (zero-frame, immediately continues) and `0x85..0x8B` (133..139-frame holds), not only `0x01..0x7F`; `0xFF` = end (all motors off, pointer cleared) | CONFIRMED |
| `0x0020A760`/`0x0020A770` | small motor is **binary** (0/1) | CONFIRMED |
| `0x0020A85C`/`0x0020A864` | per-player actuator pair cleared: small at gp-0x5448+2p, large at gp-0x5447+2p | CONFIRMED |
| `0x0020A7FC`–`0x0020A878` | idle/park sweep around the motors (counter to 0x168, clears at ≥0xB4) — purpose | INFERRED |

These are the two actuator channels a DualShock exposes through libpad
(`scePadSetActDirect`-style, comparable to the IOP pad modules loaded by
`Ps2InitSystem 0x002001B8`): one small/high-frequency motor (binary) and one
large/low-frequency motor (0..255).

### Call sites (all `jal 0x0020ACB8`, 8 sites found)

| vaddr | site | parameters | verdict |
|---|---|---|---|
| `0x00217870`–`0x00217880` | collision strength = `|Δv| (float) × 1000.0f` (`lui 0x447A` = 1000.0f; `mul.s`) | grade formula | CONFIRMED |
| `0x00217930` | `cvt.w.s` + `mfc1 a3` — float grade to integer strength | CONFIRMED |
| `0x0021793C` | car↔car car A: `a0=gateStruct, a1=flags&1, a2=6, a3=strength` | CONFIRMED |
| `0x00217944` | collision SFX id `0x14` via `PlayUiSound` right after the rumble | CONFIRMED |
| `0x002179A0` | car↔car car B: mirrored (same gate struct, same strength, each car's own player bit) | CONFIRMED |
| `0x0021787C`/`0x00217920` | gates: initiator flags `&3 != 0` selects the response; flags `&0x0FB0 == 0` required for rumble | CONFIRMED |
| `0x00217914`/`0x00217918` | per-car suppression `car+0x210 (h16) == 0` required on both cars | CONFIRMED |
| `0x00217898` + `0x00217908` | gate struct word `+0x28`, bit `0x40` set → no rumble | CONFIRMED |
| `0x00253C9C` | object↔car: `a2=6`, `a3=0x40` — fixed strength 64 | CONFIRMED |
| `0x00253C88` | object gate `(flags & ~3) == 0`; `0x00253C94` player = `flags&1` | CONFIRMED |
| `0x0021B310`–`0x0021B360` | landing sites: speed from per-wheel slot (`lw a3, (s1)`); suppression `[gate+0x28] bit 0x40` at `0x0021B31C`–`0x0021B324`; flags gate at `0x0021B330`; `slti v0, a3, 0xB3` → speed ≥ 179 at `0x0021B33C`; negative-adjust + `sra a3, a3, 4` → strength = speed >> 4 at `0x0021B348`–`0x0021B358`; call with `a2=1` (pattern 1) | landing path | CONFIRMED |
| `0x00219C0C`–`0x00219C14` | impulse-site gates: `flags & 0x0FB0 == 0` | CONFIRMED |
| `0x00219C2C` | `sltiu v0, code, 0xB` — the rumble dispatch exists only for impulse codes 0..10 | CONFIRMED |
| `0x2EAE60` | 11-entry jump table for codes 0..10 (entries 11+ fall into string data — out-of-range) | CONFIRMED |
| `0x00219C50`/`0x00219C5C` | arm: pattern 4 for codes {1,4,5}, strength `0xFF` | CONFIRMED |
| `0x00219C60`/`0x00219C68` | arm: pattern 3 for codes {2,8,10}, strength `0xFF` | CONFIRMED |
| `0x00219C80`/`0x00219C88` | arm: pattern 5 for codes {3,6,7,9}, strength `0xFF` | CONFIRMED |
| `0x002208E98`/`0x002208EB0` | options test buzz: pattern 7, strength 1 (ignored by the fixed literal), for player 0 then player 1 | CONFIRMED |

**Correction to `functions.tsv`:** the `0x00219660` note ("collision code 0–15…
code 15 = crash: prints, sets flag, rumble") is partly **[inferred]**: the rumble
dispatch provably covers codes 0..10 only (`sltiu …, 0xB` at `0x00219C2C`);
codes 11+ never reach a rumble arm.

### Gate struct at `0x0177AC50`

| vaddr / offset | what | verdict |
|---|---|---|
| `0x00217A24`–`0x00217A2C` | `lui 0x178` + `addiu -0x53B0` materializes `0x0177AC50` (also the `a0` gate pointer at every rumble site) | CONFIRMED |
| `+0x0C`, `+0x0D` | per-player enable bytes | CONFIRMED (readers) |
| `+0x0E` | master rumble option mask byte | CONFIRMED |
| `0x00208E84`/`0x00208E8C`/`0x00208E94` | options-screen writer: `sb 4`, `sb 0`, `sb 2` to `+0x0E` (three option states) | CONFIRMED |
| `+0x28` word, bit `0x40` | global suppression (read by three independent sites above) | CONFIRMED |
| UX meaning of master value 2 vs 4 | (two distinct "on" values exist; gate math treats both as masks) | OPEN |
| persistence into the `0x3448` save image | not traced; save-layout.md:74-80 keeps option offsets OPEN | OPEN |
| car `+0x198` u16: bit 0 | player index (used as primitive `a1`) | CONFIRMED |
| car `+0x198` bits 4-11 (0x0FB0) | any set → no rumble (class/interaction bits; per-bit meanings) | CONFIRMED mask, meanings INFERRED |
| car `+0x210` h16 | per-car rumble suppression flag (writer) | CONFIRMED reader, writer OPEN |
| grade unit | relative speed floats where ×216 = display speed (`0x0021D358 GetSpeedFromF15Vector`); full-strength collision ≈ 55 display units | INFERRED |

---

## 2. Native module

`rt::native::gameplay::rumble`, pure C++ (no SDL, no device calls, no guest
memory), deterministic headless tests:

- `gateOpen(master, player)` — CONFIRMED gate arithmetic.
- `collisionGrade(|Δv|) = clamp255(round(|Δv| × 1000))`, zero = silence;
  `landingGrade(raw) = 0` below 179 else `raw >> 4` (cap 255);
  `scrapeGrade = collisionGrade >> 2` (AUTHORED tempering);
  `crashPatternForCode(code)` — CONFIRMED class table, `Stop` outside 0..10.
- `Envelope` constants per pattern record (frames, small-motor presence,
  scaled vs fixed large-motor level) — CONFIRMED decodes above.
- `Channel` per device/port: `setOptions(master, playerByte)`,
  `setSuppressed(bit 0x40 analogue)`, event submissions, one `tick()` per
  60Hz frame returning `Output{left, right, durationFrames}` **only on the
  frame the burst starts** (a set-and-timeout host needs exactly one call),
  `isActive()` while the envelope plays.

Event → motor mapping (low/left = PS2 large motor, high/right = PS2 small motor):

| submission | original site | pattern | left (low) | right (high) | frames |
|---|---|---|---|---|---:|
| `submitCollision(lateral, fwd, |Δv|)` | `0x0021793C` | 6 | `grade/255` | 1.0 (click) | 8 |
| `submitObjectCollision(lateral, fwd)` | `0x00253C9C` | 6 | `64/255` | 1.0 | 8 |
| `submitLanding(per-point raw speeds)` | `0x0021B360` | 1 | `grade/255` (min 11 at the 179 gate) | 1.0 (2f click window) | 8 |
| `submitScrape(side, |Δv|)` | AUTHORED (see below) | 2 | `grade/255` | 1.0 | 4 |
| `submitCrash(code)` | `0x00219C6C`/`0x00219C8C` | 3/4/5 | 1.0 (0xFF) | 1.0 | 3/2/2 |
| `submitTestBuzz()` | `0x00208EA0` | 7 | 127/255 | 0 | 30 |
| `stop()` | pattern 0 / `0xFF` end | 0 | 0 | 0 | 0 |

AUTHORED (native decisions the original cannot answer):

- **Side/lateral balance.** A DualShock has no side motors. Centered contacts
  keep the CONFIRMED levels; a lateral contact normal tilts the split — the
  struck side stays at full level, the opposite motor dips to
  `kSideGainFloor = 0.35`. Scrape sides do the same with `Side::Left/Right`.
- **Scrape event.** The original has no sustained-scrape trigger (walls
  re-fire the 2-frame impulse codes every frame at 0xFF). Native scrapes are
  graded at quarter collision strength over the only scaled short-burst
  record in the table (pattern 2 — present in the table with **no observed
  caller**), held 4 frames and re-fired every frame while grinding: quieter
  than any real impact.
- **Priority shield.** Anything CONFIRMED above accepts the original's
  last-caller-wins replace for equal/higher priority; lower-priority events
  wait for the envelope to end: scrape 0 < landing 1 ≈ test buzz 1 <
  collision 2 < crash 3. Hence "a new landing overrides a scrape while both
  are active", but a scrape never interrupts a collision/crash.
- **Landing aggregation** to the strongest per-point speed (the original
  triggers per wheel with last-in-frame winning).
- **Single-level burst** collapses the per-frame click windows (2 of 8
  frames for pattern 6/1 small motor) into one static level + duration — the
  practical command granularity of `SDL_RumbleGamepad`.

## 3. SDL3 glue (for main.cpp — this module's contract only)

Pick `SDL_RumbleGamepad` (SDL 3.2.0+): it *is* the two-motor level+duration
command the original primitive emits (low-frequency/left =
large motor; high-frequency/right = small motor). Do **not** use the
`SDL_GetHaptics` / `SDL_OpenHapticFromJoystick` / `SDL_RumbleJoystick`
haptic-effect path: it requires authoring platform-specific effect
definitions this model doesn't own, while `SDL_RumbleGamepad` already
falls back internally where plain rumble is unavailable.

```cpp
// src/native/gameplay — glue the app owns (NOT part of the rumble module):
#include "gameplay/rumble.h"
namespace rg = rt::native::gameplay::rumble;

// 1) one channel per open pad/port (2 players max, 0x0020A870 loop):
rg::Channel g_rumble[2];

// 2) boot: read the option from the native settings/save model and wire it.
//    The original gate bytes: master from the options screen encoding
//    {0x00 off, 0x02 mask A, 0x04 mask B}; per-player enable byte.
//    Until the save-image option offset is pinned (save-layout.md OPEN),
//    keep the values in the host settings store.
std::uint8_t master = settings.rumbleMasterByte();     // {0, 2, 4}
g_rumble[0].setOptions(master, settings.rumblePlayerByte(0));
g_rumble[1].setOptions(master, settings.rumblePlayerByte(1));

// RT_APP_RUMBLE env contract: haptics glue is OPT-IN. When the env var is
// absent or not exactly "1", the app must not call any SDL rumble API and
// every channel is force-disabled (master byte 0). Match with std::getenv.
if (std::getenv("RT_APP_RUMBLE") == nullptr ||
    std::string_view(std::getenv("RT_APP_RUMBLE")) != "1") { /* disable */ }

// 3) per 60Hz logic frame (after collision/landing resolution, before draw):
g_rumble[p].setSuppressed(!inGameplay);       // 0x0177AC50+0x28 bit 0x40 analogue
rg::Output out = g_rumble[p].tick();
if (out.active()) {
    SDL_RumbleGamepad(pad,                                      // SDL_Gamepad*
                      Uint16(out.leftIntensity  * 65535.0f),    // low  / left
                      Uint16(out.rightIntensity * 65535.0f),    // high / right
                      Uint32(out.durationSeconds() * 1000.0f)); // auto-timeout
}

// 4) event wiring (values from the sim this frame):
// collision: push normal in car-local space (lateralRight, forward) of the
//   contact normal pointing INTO the car, and |dv| in original float units
//   (displaySpeed / 216.0f converts a display-unit relative speed):
//   g_rumble[p].submitCollision(addr->x, nrm->z, vrel / rg::kVelocityToDisplayScale);
// landing: submit BEFORE tickCar() zeroes CarState::fallSpeed:
//   g_rumble[p].submitLanding(car.fallSpeed, 3);
// scrape: while laterally rubbing a wall with forward speed:
//   g_rumble[p].submitScrape(side, vrel / rg::kVelocityToDisplayScale);
// crash: when ApplyCollisionImpulse-style codes 0..10 resolve:
//   g_rumble[p].submitCrash(code);
// menu: on enabling rumble in options, g_rumble[p].submitTestBuzz();
// leaving gameplay/menus: g_rumble[p].stop(); or SDL_StopGamepadRumble(pad).
```

Note `SDL_RumbleGamepad` cancels any previous effect per call, which matches
the CONFIRMED replace-on-new-event semantics; the glue only calls it again
when a *new* burst wins arbitration (old bursts simply time out).

## 4. Tests and build

```sh
cmake -S src/native -B build/rumble -DRT_NATIVE_ASSETS_BUILD_TESTS=ON
cmake --build build/rumble -j
./build/rumble/native_assets_rumble_test   # "rumble: all checks passed"
```

Coverage: CONFIRMED gate encodings and the off gate, collision/landing/scrape
grade exact values and monotonicity, crash-code class table, envelope totals,
side assignment (left/right/diagonal/head-on), scrape-vs-landing priority
arbitration (both directions), same-priority replacement, stop/suppression
semantics, clamp (≥0x100→0xFF) and deadzone (0-silence, 179 landing gate)
edges, burst-once emission.

## 5. Open / follow-ups

- Save-image offset(s) behind the gate bytes (save-layout.md:74-80 OPEN) — a
  trace of the options-menu writer through `0x0177AC50` to the record.
- UX meaning of master mask values 2 vs 4 (both gate-in when per-player bit
  matches), writer of car `+0x210`, per-bit meaning of flags `0x0FB0`.
- Original caller for pattern 2 (none observed) and purpose of the idle-park
  sweep at `0x0020A7FC`.
- `functions.tsv`/`functions.md` rows naming `0x0020ACB8` and correcting the
  `0x00219660` note's "code 0–15 / rumble [inferred]" tail.
