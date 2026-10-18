// Authored sound engine for the native audio vertical. Pure host C++; no
// PS2/IOP/RPC/guest code. The engine replaces the original's IOP-side SNDMOD
// RPC server plus the EE wrapper cluster (see docs/game-mechanics/
// sound-and-daynight.md §1): where the original staged a request at
// 0x175E880, called sceSifCallRpc against the IOP server id 0x1234567, and
// waited for the IOP to reply, this engine drives the VAG decoder and the
// mixer model in the same process. The EE-side opcode wrappers
// (gs_music_play_bgm 0x258EC8, SndRpcPlayOrStop 0x258308, ApplySoundVolumes
// 0x2590B0, SoundStopAllChannels 0x258FF0, etc.) collapse into the methods
// on this class. Guest addresses are cited for orientation only.
//
// Voice pool: kMaxVoices = 24 split into kBgmSlots = 4 BGM slots and a SE
// pool of the rest. The split matches the documented BGM wrapper convention
// (PlayBgmSlot1..PlayBgmSlot6, six slot helpers; we keep four — the active
// scenes overlap at most that many at once — and the SE pool handles the
// rest). Channel count is `mixer.MixerState::channels.size()`; voice `i`
// owns mixer channel `i`. Voices that aren't playing have pcm=nullptr in
// the mixer channel.
//
// Source model: a PcmSourceView is a non-owning view over an already-decoded
// int16 PCM buffer. Future slices will plug in a "decode on demand" source
// that calls back into vag.h::decodeVagStream, but this engine treats the
// PCM as already-decoded. Tests bind synthetic buffers directly without
// touching the VAG decoder. This is the same separation vag/mixer already
// enforces — sources are not the engine's concern, they hand it PCM.
//
// Fades model the IOP-side SNDMOD cmd 0x26 (per-channel fade). The engine
// ramps `voice.volume` linearly toward `targetVolume` over `remainingFrames`
// frames, advancing once per `tick(frames)`. After the ramp the voice holds
// the target.
//
// The one SE voice model mirrors SndRpcPlayOrStop's cmd 0xD: one PCM plays
// to end, then the voice retires (release). BGM voices default to loop=true
// so they keep playing past the end of one loop iteration.
#pragma once

#include "audio/mixer.h"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace rt::native::audio {

// ----- Voice pool sizing ---------------------------------------------------

inline constexpr std::size_t kBgmSlots = 4;
inline constexpr std::size_t kMaxVoices = 24;
inline constexpr std::size_t kSePoolStart = kBgmSlots;

// ----- BGM id packing ------------------------------------------------------
// gs_music_play_bgm packs a BGM id like 0x0a1c0201. Per docs/game-mechanics/
// sound-and-daynight.md §1 "BGM", the breakdown is:
//   (id >> 16) & 0xff   → gs_music_file_table index (cmd 7 file row)
//   (id >> 8)  & 0xff   → cmd 7 parameter
//   id & 0xff           → the low-byte cmd 0xE id
// Doc marks this layout [inferred from gs_music_play_bgm] and gives
// 0x0a1c0201 / 0x0a1a0201 / 0x0a100201 as the canonical BGM ids.
struct BgmIdParts {
    std::uint32_t fileIndex = 0;  // (id >> 16) & 0xff
    std::uint32_t param     = 0;  // (id >> 8) & 0xff
    std::uint32_t lowId     = 0;  // id & 0xff
};
BgmIdParts unpackBgmId(std::uint32_t packedId);

// ----- PCM source ----------------------------------------------------------

// Non-owning view over an already-decoded PCM buffer. The engine treats
// these as opaque PCM; whatever produced them (VAG decoder, future TSQ
// streaming, a synthetic test buffer) is the caller's responsibility. A
// view with `data=nullptr` and `frames=0` is a silent source.
struct PcmSourceView {
    const std::int16_t* data = nullptr;
    std::size_t frames = 0;     // mono frames
    bool loop = false;
};

// One voice in the pool. A voice with `source.data == nullptr` is idle
// (the matching mixer channel has pcm=nullptr and contributes silence).
struct Voice {
    PcmSourceView source {};
    std::size_t cursor = 0;             // frames consumed this playback
    float volume = 1.f;                 // current scaled volume
    float targetVolume = 1.f;           // fade target; == volume when no fade
    int fadeRemainingFrames = 0;        // 0 = no fade active
    int fadeTotalFrames = 0;            // ORIGINAL fade length; progress is
                                        //   (fadeTotalFrames - fadeRemainingFrames)
                                        //   / fadeTotalFrames so the ramp stays
                                        //   linear across multi-tick fade advances
    float fadeStartVolume = 1.f;        // volume at fade kickoff
    float pan = 0.f;                    // [-1..+1]
    bool mono = false;                  // mono override (per-voice)
    bool playing = false;                // active in the mixer
    bool loop = false;                   // copy of source.loop kept locally so
                                        //   callers can toggle mid-playback
};

// ----- Engine --------------------------------------------------------------

class SoundEngine {
public:
    SoundEngine();

    // ---- BGM (mirror gs_music_play_bgm + MusicRpcPlayBgm, slot-bgtable) ----
    // Stop whatever is in the slot, allocate the BGM voice at `slot`
    // (0..kBgmSlots-1), bind `source` to it at `volume`, and mark it
    // playing. `packedId` is interpreted through `unpackBgmId` so callers
    // (the future asset slice) can pick the right file; the engine doesn't
    // actually look up a file — that's `source`'s responsibility.
    void playBgm(int slot, std::uint32_t packedId, PcmSourceView source,
                 float volume = 1.f);

    // Re-pan a live voice. playBgm binds pan = 0 (mirror of the engine's
    // centred BGM default); the radio station pair needs the hard split the
    // guest gets from two SNDMOD voices (radio-stations.md: L file pan -1,
    // R file pan +1). Mixer::PanLaw (mixer.h:48-61) gives L=1,R=0 at -1 and
    // L=0,R=1 at +1, matching the guest's per-voice E1 split.
    void setVoicePan(int voice, float pan);

    // Stop all BGM slots (mirror MusicRpcCmd15 with param=1, hard stop).
    void stopBgm();

    // ---- Teardown (mirror SoundRpcStopAll cmd 0x20) ------------------------
    // Stop every voice (BGM + SE) and reset volumes.
    void stopAll();

    // ---- SE one-shot (mirror SndRpcPlayOrStop cmd 0xD) ---------------------
    // Pick the first idle voice in the SE pool, bind `source` to it at
    // `volume`, and mark it playing. If the SE pool is full the request is
    // dropped (the original SNDMOD overwrites a slot in that case; we
    // choose the safer no-op to avoid masking bugs — a future slice can
    // implement a steal-oldest policy). Returns true on success.
    bool playSe(PcmSourceView source, float volume = 1.f, float pan = 0.f);

    // ---- Volume (mirror SNDMOD commands) -----------------------------------
    // Master volume — cmd 0x31. Scales every voice.
    void setMasterVolume(float v);

    // Per-channel volume — cmd 0x21. Operates on the given voice; the
    // mixer channel follows the voice index.
    void setChannelVolume(int voice, float v);

    // Per-voice fade — cmd 0x26. `target` is reached after exactly
    // `frames` ticks. Calling again on an already-fading voice replaces
    // the ramp from the current volume (a "re-target").
    void fadeChannel(int voice, float target, int frames);

    // ---- Tick (drive the mixer) -------------------------------------------
    // Advance fades by `frames` frames, write `frames` interleaved-stereo
    // frames into `out`, and retire one-shots whose PCM is exhausted. The
    // mixer is driven once per call. Calling with frames=0 is a no-op.
    void tick(int frames, OutputBuffer& out);

    // ---- Introspection (used by tests and future debug UI) ----------------
    int activeVoiceCount() const;       // voices with playing=true
    int activeBgmSlot(int slot) const;  // -1 if slot is empty
    float voiceVolume(int voice) const; // current (post-fade) volume
    bool voicePlaying(int voice) const;
    const Voice& voiceAt(int voice) const;

private:
    std::vector<Voice> voices_;          // size kMaxVoices
    MixerState mixer_;                   // owns kMaxVoices channels
    int bgmSlots_[kBgmSlots] {-1, -1, -1, -1};  // voice index per slot, -1 = empty

    // Helpers
    int findIdleSeVoice() const;          // -1 if none
    void bindVoice(int v, PcmSourceView src, float volume, float pan, bool mono);
    void unbindVoice(int v);
    void advanceFades(int frames);
};

}  // namespace rt::native::audio
