// Authored sound engine implementation. See engine.h for the model and the
// mapping to the documented SNDMOD wrappers
// (docs/game-mechanics/sound-and-daynight.md §1).
//
// Tick flow (driven by the host once per audio frame):
//   1. Advance every voice's fade ramp by min(frames, fadeRemainingFrames).
//   2. For each active voice, point its mixer channel at a fresh slice of
//      the source's PCM (starting at the voice's cursor, length = min(frames,
//      remaining). One-shots whose cursor reaches the end retire here.
//   3. mixer.mix(out, frames) sums every channel.
//   4. Advance each voice's cursor by the frames the mixer's readCursor
//      walked (which is at most `frames`; smaller if the voice's slice ran
//      out mid-tick). One-shots with cursor >= total retire.
#include "audio/engine.h"

#include <algorithm>
#include <cstdio>

namespace rt::native::audio {

BgmIdParts unpackBgmId(std::uint32_t packedId) {
    BgmIdParts p;
    // Per docs/game-mechanics/sound-and-daynight.md §1 "BGM": the packed
    // BGM id is broken down as
    //   (id >> 16) & 0xff   → gs_music_file_table index (cmd 7 file row)
    //   (id >> 8)  & 0xff   → cmd 7 parameter
    //   low bits           → the `id` arg sent with cmd 0xE
    // Doc marks this layout [inferred from gs_music_play_bgm] and gives
    // 0x0a1c0201 / 0x0a1a0201 / 0x0a100201 as examples.
    p.fileIndex = (packedId >> 16) & 0xFFu;
    p.param     = (packedId >> 8)  & 0xFFu;
    p.lowId     =  packedId        & 0xFFu;
    return p;
}

SoundEngine::SoundEngine() {
    voices_.resize(kMaxVoices);
    mixer_.channels.resize(kMaxVoices);
    for (std::size_t c = 0; c < kMaxVoices; ++c) {
        mixer_.channels[c].pcm = nullptr;
        mixer_.channels[c].pcmFrames = 0;
        mixer_.channels[c].volume = 0.f;
    }
}

int SoundEngine::findIdleSeVoice() const {
    // SE pool occupies voice indices [kSePoolStart, kMaxVoices).
    for (std::size_t v = kSePoolStart; v < kMaxVoices; ++v) {
        if (!voices_[v].playing) return static_cast<int>(v);
    }
    return -1;
}

void SoundEngine::bindVoice(int v, PcmSourceView src, float volume,
                            float pan, bool mono) {
    if (v < 0 || static_cast<std::size_t>(v) >= kMaxVoices) return;
    Voice& vo = voices_[v];
    vo.source = src;
    vo.cursor = 0;
    vo.volume = volume;
    vo.targetVolume = volume;
    vo.fadeStartVolume = volume;
    vo.fadeRemainingFrames = 0;
    vo.fadeTotalFrames = 0;
    vo.pan = pan;
    vo.mono = mono;
    vo.loop = src.loop;
    vo.playing = (src.data != nullptr && src.frames > 0);
    // Mixer channel starts in a "no slice" state (pcm=nullptr); the tick
    // step is what repoints it at a fresh slice each frame. We still set
    // the volume/pan here so a caller can read voiceVolume() or peek the
    // mix state without having tick'd first.
    MixerChannel& mc = mixer_.channels[v];
    mc.pcm = nullptr;
    mc.pcmFrames = 0;
    mc.readCursor = 0;
    mc.volume = vo.playing ? volume : 0.f;
    mc.pan = pan;
    mc.mono = mono;
}

void SoundEngine::unbindVoice(int v) {
    if (v < 0 || static_cast<std::size_t>(v) >= kMaxVoices) return;
    Voice& vo = voices_[v];
    vo.source = PcmSourceView{};
    vo.cursor = 0;
    vo.volume = 0.f;
    vo.targetVolume = 0.f;
    vo.fadeRemainingFrames = 0;
    vo.fadeTotalFrames = 0;
    vo.playing = false;
    vo.loop = false;
    MixerChannel& mc = mixer_.channels[v];
    mc.pcm = nullptr;
    mc.pcmFrames = 0;
    mc.readCursor = 0;
    mc.volume = 0.f;
}

void SoundEngine::playBgm(int slot, std::uint32_t packedId, PcmSourceView source,
                          float volume) {
    // The packed id is unpacked for asset-pipeline use; the engine doesn't
    // look up a file (that's the caller's job).
    (void)unpackBgmId(packedId);

    if (slot < 0 || static_cast<std::size_t>(slot) >= kBgmSlots) return;

    // Stop whatever is in the slot first.
    const int existing = bgmSlots_[slot];
    if (existing >= 0) {
        unbindVoice(existing);
        bgmSlots_[slot] = -1;
    }

    // BGM lives at slot indices 0..kBgmSlots-1 directly.
    const int v = static_cast<int>(slot);

    // BGM defaults to loop even if the caller forgot the flag.
    PcmSourceView src = source;
    if (src.data != nullptr && src.frames > 0 && !src.loop) src.loop = true;

    bindVoice(v, src, volume, 0.f, false);
    bgmSlots_[slot] = (src.data != nullptr && src.frames > 0) ? v : -1;
}

void SoundEngine::setVoicePan(int voice, float pan) {
    if (voice < 0 || static_cast<std::size_t>(voice) >= kMaxVoices) return;
    Voice& vo = voices_[voice];
    vo.pan = pan;
    mixer_.channels[static_cast<std::size_t>(voice)].pan = pan;
}

void SoundEngine::stopBgm() {
    for (std::size_t s = 0; s < kBgmSlots; ++s) {
        const int v = bgmSlots_[s];
        if (v >= 0) {
            unbindVoice(v);
            bgmSlots_[s] = -1;
        }
    }
}

void SoundEngine::stopAll() {
    for (std::size_t v = 0; v < kMaxVoices; ++v) unbindVoice(static_cast<int>(v));
    for (std::size_t s = 0; s < kBgmSlots; ++s) bgmSlots_[s] = -1;
}

bool SoundEngine::playSe(PcmSourceView source, float volume, float pan) {
    if (source.data == nullptr || source.frames == 0) return false;
    const int v = findIdleSeVoice();
    if (v < 0) return false;
    // SE is one-shot: never loop.
    PcmSourceView src = source;
    src.loop = false;
    bindVoice(v, src, volume, pan, false);
    return true;
}

void SoundEngine::setMasterVolume(float v) {
    mixer_.masterVolume = std::max(0.f, v);
}

void SoundEngine::setChannelVolume(int voice, float v) {
    if (voice < 0 || static_cast<std::size_t>(voice) >= kMaxVoices) return;
    Voice& vo = voices_[voice];
    vo.volume = v;
    vo.targetVolume = v;
    vo.fadeRemainingFrames = 0;     // cancel any in-flight fade
    vo.fadeTotalFrames = 0;
    vo.fadeStartVolume = v;
    mixer_.channels[voice].volume = v;
}

void SoundEngine::fadeChannel(int voice, float target, int frames) {
    if (voice < 0 || static_cast<std::size_t>(voice) >= kMaxVoices) return;
    Voice& vo = voices_[voice];
    vo.fadeStartVolume = vo.volume;
    vo.targetVolume = target;
    if (frames <= 0) {
        vo.volume = target;
        vo.fadeRemainingFrames = 0;
        vo.fadeTotalFrames = 0;
        mixer_.channels[voice].volume = target;
    } else {
        vo.fadeRemainingFrames = frames;
        vo.fadeTotalFrames = frames;
    }
}

void SoundEngine::advanceFades(int frames) {
    if (frames <= 0) return;
    for (std::size_t v = 0; v < kMaxVoices; ++v) {
        Voice& vo = voices_[v];
        if (vo.fadeRemainingFrames <= 0) continue;
        // `fadeTotalFrames` is the ORIGINAL fade length, set when the
        // fade was kicked off. Computing `t` from the decrementing
        // remaining is what made multi-tick fades non-linear; using the
        // frozen total keeps the ramp linear across any tick size.
        const int total = (vo.fadeTotalFrames > 0) ? vo.fadeTotalFrames
                                                   : vo.fadeRemainingFrames;
        const int step   = std::min<int>(vo.fadeRemainingFrames, frames);
        const float delta = vo.targetVolume - vo.fadeStartVolume;
        vo.fadeRemainingFrames -= step;
        const int elapsed = total - vo.fadeRemainingFrames;
        const float t = static_cast<float>(elapsed) / static_cast<float>(total);
        vo.volume = vo.fadeStartVolume + delta * t;
        mixer_.channels[v].volume = vo.volume;
        if (vo.fadeRemainingFrames <= 0) {
            // Snap to target on completion so floating-point drift
            // can't leave a fractional gap at the end of the ramp.
            vo.volume = vo.targetVolume;
            vo.fadeTotalFrames = 0;
            mixer_.channels[v].volume = vo.volume;
        }
    }
}

void SoundEngine::tick(int frames, OutputBuffer& out) {
    if (frames <= 0) return;
    const std::size_t want = static_cast<std::size_t>(frames);

    // 1. Advance fades (use current voice volume for this tick).
    advanceFades(frames);

    // 2. Repoint each active voice's mixer channel at a fresh slice of its
    //    source PCM, sized for at most `frames` frames.
    for (std::size_t v = 0; v < kMaxVoices; ++v) {
        Voice& vo = voices_[v];
        if (!vo.playing) continue;
        const std::size_t total = vo.source.frames;
        if (total == 0) {
            unbindVoice(static_cast<int>(v));
            continue;
        }
        const std::size_t cur = vo.cursor;
        if (cur >= total) {
            // Cursor overshot: either loop or retire.
            if (vo.loop) vo.cursor = 0;
            else { unbindVoice(static_cast<int>(v)); continue; }
        }
        // Slice length: at most `frames`, bounded by what the source has
        // left until end (or until the start of the next loop, for looping
        // voices). For looping voices the natural bound is just `frames`
        // because the slice rolls from the cursor and the cursor will
        // wrap next tick.
        const std::size_t remaining = total - vo.cursor;
        const std::size_t slice     = std::min(want, remaining);

        MixerChannel& mc = mixer_.channels[v];
        mc.pcm = vo.source.data + vo.cursor;
        mc.pcmFrames = slice;
        mc.readCursor = 0;
    }

    // 3. Drive the mixer. The mixer's per-channel readCursor advances up
    //    to `frames` (or less if the channel's slice was shorter).
    mixer_.mix(out, want);

    // 4. Advance voice cursors by the frames the mixer consumed on each
    //    channel. After the slice is consumed, looping voices wrap and
    //    one-shots retire.
    for (std::size_t v = 0; v < kMaxVoices; ++v) {
        Voice& vo = voices_[v];
        if (!vo.playing) continue;
        const std::size_t consumed =
            std::min<std::size_t>(want, mixer_.channels[v].readCursor);
        if (consumed == 0) continue;
        const std::size_t total = vo.source.frames;
        const std::size_t next  = vo.cursor + consumed;
        if (vo.loop) {
            vo.cursor = next % total;
        } else {
            if (next >= total) {
                unbindVoice(static_cast<int>(v));
            } else {
                vo.cursor = next;
            }
        }
    }
}

int SoundEngine::activeVoiceCount() const {
    int n = 0;
    for (const Voice& v : voices_) if (v.playing) ++n;
    return n;
}

int SoundEngine::activeBgmSlot(int slot) const {
    if (slot < 0 || static_cast<std::size_t>(slot) >= kBgmSlots) return -1;
    const int v = bgmSlots_[slot];
    if (v < 0) return -1;
    return voices_[v].playing ? v : -1;
}

float SoundEngine::voiceVolume(int voice) const {
    if (voice < 0 || static_cast<std::size_t>(voice) >= kMaxVoices) return 0.f;
    return voices_[voice].volume;
}

bool SoundEngine::voicePlaying(int voice) const {
    if (voice < 0 || static_cast<std::size_t>(voice) >= kMaxVoices) return false;
    return voices_[voice].playing;
}

const Voice& SoundEngine::voiceAt(int voice) const {
    return voices_[voice];
}

}  // namespace rt::native::audio
