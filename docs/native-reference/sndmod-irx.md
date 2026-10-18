# SNDMOD.IRX — TSQ driver internals (SLUS-20398)

Extracted 2026-10-15 by importing `SNDMOD.IRX` (IOP R3000 ELF, 110 KB) into
Ghidra via the MCP `import_file` (place the file under `GHIDRA_MCP_FILE_ROOT` =
`~/Library/GhidraMCP/projects` first). `.mdebug` is intact — all original
symbols recovered (`play`, `key_on`, `load_tsqf`, `procDriver`, …).

## Tick rate — PROVEN

`thread_sddr` main loop: `WaitVblankEnd()` → `procRadio/procEngine/procDirect/
procDriver/procFade`. `play()` decrements a per-track countdown per iteration
and only advances the stream when it underflows. At NTSC 60 Hz ⇒ **one tick =
1/60 s** (the provisional `kTickHz = 60` in `tsq_sequencer` is exact).

## Command dispatch in `play` (PROVEN, corrects the corpus grammar)

Loop: read byte; `00..7F` = set countdown to that many ticks. Else:

| Op | Handler | Semantics |
|---|---|---|
| `80..DF` | `key_on(key=b&0x7F)` | pitch = `freqtbl[key]`, optional bend `*c>>12`, start voice on matching channel |
| `E0 vv` | `volume` | 1 op, **not** 2 |
| `E1 ll rr` | `panpot` | 2 ops (left/right byte multipliers, stored +6/+7) |
| `E2 pp` | `tone_chg` | TVB program slot |
| `E3 tt` | `tempo` | 2 ops (consumes u16; master tempo scale) |
| `E4 pp pp` | `pitch` | u16 |
| `E5 pp pp` | `key_pitch` | u16 **absolute freq stored at +0xE**, re-triggers the voice (`key_pitch 0x4A40`); corrected per `audio-runtime.md` re-disassembly — the old "bend multiplier" claim was wrong |
| `E6 m` / `E7 d` | `set_rev_mode` / `set_rev_depth` | 1 op each — **confirmed** (previously "plausible") |
| `E8` / `E9` | reverb-on / reverb-off flag (+0x17) | 0 op — **confirmed** |
| `EA cc cc` | `pitch_cent` | u16 stored as the Q12 **bend multiplier at +0xC** (`pitch_cent 0x48BC`, `sh +0xC`); corrected per `audio-runtime.md` |
| `F0` | `key_off` | |
| `F1 a` | `pri_chg` | 1 op — voice priority byte |
| `F8 dd dd` | `data_jump` | signed 16-bit delta added **after** the two operand bytes (matches our implementation) |
| `F9 cc cc` | `ext_chan` | 2 ops — u16 external-channel allocation |
| `FF` | `data_end` | |

## Volume math (`volume`) — PROVEN

`volL = round(E0 * panL * master) >> 8`, same for R; `panL/panR` default
`0x40` each (center, `E1 40 40`), master lives at +0x18 (SPU-domain, center
≈ `0x4000`). Our provisional gain model `E0/127` with `E1` as balance is
correct up to the global master scalar.

## Pitch table (`freqtbl` @ 0x8A9C, 72 u16 LE) — PROVEN

`freqtbl[key] = 4096 · 2^((key-48)/12)` — **key 48 = 4096 = natural pitch**,
the table halves/doubles every 12 keys (octave) from key 0 (256) up through
key 71 (15464). Example entries: k0=256, k12=512, k24=1024, k36=2048,
**k48=4096**, k60=8192. `key_on` computes
`freq = freqtbl[key] * bendMult >> 12` (`bendMult` from **EA**, 0 = disabled; was mis-attributed to E5).

## File loading

`load_tvbf` / `load_tsqf` read TVB/TSQ via CDVD into `easyHeap`;
`sddrTvbfAdrs`/`sddrTsqfAdrs` return the parsed bases — confirming the
containers documented in `tsq-tvb-format.md` are consumed as-is.
