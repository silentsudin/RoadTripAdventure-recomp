# Audio Inventory – RoadTripAdventure (PS2)

## 1. Full asset table (ROM)

| Path (relative to ROM) | Size (bytes) | Detected magic (first 4 bytes) |
|------------------------|--------------|--------------------------------|
| SOUND/3CH_L.VAG | 24 891 520 | `VAGp` |
| SOUND/2CH_L.VAG | 24 724 272 | `VAGp` |
| SOUND/1CH_L.VAG | 24 706 368 | `VAGp` |
| SOUND/1CH_R.VAG | 24 706 368 | `VAGp` |
| SOUND/3CH_R.VAG | 24 891 520 | `VAGp` |
| SOUND/2CH_R.VAG | 24 724 272 | `VAGp` |

**Total assets:** 6 files (all `.VAG`).
**Combined size:** **148 644 320 bytes**.

*No files with extensions `.VS`, `.ADS`, `.SEQ`, `.STR`, `.VGM`, `.WAV`, `.AT3` were found anywhere in the disc image.*

## 2. Format breakdown

| Extension | Count | Description |
|-----------|-------|-------------|
| `.VAG` | 6 | Sony ADPCM (“VAG”) streams. 4‑bit ADPCM, mono per file. The game stores left/right channels as separate files (`*_L.VAG`, `*_R.VAG`). Each file contains a 48‑byte header (`VAGp`) followed by raw ADPCM blocks.
| others | 0 | – |

## 3. PS2 SPU implications

- **Encoding** – VAG is the native ADPCM format the PS2 SPU expects. The decoder in `src/native/audio/vag.cpp` implements the exact block‑wise 4‑bit ADPCM algorithm used by the SPU, including predictor filters and shift values.
- **Sample rate** – The VAG header stores the sample rate at offset 0x10 (big‑endian). The six streams all contain the byte sequence `e0 2e 00 00` (little‑endian view). Interpreted as big‑endian this yields **12 000 Hz**, the standard rate for Road Trip’s background music.
- **Looping** – The final block flag byte is `0x07` (end‑of‑stream). `loadVagToPcm` forces `out.loop = true` for all streams, matching the in‑game behaviour where BGM repeats indefinitely.
- **Stereo handling** – The SPU receives mono PCM; the engine pairs left/right files to produce a stereo output. The naming convention (`1CH`, `2CH`, `3CH`) reflects the channel count of the original source before it was split.

### Sample header decode (first VAG file)
```
Offset 0x00‑0x03 : "VAGp"            // magic
Offset 0x04‑0x07 : 0x00 0x00 0x00 0x00 // version / reserved
Offset 0x10‑0x13 : 0xe0 0x2e 0x00 0x00 // big‑endian 0x00002EE0 = 12000 Hz
…
```

## 4. Native audio implementation overview

| Module / file | Responsibility |
|---------------|----------------|
| `bgm_player.{h,cpp}` | High‑level BGM loader – resolves TVB (metadata) and TSQ (sequence) files, renders full PCM via `assets::seqRenderMusic`, stores PCM in `BgmPlayer::pcm_` and streams it.
| `engine.{h,cpp}` | Core sound engine – voice pool, BGM slot management, SE playback, volume / fade handling, per‑frame tick that drives the mixer.
| `mixer.{h,cpp}` | Mixes active PCM channels into the final interleaved buffer, supports master/voice volume, pan, mono flag, and master‑mono override.
| `vag_loader.{h,cpp}` | Loads a `.VAG` file from disk, parses the header, validates block headers, and calls the decoder.
| `vag.{h,cpp}` | Implements the low‑level ADPCM decoder (`decodeVagBlock`, `decodeVagStream`, `decodeVagBlocks`) and header parser.
| `music_library.{h,cpp}` | Maintains a map from file‑index → VAG path, resolves the path, loads the VAG, caches the decoded PCM, and forwards it to `SoundEngine::playBgm`.
| Tests | No unit tests under `src/native/audio` that target audio (search for `*_test*` yields none). Existing tests live under other modules (e.g., `tests/native_assets`). |

### What is **implemented**
- VAG decoding (raw ADPCM → PCM) ✅
- BGM playback with looping ✅
- Per‑voice volume, fading, pan, mono handling ✅
- Master volume control ✅
- Mixer that sums all active voices ✅

### What is **missing** or **partial**
- **Title screen BGM** – No track index mapping for the title theme in `MusicLibrary`. The disc contains only the generic `SOUND/*CH*_VAG` streams, but the title theme appears to be a special TSQ/TVB combo that is never resolved.
- **Menu SFX (SE) assets** – No `.VS` (or other SE) files were discovered in the ROM. The engine can play SEs, but there is no source data.
- **Race BGM cross‑fade** – `SoundEngine::playBgm` simply replaces a slot; there is no built‑in cross‑fade or fade‑out when switching BGM during a race.
- **Audio test coverage** – No dedicated audio tests for decoding, mixing, or BGM‑slot behaviour.

## 5. Config references (audio‑related keys)

| Config file | Key / path | Meaning |
|-------------|------------|---------|
| `config/game_state.toml` | `iop_bgmbuf` | Slot << 8 | song of the last `PlayBGM` (stale when stopped).
| `config/game_state.toml` | `play_bgm` | RPC command layout: `0x0A ff ss nn` (file, slot, song).
| `config/game_state.toml` | `race_bgm_table` | Table of 18 BGM codes indexed by course ID.
| `config/game_state.toml` | `radio_station` | Radio selection (0 = off, 1 = PEACH FM (`SOUND/1CH`), 2 = E‑RADIO (`SOUND/3CH`)).
| `config/game_state.toml` | `radio_streams` | List of stream identifiers (`SOUND/1CH`, `SOUND/3CH`, `SOUND/2CH`).
| `config/roadtrip.base.toml` | (audio volume keys hidden in comments) – notes about `sceSdSetParam` (sound volume) and `sceSdSetEffectAttr` (reverb) being stubbed.

These keys drive the high‑level behaviour of the native engine but do **not** reference any additional file assets.

## 6. Prioritized gap list

| Gap | Description | Native module to extend / adapt |
|-----|-------------|--------------------------------|
| **(a) Title BGM playing** | The title screen expects a dedicated BGM (TSQ/TVB) that is never loaded because `MusicLibrary` lacks an entry for the title file index. | Add a mapping in `MusicLibrary::addTrack` (or initialise the library elsewhere) for the title index, and ensure `BgmPlayer::load` can locate the associated TSQ/TVB assets (currently missing from the ROM). |
| **(b) Menu SFX** | No sound‑effect (`.VS` or similar) files are present in the disc, yet the engine calls `SoundEngine::playSe`. | Populate the ROM with SE assets (or provide a fallback silent buffer) and extend `MusicLibrary`/`BgmPlayer` to resolve SE indices, or implement a synthetic generator for UI clicks. |
| **(c) Race BGM loop + cross‑fade** | During race transitions the game swaps BGM slots abruptly; a smooth cross‑fade is desirable. | Add a cross‑fade helper in `SoundEngine` that fades out the old slot while fading in the new one (reuse existing `fadeChannel`). Update the race‑logic RPC wrappers (`PlayBgmSlot*`) to invoke the helper instead of immediate `playBgm`. |
| **(d) Test coverage** | No unit tests verify VAG decoding, mixing, or BGM slot behaviour. | Add tests under `tests/native/audio/` that load a small handcrafted VAG, decode it, feed it to `SoundEngine`, and assert PCM output, looping, and volume/fade correctness. |

---

*All observations are based on the current repository checkout and the read‑only disc image provided. Items marked as “open questions” require further ROM investigation or game‑logic reverse‑engineering.*