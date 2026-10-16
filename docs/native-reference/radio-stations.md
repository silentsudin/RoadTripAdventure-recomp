# Radio stations — module map and evidence ledger

Date: 2026-10-16. Owner: `src/native/gameplay/radio.{h,cpp}` (pure C++, no SDL, no
audio output). This is the retention bridge for the audit row *"Radio (station
selection and VAG streams) … the save model parses/stores `radioStation` but
nothing consumes it; the host plays one fixed VAG/TSQ track"* into a consumable
module: enum + save-byte validation, the authentic stereo VAG pair per station,
and the town-frame lifecycle the audio host must consult.

Related: `audio-map.md` SS2/SS3/SS5 (radio verdict), `save-layout.md`
(`0x12DD` field + migration rule), `functions/sound-rpc.md` (cmd wrappers),
`src/native/gameplay/save.h` (the `radioStation` field this module round-trips).

---

## 1. Evidence ledger

Everything below was re-derived for this module. "CONFIRMED" = read directly
from the ELF `SLUS_203.98` this session (capstone, `fileOffset = vaddr -
0x1FF000`, the verified conversion from `save-layout.md`); "PROVEN" =
established in the citing doc; "INFERRED" = mechanism proven, human-facing
binding still a reading of semantics; "AUTHORED" = native host policy, not of
the original disc.

| # | claim | status | evidence |
|---|---|---|---|
| 1 | Station byte = `progress+0x12DD` == EE `0x01780A3D` | CONFIRMED | `ApplySoundVolumes 0x2590B0`: `lui $s0,0x178; addiu $s0,$s0,-0x8a0` (=0x0177F760), `lb $v0,0x12dd($s0)` @0x2590F8; options handler writes `sb $v1,0xa3d($v0)` with `lui $v0,0x178` @0x21390C. Also PROVEN in `save-layout.md` ("radio: 0 off, 1 Peach FM, 2 E-Radio"). |
| 2 | `ApplySoundVolumes 0x2590B0` = the radio apply: master volume, then station>0 → **cmd 0x33** with **`station-1`**, then **cmd 0x34** commit | CONFIRMED | `lh $v1,0x12de($s0)` (amp) → `… sra $a0,7; jal 0x2589a0` (cmd 0x31, buffer 0x175E840) @0x2590CC-0x2590F0; `lb $v0,0x12dd($s0)`; `blez $v0,0x259120`; delay slot `addiu $a0,$v0,-1` @0x259100 (station−1 … wait, this loads a0 with station-1); `jal 0x258a48` (cmd 0x33: `addiu $a1,$zero,0x33` @0x258A5C) @0x259104; tail `j 0x258af0` (cmd 0x34: `addiu $a1,$zero,0x34` @0x258AF8) @0x259118. Matches `audio-map.md:249-253` and `functions/sound-rpc.md` rows 40/41/59. |
| 3 | Station 0 inside `ApplySoundVolumes` **returns; it does not send cmd 0x35** | CONFIRMED (**doc downgrade**) | The `blez`-taken branch @0x259120-0x259130 restores `$s0/$s1/$ra` and `jr $ra` — no call. `audio-map.md:251-253` ("if the station is 0 it calls FUN_00258b30 (stop)") is wrong about *where*: the stop for Off is the options commit's `jal 0x258b30` (cmd 0x35) @0x213A30, and the four town-task cmd-0x35 sites are dialogue/teardown paths (each preceded by `jal 0x258ff0` SoundStopAllChannels: @0x210C64/0x210E1C/0x210F88/0x211128). |
| 4 | The radio only becomes audible through town code: `ApplySoundVolumes` callers are `StartTownModeTask 0x210430`, `UpdateTownExplorationTask 0x2105B8`, and the options/radio handler `FUN_002137B0 0x2137B0` | CONFIRMED | `jal 0x2590B0` sites (ELF scan, 4 hits): **0x210488** (in `StartTownModeTask`, delay slot `addiu $a0,$zero,2` = the cmd-0x33 volume/voice param), **0x2106C0** and **0x2107C8** (in `UpdateTownExplorationTask`), **0x213860** (in the options handler). Matches `audio-map.md:252-253` ("reached from UpdateTownExplorationTask … the radio only runs in town") and `functions/sound-rpc.md` row 59 callers. |
| 5 | Options/radio menu commits the station byte and applies it in the same handler | CONFIRMED | `FUN_002137B0 0x2137B0`: cursor move plays SE `0x1a` (next) / `0x1b` (prev) via `jal 0x258e50` @0x2138F0/0x213940; on a detected station change it writes `sb $v1,0xa3d` @0x21390C then, if the byte is 0 → `jal 0x258b30` (cmd 0x35 stop) @0x213A30, else `jal 0x258a48` with delay slot `addiu $a0,$a0,-1` @0x213924 followed by `jal 0x258af0` @0x213928. Cursor-volume path bounds the amp halfword `0x01780A3E` to `0..0x80` (`sh $v1,0xa3e` @0x213990) and re-scales it (`(amp<<14−amp)>>7` ≈ amp·128) into cmd 0x31 @0x2139DC. |
| 6 | Boot registers exactly three stereo VAG pairs with cmd 0x32, driver slots `{0,2,1}` in code order | CONFIRMED | `InitSoundRpcDriver 0x256AF8` → `SoundRpcRegisterFiles 0x2575E0` calls @**0x257004** (slot a0=0; records `$s2+0x00`/`$s2+0x0C`), **0x257014** (a0=2; `$s2+0x18`/`$s2+0x24`), **0x257024** (a0=1; `$s2+0x30`/`$s2+0x3C`), where `lui $s2,0x2a; addiu $s2,$s2,-0x51e0` = **0x29AE20 = `gs_music_file_table` row 4** (table at 0x29ADF0, 12-byte records). Matches (and refines) `audio-map.md:84-93` (`FUN_002575e0`, "slot*2 and slot*2+1", "Code order in the ELF is slot 0, then 2, then 1"). |
| 7 | The three registered pairs are `1CH_L/R` (rows 4/5), `2CH_L/R` (rows 6/7), `3CH_L/R` (rows 8/9) | CONFIRMED | Rows read from the ELF: row 4 startLBN **228654**, row 5 **192175**, row 6 **240904**, row 7 **179788**, row 8 **204239**, row 9 **216475** — all six resolve through `disc/.lbn_map.tsv` to `SOUND/1CH_L.VAG`, `1CH_R`, `2CH_L`, `2CH_R`, `3CH_L`, `3CH_R` with matching byte sizes. So: slot 0 = 1CH pair, slot 2 = 2CH pair, slot 1 = 3CH pair (PROVEN in `audio-map.md` SS3, CONFIRMED here against the map file). |
| 8 | Station names "PEACH FM" / "E-RADIO", byte values 0/1/2 | PROVEN | `save-layout.md` field map + ledger; `src/game/GameOptions.cpp` `radioStationName` (kRadioOff/kRadioPeachFm/kRadioERadio); `audio-map.md:328`. |
| 9 | **Station 1 (PEACH FM) → registered slot 0 = `1CH` pair; station 2 (E-RADIO) → slot 1 = `3CH` pair** | INFERRED | Chain: byte→cmd-0x33 index `station−1` is CONFIRMED (#2); the pairs' slot order is CONFIRMED (#6/#7); but the index→slot consumption is driver-internal (SNDMOD) and not visible in the ELF. `audio-map.md:257-262` derives the same reading and §8 item 5 names it the one open item ("which registered VAG slot the cmd-0x33 index plays"). NOT upgraded by this pass; module labels the binding INFERRED on the doc's authority. |
| 10 | Slot 2 (`2CH` pair) is unreachable by a station byte ("unused/hidden on this disc") | INFERRED | The station byte is documented 0..2 (#8); cmd 0x33 carries `station−1` ∈ {0,1}; no path in `ApplySoundVolumes`/options code hands a larger index. `audio-map.md:261-262`. The module exposes no API that can select 2CH, and the tests assert no station maps to it. |
| 11 | Station→pair binding cannot be AUTHORED here | AUTHORED-free | Every pair the module returns is grounded as above; nothing is invented. The only invented pieces are below. |
| 12 | Module lifecycle: a station request (save byte / options choice) applies at the next **town-frame boundary**; Off stops the stream | AUTHORED (policy) | Consolidates #2/#4/#5: the byte may be written at any time (the original writes it at options commit), but during gameplay it only becomes audible where the town task runs `ApplySoundVolumes`; the original's in-menu commit also applies immediately inside the options handler (#5). This module folds both into one deterministic rule (below) so the host needs a single consult point. |
| 13 | Out-of-range bytes clamp to 2 | AUTHORED (matches save model) | `save.cpp` `migrateToNative`: `clampU8(radioStation, 2, ...)` and `save-layout.md` migration step 4 ("radio 0x12DD ∈ 0..2"). The module mirrors it byte-for-byte so `stationFromSaveByte` and the save model agree (test-pinned). The original writer only produces 0..2; the apply path itself tests the byte signed (`blez` #2), which this module does not rely on. |
| 14 | Pair path form `SOUND/<name>.VAG`, disc-root-relative, joined to the host disc root | AUTHORED | Matches the `disc/.lbn_map.tsv` relative names and the existing host convention (`initAudio` resolves `discRoot` from `RT_DISC`/HOME and joins `"SOUND"` for TSQ load). |

No strings "1CH_L.VAG" etc. exist in the ELF (verified by scan, consistent with
`audio-map.md` SS1: "no ASCII asset names inside the ELF"); identity is by LBN
resolution only — which is why the tests pin *names copied from the doc table*,
not disc reads.

---

## 2. Grounded station → VAG pair table

| byte | station | driver slot (cmd 0x32) | `gs_music_file_table` rows (LBN) | stream pair (disc-root-relative) | grounding |
|---:|---|---:|---|---|---|
| 0 | Off | — (none) | — | none — stops any stream request | #1, #3, #5, #8 |
| 1 | PEACH FM | slot 0 | rows 4/5 (228654 / 192175) | `SOUND/1CH_L.VAG` + `SOUND/1CH_R.VAG` | pair CONFIRMED (#6, #7); binding **INFERRED** (#9) |
| 2 | E-RADIO | slot 1 | rows 8/9 (204239 / 216475) | `SOUND/3CH_L.VAG` + `SOUND/3CH_R.VAG` | pair CONFIRMED (#6, #7); binding **INFERRED** (#9) |
| — | hidden/unused | slot 2 | rows 6/7 (240904 / 179788) | `SOUND/2CH_L.VAG` + `SOUND/2CH_R.VAG` | registered at boot, **no station byte reaches it** (#10); NOT exposed by the module |

Each side is a `VAGp` v0x20 stream, 12 kHz, ≈60 minutes (`audio-map.md` SS1) —
the radio *streams* are the stereo pairs, not TSQ sequences. Nothing is
AUTHORED in this table: file identity is LBN-resolved (PROVEN) and the boot slot
order is CONFIRMED; only the byte→slot binding stays INFERRED per #9.

---

## 3. Module API (`src/native/gameplay/radio.h`)

```cpp
enum class RadioStation : std::uint8_t { Off = 0, PeachFm = 1, ERadio = 2 };

struct RadioVagPair { std::filesystem::path left, right; };   // SOUND/xCH_L,VAG / _R.VAG

bool          isValidRadioByte(std::uint8_t raw);            // 0..2
RadioStation  stationFromSaveByte(std::uint8_t raw, bool* clamped = nullptr);  // clamps like save.cpp
std::uint8_t  stationToSaveByte(RadioStation);               // enum -> SaveData::radioStation
std::optional<RadioVagPair> stationToVagPair(RadioStation);  // nullopt for Off; never 2CH

enum class RadioStreamAction { None, Start, Stop };
struct RadioApply { RadioStation station; RadioStreamAction action; bool changed;
                    std::optional<RadioVagPair> pair; };     // pair set for Start

class RadioState {
public:
    explicit RadioState(std::uint8_t saveByte = 0);          // progress+0x12DD / SaveData::radioStation
    void requestStation(RadioStation);                       // deferred: pending until town frame
    void requestSaveByte(std::uint8_t);                      // clamped options-UI commit path
    RadioApply onTownFrame();                                // ONE call per town-frame boundary
    RadioStation currentStation() const;                     // effective (audible) station
    std::optional<RadioStation> pendingStation() const;      // still-to-apply request, if any
    std::optional<RadioVagPair> pathsForCurrent() const;      // null == Off == no stream
    std::uint8_t saveByte() const;                           // round-trips SaveData::radioStation
};
```

Round-trip with the save model: `SaveData::radioStation` (u8) →
`RadioState{save.radioStation}` → play → `save.radioStation = radio.saveByte()`
→ `serializeSave` → `parseSave` — byte-identical for 0/1/2, and identical after
`migrateToNative` normalization for any raw byte (clamp-to-2 mirrors
`save.cpp:393`). Pinned in `tests/native_assets/test_radio.cpp`.

---

## 4. Lifecycle (AUTHORED, per ledger #12)

State per station per track: `applied` (effective/audible), `setting` (the
persisted byte), `pending` (a request not yet applied).

1. **Load.** `RadioState(save.radioStation)`: the *setting* is the save byte;
   nothing is audible yet. A save byte of 1/2 becomes pending; 0 stays quiet.
   Ground: the byte may be set anywhere (options save load, migration), but the
   stream only starts where town code applies it (#2/#4).
2. **Request.** `requestStation`/`requestSaveByte` set the setting byte — like
   the original menu commit writing `0x01780A3D` at request time (#5) — and mark
   it pending. The audible stream does not change yet: `pathsForCurrent()` keeps
   the old pair until the boundary ("takes effect at the next town-frame").
3. **Apply.** `onTownFrame()` applies one pending request and returns the
   `RadioApply` action: change to a station → `Start` with that station's pair
   (restart semantics: replaces whatever the radio was playing); change to Off
   → `Stop` (#5 cmd-0x35 semantics: Off stops the stream request); same station
   or no pending → `None`. Call this once per town frame — i.e. wherever the
   host's town-state update runs, mirroring `StartTownModeTask` → town update →
   `ApplySoundVolumes` (#4).
4. **Off.** `Off` never has a pair, and any pending Off stops the radio stream
   at the boundary. When Off is already applied, further frames are `None`.

Determinism: no clocks, no randomness, no audio output; the whole machine is
plain state transition (test-pinned across the exhaustive byte range).

---

## 5. Audio-host glue (rt-app initAudio + town frame)

`main.cpp`/`preview.cpp` are intentionally NOT edited by the radio change; this
is the contract their owner should follow. Host facts referenced:
`AudioHost{ engine, tsqBgm, usingTsqBgm, stream, … }`, `initAudio(AudioHost&)`
(opens 48 kHz stereo, loads `BGM_01` TSQ or `RT_APP_BGM` VAG,
`discRoot` from `RT_DISC`/HOME), `fillAudio` callback (calls
`engine.tick` or `tsqBgm.pump`), the fixed 60 Hz logic-step frame loop,
`bootstrapSettings` (`RT_APP_*` env-first pattern: environment overrides,
settings only fill absent keys).

```cpp
// -- AudioHost (main.cpp) gains -------
gameplay::RadioState radio{0};               // load: session save byte or RT_APP_RADIO below
bool                 radioFollow = false;     // station-follow enabled by RT_APP_RADIO
gameplay::RadioVagPair radioPlaying{};       // pair the engine currently holds ("" = none)
std::vector<audio::PcmBuffer> radioPcm;      // decoded L + R, host-owned

// -- initAudio -------
const char* env = std::getenv("RT_APP_RADIO");
std::uint8_t byte = hostSave.data.radioStation;              // SaveData::radioStation (u8)
if (env && *env) {                                           // override wins, RT_APP_* style
    byte = gameplay::stationToSaveByte(
        gameplay::stationFromSaveByte(static_cast<std::uint8_t>(std::atoi(env))));
    radioFollow = true;                                       // any explicit value enables follow
}
audio.radio = gameplay::RadioState{byte};
if (radioFollow && audio.radio.pendingStation() &&
    *audio.radio.pendingStation() != gameplay::RadioStation::Off) {
    // Radio owns the BGM path: skip the fixed BGM_01 TSQ / RT_APP_BGM VAG load
    // (leave the SDL device opening exactly as-is). Otherwise: keep today's
    // behavior untouched (RT_APP_RADIO unset or 0).
}

// -- once per completed town frame (the town-state step of the 60 Hz loop) --
gameplay::RadioApply a = audio.radio.onTownFrame();           // ONE call per town frame
if (radioFollow && a.action == gameplay::RadioStreamAction::Start) startRadio(audio, *a.pair);
if (radioFollow && a.action == gameplay::RadioStreamAction::Stop) stopRadio(audio);

// -- startRadio (main thread; restart-on-change = pathsForCurrent() differs) --
void startRadio(AudioHost& audio, const gameplay::RadioVagPair& pair) {
    const std::filesystem::path discRoot = /* same resolution as initAudio */;
    audio::PcmBuffer left, right;                            // ~24.7 MB each decoded
    if (!audio::loadVagFile(discRoot / pair.left,  left) ||
        !audio::loadVagFile(discRoot / pair.right, right)) { /* log + keep old stream */ return; }
    SDL_LockAudioStream(audio.stream);                       // the callback ticks concurrently
    audio.engine.stopBgm();                                  // mirrors cmd 0x35 (options Off, 0x213A30)
    audio.radioPcm = {std::move(left), std::move(right)};     // host owns the PCM lifetime
    audio.engine.playBgm(0, kRadioLId, {audio.radioPcm[0].samples.data(),
        audio.radioPcm[0].frames, /*loop=*/true}, /*volume=*/.7f);   // L half
    audio.engine.playBgm(1, kRadioRId, {audio.radioPcm[1].samples.data(),
        audio.radioPcm[1].frames, true}, .7f);                        // R half
    audio.radioPlaying = pair;                               // pathsForCurrent()'s twin
    SDL_UnlockAudioStream(audio.stream);
}

void stopRadio(AudioHost& audio) {
    SDL_LockAudioStream(audio.stream);
    audio.engine.stopBgm(); audio.radioPlaying = {}; std::vector<audio::PcmBuffer>().swap(audio.radioPcm);
    SDL_UnlockAudioStream(audio.stream);
}
```

Notes and honesty labels:

- **Stereo (AUTHORED mapping).** The engine's voices are mono with a pan field;
  the natural mapping is left file → one BGM voice pan −1, right file → a
  second voice pan +1 (two `playBgm` slots as above; add the pan wiring if the
  engine exposes it). The original's L/R mixing is driver-internal (SNDMOD) and
  cannot be confirmed from this disc's ELF. If per-voice pan stays unavailable,
  interleaving L/R frames into one stereo buffer is the fallback; either way
  the *pair* is the authentic artifact and the module maps it identically.
- **Volume (host policy).** The original's radio audible volume is a runtime
  halfword at `0x01780A3E` (`0..0x80`, scaled ≈`amp<<7` via cmd 0x31 →
  `engine.setMasterVolume`), not a modeled save field — the radio module is
  station-only and leaves gain to the host, as the task scopes.
- **Restart condition.** "Restart the SoundEngine when it changes" means:
  compare `pathsForCurrent()` against the pair the engine holds
  (`radioPlaying`); the delta arrives via `onTownFrame()`'s `Start`/`Stop`
  anyway. The doc-anchored save-load path: the session save byte flows in at
  `RadioState{byte}`; `RT_APP_BGM` keeps its exact current meaning whenever the
  radio is Off/unset.

### Environment flag contract (`RT_APP_RADIO`)

Following the existing `RT_APP_*` precedence (environment overrides execution;
`bootstrapSettings` only fills absent keys, e.g. a `sound.radio` settings key
would `setDefaultEnv("RT_APP_RADIO", …)` — settings wiring belongs to the
options-menu/settings work, not this module):

| value | meaning |
|---|---|
| unset | exactly today's behavior: fixed `BGM_01` TSQ or `RT_APP_BGM` VAG; the radio state may still run (available to UI/`radioCommand` hooks), but the host's audio path never consults it. (Retention-safe default; no behavioral change unless opted in.) |
| `0` | station Off — explicit override: the radio stream never starts even if the save byte says otherwise; audio follows the `unset` path. |
| `1` | **PEACH FM — enables station-follow**: `initAudio` skips the fixed track, the next town-frame boundary starts the `1CH` pair (`SOUND/1CH_L.VAG` + `1CH_R.VAG`), and every later station change (options commit writing `radio.saveByte()`, save reload) restarts the stream at the next town boundary. |
| `2` | **E-RADIO — station-follow** with the `3CH` pair. |

Out-of-range values clamp to the documented enum via `stationFromSaveByte`
(same policy as the save model). `RT_APP_RADIO` set to 1/2 takes precedence over
`RT_APP_BGM` (the radio *is* the BGM in town, per the disc's own design — the
radio streams are the streaming VAGs); with `0`/unset, `RT_APP_BGM` semantics
are untouched.

---

## 6. Tests (`tests/native_assets/test_radio.cpp`)

- byte validation + clamp over the exhaustive 0..255 range, agreeing with
  `migrateToNative` normalization through a live `parseSave`/`migrateToNative`
  round;
- deferred apply: pending visible immediately, audible change only at the town
  tick, last request wins, same-station request is a no-op;
- station→pair table hardcoded from `audio-map.md` SS2/SS5 (+ "no 2CH" negative
  assertion, disc-root-relative path shape);
- Off semantics: Off-at-load starts nothing, Off stops at the boundary, Off
  re-request is a no-op;
- save round-trip through `serializeSave`/`parseSave` both directions,
  including the in-session station change reaching the next-save byte.

Registered as `native_assets_radio_test` in `src/native/CMakeLists.txt`
(linked to `rt_native_assets`, the standard test pattern; run by hand —
no CTest, per the tree's convention).
