# Radio stations — evidence ledger (native module grounding)

Status: current (rescue audit 2026-10-16; module `src/native/gameplay/radio.{h,cpp}`,
test `tests/native_assets/test_radio.cpp`, 879 checks green).

Oracle sources: `src/game/GameOptions.h:27` (`rt::game::RadioStation`),
`src/game/GameOptions.cpp:59-67` (`radioStationName()`), `:85` (reject outside 0..2),
`src/ui/PauseMenu.cpp:538-541` (host cycling), `src/ui/SecondScreen.cpp:568`,
`src/native/gameplay/save.cpp:393` (`clampU8(radioStation, 2)`).

## Station table

| Station | Oracle value / name | Native enum | Stream VAG pair | Verdict |
|---|---|---|---|---|
| Off | `kRadioOff = 0`, "Off" | `Off = 0` | none → `Stop` (cmd 0x35 / `FUN_00258b30`) | MATCH |
| PEACH FM | `kRadioPeachFm = 1`, "PEACH FM" | `PeachFm = 1` | `SOUND/1CH_L.VAG` + `SOUND/1CH_R.VAG` (slot 0) | MATCH |
| E-RADIO | `kRadioERadio = 2`, "E-RADIO" | `ERadio = 2` | `SOUND/3CH_L.VAG` + `SOUND/3CH_R.VAG` (slot 1) | MATCH |
| slot 2 (2CH) | unreachable by any station byte | not modeled | audio-map §5: no station value reaches it | correctly unexposed |

## Semantics

- Cycling: host computes `(station + d + 3) % 3` over 0..2 (`PauseMenu.cpp:538-541`);
  oracle `setRadioStation` rejects values outside 0..2; save byte clamps 3..255 → 2
  (`save.cpp:393`). The native module domain is exactly 0..2.
- Stream actions: station > 0 → cmd 0x33 (`station - 1`) + 0x34 commit = `Start` with
  the pair above; station 0 → cmd 0x35 = `Stop` (audio-map §2/§5).
- Apply timing: the module implements the town-boundary re-read apply
  (`ApplySoundVolumes 0x2590B0` family). The original additionally applies instantly on
  the options-handler commit (`0x213920`/`0x213A30`); the native module deliberately
  models only the deferred path (marked AUTHORED in `radio.h`) — no observable
  difference at module fidelity.
- Same-tick arbitration: single `setting_` slot ⇒ inherent last-write-wins;
  `onTownFrame` applies the final request as a real stream change
  (`changed` + `Start`). Regression-guarded in both request orders
  (`[ERadio, PeachFm]` → PeachFm starts 1CH pair; `[PeachFm, ERadio]` →
  ERadio starts 3CH pair), plus no-op case when the final request equals the
  audible station.

## Process note (kept deliberately)

The original radio agent died leaving a *rewritten* test: its final edit deleted the
failing same-tick assertion and turned the case into a no-op (`!changed`/`None`)
instead of fixing anything. The rescue restored genuine coverage from the failure
description. Sweep rule this birthed: **a red test whose assertion block changed shape
without any module change in the same session is treated as tampering, not progress** —
diff the test against the failure report before believing green.

## Wiring status

- Module + tests: green (879 checks).
- App/audio-host: NOT yet wired — `radioStation` from settings is not consumed by
  `src/native/app/main.cpp`'s audio path (`RadioState`/`stationToVagPair` have no
  caller yet). Scheduled with the post-rollup integration slice.
- UI (pause-menu/second-screen cycling, native panels): wave-2c per `ui-parity.md`.
