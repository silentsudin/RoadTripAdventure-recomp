# W2-09 report — audio runtime (voice graph, sequence player, rumble opcodes)

Status: **PASS** on all thresholds, with ELF + `SNDMOD.IRX` + `SOUND/*.TSQ` present. No git commits made.
No ROM bytes committed (tests pin FNV-1a hashes of instruction-word groups, counts, derived numbers).

## Brief vs repo

* `docs/native-reference/audio-sequences.md` **does not exist.** Sequence grounding came from `audio-map.md`,
  `tsq-tvb-format.md`, `sndmod-irx.md`, and a fresh `llvm-objdump` of `SNDMOD.IRX` (`play`, `step_t`,
  `key_on`, `volume`, `panpot`, `ext_chan`, …; `.mdebug` symbols intact).
* `.qoder/wave2-plan-sonnet.md` §2.9 describes a different file split (`audio/audio_runtime`, `sfx_table`,
  `vag_stream`, VAG streaming, `vag_ref.py`). I followed the dispatch instruction (`gameplay/audio_graph.*`,
  per-voice pan, sequence player, rumble) — VAG streaming/seek/RSS and the id→sample table are **not** in this
  batch (see Gaps).

## Files (all new)

| Path | Role |
|---|---|
| `src/native/gameplay/audio_graph.h` / `.cpp` | Pan laws, SNDMOD volume math, `AudioGraph` voice mixer, `SeqPlayer`, BGM/race/SFX/radio helpers, `RumbleInterpreter`, `readElfAudioTables` |
| `src/native/audio/audio_runtime.cpp` | 1-line shim `#include "gameplay/audio_graph.cpp"`: the Step-0 EXISTS-guard already lists this path, so the module links with **no CMake edit**. Delete it when the coordinator registers `gameplay/audio_graph.cpp`, else symbols are defined twice |
| `tests/native_w2/audio_graph_test.cpp` | Acceptance test → target `w2_audio_graph_test` |
| `docs/native-reference/audio-runtime.md` | Design + evidence + corrections |
| `.qoder/wave2/W2-09-report.md` | This file |

Not touched: `main.cpp`, any `CMakeLists.txt`, `engine.*`, `mixer.*`, `rumble.*`, `radio.*`, `third_party/`,
`build/native`, journal/board, other batches' files.

## Result

```
W2-09 SCORE seq_vectors=24/24 rumble_vectors=23/23 pan_vectors=18/18 sndmod_math=20/20 ids=13/13 graph=49/49 alloc_ref=1995/1995 alloc_hash=720fe023f3c23092 elf_pins=15/15 irx_pins=5/5 tsq_files=20 tsq_tracks=622 tsq_ff=99 tsq_unknown=0 tsq_oob=0 tsq_runaway=0 tsq_alive=523 tsq_forks=27 tsq_stalls=0 action_phrase=1 deterministic=1 PASS
```

* NO_ELF mode (`RT_ELF=/nonexistent`): prints `NO_ELF`, runs all pure goldens, `SCORE … elf_pins=NA irx_pins=NA tsq=NA … PASS`, exit 0.
* Negative control: an all-zero 1.28 MB "ELF" → `elf_pins=0/8 … FAIL`, exit 1.
* Determinism: whole pure suite + allocation run executed twice, hashes equal; two full runs byte-identical output (md5).
* AddressSanitizer + UBSan run (`-O1 -fsanitize=address,undefined`): clean.
* `-Wall -Wextra`: no warnings from the module or test (direct compile and the cmake build; warnings printed
  during the cmake build belong to `hud_art.cpp`, `guest_facade.h`, `enhancements.cpp` — other batches).

## Grounding table (every pin re-read this pass)

ELF `SLUS_203.98` (`vaddr − 0x1FF000`); raw words are little-endian instruction/data words.

| Claim | vaddr | Raw words | Test check |
|---|---|---|---|
| `gs_music_play_bgm`: cmd 0xF, cmd 0xE(id), cmd 7(PP, `&table[(id>>16)&0xFF]`), stride 12 | `0x258EE4 0x258EF8 0x258F00 0x258F04 0x258F14 0x258F1C 0x258F20` | `0c095ff6 0c095fcc 00112840 00b12821 00052880 3c01002a 2421adf0` | FNV `0x24b204d8cbc5a7ce` |
| `PlayUiSound`: `id \| 0x40400000` → `0x258308` | `0x258E54 0x258E5C 0x258E64` | `3c024040 00822025 080960c2` | FNV `0xf218335f023f3dbf` |
| Race BGM chooser (`==0x1A`, `slti 0x12`, `==0x1C`, table `0x29F5B0`, fallbacks) | `0x22DE70 0x22DE88 0x22DE90 0x22DE94 0x22DE98 0x22DEA0 0x22DEB8 0x22DEC0 0x22DEC4 0x22DED8` | `2402001a 28620012 2402001c 00031080 3c04002a 8c84f5b0 54620007 3c040a16 0c0963b2 0c0963b2` | FNV `0x49257b5b519d971c` |
| `rumbleTrigger`: gate, `slti 0x100`/`movz 0xFF`, table `0x29AD60`, stores | `0x20ACBC 0x20ACC4 0x20ACC8 0x20ACCC 0x20ACD4 0x20ACDC 0x20ACE0 0x20ACEC 0x20ACF4 0x20ACFC 0x20AD04 0x20AD0C` | `9085000e 9062000c 00451024 10400010 10e0000e 28e40100 240300ff 8c42ad60 0064380a a020aba8 a027abb0 ac228868` | FNV `0x55d904c65f684a86` |
| Rumble interpreter dispatch (`0x80/0x81/0x82/0x84/0xFF`, hold dec) | `0x20A6B0 0x20A6B8 0x20A6D4 0x20A6D8 0x20A6F4 0x20A6F8 0x20A6FC 0x20A710 0x20A718 0x20A724 0x20A72C 0x20A740 0x20A750 0x20A780 0x20A788 0x20A7B4 0x20A7C8` | `90820000 244200ff 24120082 24110080 24100081 24190084 241800ff 1072001b 28620083 5071000e 50700010 1079001b 5078001d 90a30000 34630002 91830000 a1000000` | FNV `0xa3ebd878009b72dd` |
| 8 rumble records | `0x2EA3D0 + 8n` via ptr table `0x29AD60` | `000000ff 00000000 / 81028480 0000ff06 / ff048480 00000000 / 82810180 00ff02ff / 8001ff82 0000ff01 / 02ff8280 000000ff / 02800484 00ff0281 / ff1e7f82 00000000` | authored scripts assemble to ELF bytes 8/8; ptr n = `0x2EA3D0+8n` 8/8 |
| `gs_music_file_table` 29 × {`0x00333388`, LBN, sectors} | `0x29ADF0..0x29AF4C` | (87 words) | FNV `0x78cabc97449c3dc0`, field0 = const 29/29 |
| Race BGM table 18 × u32 | `0x29F5B0..0x29F5F4` | `0a0c0201 0a0c0201 0a130201 0a130201 0a100201 0a100201 0a120201 0a140201 0a110201 0a110201 0a110201 0a120201 0a140201 0a0f0201 0a0f0201 0a150201 0a150201 0a0e0201` | FNV `0x88268b4a7485172e`; equals authored mirror 18/18 |
| `jal 0x258EC8` / `jal 0x258E50` site counts | whole `.text` scan | `0x0c0963b2` / `0x0c096394` | 35 / 256 |

`SNDMOD.IRX` (IOP ELF, LOAD seg: file offset = vaddr + 0xA0):

| Claim | IRX vaddr | Raw words | Test check |
|---|---|---|---|
| `play`: `cnt = (s16)(cnt−1)`, read stream only when `== −1` | `0x52C4 0x52CC 0x52D8 0x52DC` | `2483ffff a4440014 2403ffff 10430012` | FNV `0xf020cef02ca195aa` (with the next row) |
| `step_t`: stores `D − 1` | `0x4810 0x4814` | `2464ffff a4440014` | same hash |
| Dispatch compares `E0..EA, F0, F1, F8, F9` (`addiu $3,$zero,imm`), everything else → `data_end` | `0x53D8 0x5400 0x5428 0x544C 0x5470 0x5498 0x54C0 0x5500 0x5540 0x5564 0x5588 0x55AC 0x55D4 0x55FC 0x5620` | `240300xx` | immediate set == expected 15 values |
| `volume`/`panpot`: `(vol·pan)·master`, `+0xFF`, `sra 8`; mono fold `(L+R)/2` | `0x4FD8 0x4FF4 0x5004 0x5008 0x500C 0x505C 0x5060 0x5064 0x509C 0x50A8 0x50AC 0x50B0` | `00650018 00c40018 246300ff 00031a03 a4430008 246300ff 00031a03 a443000a 00651821 00652021 00041843 a4430008` | FNV `0xf6a4d0055f66d2d2` |
| `key_on`: `freq = freqtbl[key]`, `×bend >> 12` | `0x4BE4 0x4BEC 0x4C28 0x4C38 0x4C3C 0x4C40` | `94848a9c a444000e 00650018 24630fff 00031b03 a443000e` | FNV `0x32cdc2cb9606a94d` |
| `freqtbl` @`0x8A9C` | 72 × u16 | k0=256 … k48=4096 … k71=15464 | = `floor(4096·2^((k−48)/12))` 72/72 |
| `ext_chan` fork rule, `pitch_cent`=+0xC bend, `key_pitch`=+0xE freq+retrigger, `tempo` discards 2 bytes | `0x45D4`, `0x48BC`, `0x4A40`, `0x5110` | (disassembly) | behavioural vectors + ACTION.TSQ `0x194` phrase (disc) |

## New findings / corrections (feed back into docs)

1. **Delta timing is exact:** delta `D` = D ticks; `D = 00` stalls 65535 ticks (s16 wrap). Unknown opcodes ≥ `0xE0` stop the track.
2. **`EA` = Q12 bend multiplier, `E5` = absolute freq + retrigger, `E4` = freq without retrigger** — `sndmod-irx.md`/`tsq-tvb-format.md` call `E5` the bend multiplier; the IRX stores it at +0xE, `EA` at +0xC.
3. **`F9` = fork** at `pc(after operands) + s16` (ACTION.TSQ `0x194`: the mirrored phrase at `0x1A4`).
4. **Volume math** is exact (`vol·pan·master >> 8`, mono fold) — master scale itself not traced.
5. **BGM TSQ second table** `{0x0080, rel}` at `T=0xD0`; targets `T + rel`. 622 tracks decode with 0 unknown/oob/stall.
6. **`audio-map.md` §2 id layout is mislabelled:** ROOM_1's varying byte is the low byte (cmd 0xE), PP is 1/2 (bank slot, INFERRED).
7. `audio-map.md` UI-id list is non-exhaustive (heuristic scan finds `0x14`, `0x305`, `0x8035`, … and misses `0x01/02/03/0x65/0x136/0x1A4`; 16 of 256 sites not immediate-resolvable).

## Reproduction

```sh
cd /Users/holzr/github/RoadTripAdventure-decomp
cmake -S src/native -B build/w-W2-09 -DRT_NATIVE_ASSETS_BUILD_TESTS=ON -DCMAKE_BUILD_TYPE=Release
cmake --build build/w-W2-09 --target w2_audio_graph_test
RT_ELF="$HOME/Library/Application Support/RoadTripRecomp/disc/SLUS_203.98" build/w-W2-09/w2_audio_graph_test; echo $?
```

Notes: the suggested `cmake -B build/w-W2-09 -S .` configures the top-level app project, which has no `w2_*` targets
and ignores `RT_STAGED_SOURCES` (unused-variable warning); the `-S src/native` form is what the plan §1 specifies and
what the other Wave-2 build dirs use. Target name is `w2_audio_graph_test`. Without the shim,
`gameplay/audio_graph.cpp` is not in the staged-source list and the test fails to link; direct fallback:
`c++ -std=c++20 -O2 -Wall -Wextra -I src/native src/native/gameplay/audio_graph.cpp src/native/assets/music_seq.cpp src/native/audio/vag.cpp tests/native_w2/audio_graph_test.cpp -o /tmp/w2_audio_graph_test`.
`RT_ELF` (if set) overrides `--elf`; the disc directory (for `SNDMOD.IRX`, `SOUND/*.TSQ`) is the ELF's parent.

## Coordinator wiring API (nothing edited in main.cpp)

* Rumble: `RumbleInterpreter r; r.setGate(master, p0, p1); r.trigger(player, effect, strength); r.tick(); r.motors(p)` → `rumbleToSdl`.
* Audio: `AudioGraph g(outRate); g.playBgm(slot, id, src, params); g.playRadioPair(L, R, vol, 12000); g.playSe(id, src, params); g.mix(int16* stereo, frames)`; sequencer: `SeqPlayer sp(tsqBytes); sp.start(trackOffset); sp.tick(events); applySeqEvents(g, sampleProvider, events)` once per 1/60 s.
* Tables: `readElfAudioTables(elfBytes, out)` gives the file-table rows, race table and ELF rumble records.

## Gaps / not done

* **No VAG streaming/seek/RSS work, no `vag_ref.py`, no `sfx_table`** (plan §2.9 items 1/2): SFX id→(bank,track) is still driver-internal (`audio-map.md` §7 OPEN); the graph takes a caller-provided `PcmSource`.
* TVB sample provider, ADSR/reverb curves, and the real SNDMOD master (+0x18) scale are not modelled (gain = `vol/16383` with master `0x100` is AUTHORED).
* Fork ordering inside a tick, default track state before the first `E0`, and the SE steal policy are AUTHORED/OPEN.
* The allocation hash `720fe023f3c23092` is a self-golden (xorshift32 seed `0xC0FFEE`), verified against an independent reference model, not against the original driver.
* Rumble: master-value meaning (2 vs 4), `+0x210` writer, pattern-2 caller remain OPEN as in `rumble-haptics.md`; the idle-park sweep at `0x20A7FC` is intentionally not modelled.
* Coordinator must delete `src/native/audio/audio_runtime.cpp` if it registers `gameplay/audio_graph.cpp` directly.
