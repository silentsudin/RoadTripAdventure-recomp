#include "gameplay/audio_graph.h"

#include <algorithm>
#include <cmath>
#include <cstring>

namespace rt::native::gameplay::audio_graph {

// ---------------------------------------------------------------- pan / gain

StereoGain panLinear(float pan) {
    pan = std::clamp(pan, -1.f, 1.f);
    return {pan >= 0.f ? 1.f - pan : 1.f, pan <= 0.f ? 1.f + pan : 1.f};
}

StereoGain panConstantPower(float pan) {
    pan = std::clamp(pan, -1.f, 1.f);
    const float a = (pan + 1.f) * 0.78539816339744830962f;  // 0..pi/2
    return {std::cos(a), std::sin(a)};
}

StereoGain balanceFromE1(std::uint8_t ll, std::uint8_t rr) {
    return {static_cast<float>(ll) / 64.f, static_cast<float>(rr) / 64.f};
}

std::int16_t sndmodSideVolume(std::uint8_t vol, std::uint8_t pan, std::int16_t master) {
    std::int32_t p = static_cast<std::int32_t>(vol) * static_cast<std::int32_t>(pan);
    p = static_cast<std::int32_t>(static_cast<std::uint32_t>(p) * static_cast<std::uint32_t>(
                                      static_cast<std::int32_t>(master)));
    if (p < 0) p += 0xFF;  // bgez/addiu 0xFF/sra 8 in the IRX: truncate toward zero
    return static_cast<std::int16_t>(p >> 8);
}

void sndmodMonoFold(std::int16_t& l, std::int16_t& r) {
    std::int32_t s = static_cast<std::int32_t>(l) + static_cast<std::int32_t>(r);
    s += (s < 0) ? 1 : 0;
    const auto m = static_cast<std::int16_t>(s >> 1);
    l = m;
    r = m;
}

std::uint16_t sndmodFreq(int key) {
    if (key < 0 || key >= kFreqTableSize) return 0;
    return static_cast<std::uint16_t>(std::floor(4096.0 * std::exp2((key - 48) / 12.0)));
}

std::uint16_t applyBend(std::uint16_t freq, std::uint16_t bend) {
    if (bend == 0) return freq;
    auto p = static_cast<std::int32_t>(static_cast<std::uint32_t>(freq) * bend);
    if (p < 0) p += 0xFFF;
    return static_cast<std::uint16_t>(p >> 12);
}

// ----------------------------------------------------------- ids / BGM / SFX

BgmId unpackBgmId(std::uint32_t id) {
    BgmId b;
    b.fileIndex = (id >> 16) & 0xFFu;
    b.param = (id >> 8) & 0xFFu;
    b.song = id & 0xFFu;
    b.wellFormed = (id >> 24) == 0x0Au;
    return b;
}

const std::array<std::uint32_t, kRaceBgmEntries>& raceBgmDefaultTable() {
    static const std::array<std::uint32_t, kRaceBgmEntries> t = [] {
        constexpr std::uint8_t idx[kRaceBgmEntries] = {12, 12, 19, 19, 16, 16, 18, 20, 17,
                                                       17, 17, 18, 20, 15, 15, 21, 21, 14};
        std::array<std::uint32_t, kRaceBgmEntries> r{};
        for (int i = 0; i < kRaceBgmEntries; ++i) r[static_cast<std::size_t>(i)] = packBgmId(idx[i], 2);
        return r;
    }();
    return t;
}

std::uint32_t raceBgmId(std::uint8_t mode20, std::int8_t mode22,
                        std::span<const std::uint32_t, kRaceBgmEntries> table) {
    if (mode20 == 0x1A) return 0x0A170201u;
    if (mode22 < 0) return 0x0A0D0201u;  // original indexes below the table (OOB); native fallback
    if (mode22 < kRaceBgmEntries) return table[static_cast<std::size_t>(mode22)];
    if (mode22 == 0x1C) return 0x0A160201u;
    return 0x0A0D0201u;
}

SfxId decodeSfxId(std::uint32_t id) {
    SfxId s;
    s.stop = (id & 0x8000u) != 0;
    s.base = static_cast<std::uint16_t>(id & 0x7FFFu);
    return s;
}

RadioRows radioRows(std::uint8_t station) {
    switch (station) {
        case 1: return {true, 4, 5};
        case 2: return {true, 8, 9};
        default: return {};
    }
}

// ---------------------------------------------------------------- voice graph

std::size_t BufferSource::read(std::int16_t* dst, std::size_t frames) {
    const std::size_t n = std::min(frames, frames_ - pos_);
    if (n != 0 && data_ != nullptr) std::memcpy(dst, data_ + pos_, n * sizeof(std::int16_t));
    pos_ += n;
    return n;
}

AudioGraph::AudioGraph(std::uint32_t outRate) : outRate_(outRate == 0 ? 48000 : outRate) {}

bool AudioGraph::pull(Voice& v, std::int16_t& s) {
    if (v.chunkPos >= v.chunkLen) {
        v.chunkLen = v.src ? v.src->read(v.chunk.data(), v.chunk.size()) : 0;
        v.chunkPos = 0;
        if (v.chunkLen == 0 && v.loop && v.src) {
            v.src->rewind();
            v.chunkLen = v.src->read(v.chunk.data(), v.chunk.size());
        }
        if (v.chunkLen == 0) return false;
    }
    s = v.chunk[v.chunkPos++];
    return true;
}

void AudioGraph::advance(Voice& v) {
    v.prev = v.cur;
    std::int16_t s = 0;
    if (pull(v, s)) {
        v.cur = s;
    } else {
        v.cur = 0;
        ++v.exhausted;
    }
}

bool AudioGraph::bind(int idx, VoiceClass cls, PcmSource* src, const VoiceParams& p) {
    if (idx < 0 || idx >= kMaxVoices || src == nullptr) return false;
    Voice v;
    v.cls = cls;
    v.src = src;
    v.srcRate = p.srcRate == 0 ? outRate_ : p.srcRate;
    v.pitchQ16 = p.pitchQ16 == 0 ? 65536u : p.pitchQ16;
    v.stepQ16 = static_cast<std::uint32_t>(static_cast<std::uint64_t>(v.srcRate) * v.pitchQ16 / outRate_);
    v.loop = p.loop;
    v.mono = p.mono;
    v.volume = p.volume;
    v.pan = p.pan;
    v.priority = p.priority;
    v.serial = ++serial_;
    v.tag = p.tag;
    v.owner = p.owner;
    std::int16_t s = 0;
    if (pull(v, s)) v.prev = s; else ++v.exhausted;
    if (pull(v, s)) v.cur = s; else ++v.exhausted;
    voices_[static_cast<std::size_t>(idx)] = v;
    return true;
}

bool AudioGraph::playBgm(int slot, std::uint32_t packedId, PcmSource* src, const VoiceParams& p) {
    if (slot < 0 || slot >= kBgmVoices) return false;
    VoiceParams q = p;
    q.tag = packedId;
    return bind(slot, VoiceClass::Bgm, src, q);
}

void AudioGraph::stopBgm(int slot) {
    if (slot >= 0 && slot < kBgmVoices) voices_[static_cast<std::size_t>(slot)] = Voice{};
}

bool AudioGraph::playRadioPair(PcmSource* left, PcmSource* right, float volume, std::uint32_t srcRate) {
    if (left == nullptr || right == nullptr) return false;
    VoiceParams p;
    p.volume = volume;
    p.loop = true;
    p.srcRate = srcRate;
    p.pan = -1.f;
    p.mono = false;
    if (!bind(kRadioLVoice, VoiceClass::RadioL, left, p)) return false;
    p.pan = +1.f;
    return bind(kRadioRVoice, VoiceClass::RadioR, right, p);
}

void AudioGraph::stopRadio() {
    voices_[kRadioLVoice] = Voice{};
    voices_[kRadioRVoice] = Voice{};
}

int AudioGraph::pickSeVoice(int priority) const {
    int victim = -1;
    for (int i = kSeVoiceStart; i < kMaxVoices; ++i) {
        const Voice& v = voices_[static_cast<std::size_t>(i)];
        if (v.cls == VoiceClass::Free) return i;
        if (victim < 0) { victim = i; continue; }
        const Voice& w = voices_[static_cast<std::size_t>(victim)];
        if (v.priority < w.priority || (v.priority == w.priority && v.serial < w.serial)) victim = i;
    }
    if (victim >= 0 && voices_[static_cast<std::size_t>(victim)].priority <= priority) return victim;
    return -1;
}

int AudioGraph::playSe(std::uint32_t id, PcmSource* src, const VoiceParams& p) {
    const int idx = pickSeVoice(p.priority);
    if (idx < 0) return -1;
    VoiceParams q = p;
    q.tag = id;
    return bind(idx, VoiceClass::Se, src, q) ? idx : -1;
}

int AudioGraph::playNote(int owner, PcmSource* src, const VoiceParams& p) {
    releaseOwner(owner, 0);
    const int idx = pickSeVoice(p.priority);
    if (idx < 0) return -1;
    VoiceParams q = p;
    q.owner = owner;
    return bind(idx, VoiceClass::Seq, src, q) ? idx : -1;
}

void AudioGraph::releaseOwner(int owner, std::uint32_t releaseFrames) {
    for (auto& v : voices_) {
        if (v.cls != VoiceClass::Seq || v.owner != owner) continue;
        if (releaseFrames == 0) {
            v = Voice{};
        } else {
            v.fadeStart = v.volume;
            v.fadeTarget = 0.f;
            v.fadeTotal = releaseFrames;
            v.fadeDone = 0;
            v.fadeRetire = true;
            v.owner = -1;  // a re-trigger must not release a voice already releasing
        }
    }
}

void AudioGraph::stop(int voice) {
    if (voice >= 0 && voice < kMaxVoices) voices_[static_cast<std::size_t>(voice)] = Voice{};
}

void AudioGraph::stopAll() {
    for (auto& v : voices_) v = Voice{};
}

void AudioGraph::setVolume(int voice, float v) {
    if (voice < 0 || voice >= kMaxVoices) return;
    auto& x = voices_[static_cast<std::size_t>(voice)];
    x.volume = v;
    x.fadeTotal = 0;
}

void AudioGraph::setPan(int voice, float pan) {
    if (voice >= 0 && voice < kMaxVoices) voices_[static_cast<std::size_t>(voice)].pan = pan;
}

void AudioGraph::fade(int voice, float target, std::uint32_t frames, bool retire) {
    if (voice < 0 || voice >= kMaxVoices) return;
    auto& v = voices_[static_cast<std::size_t>(voice)];
    if (v.cls == VoiceClass::Free) return;
    v.fadeStart = v.volume;
    v.fadeTarget = target;
    v.fadeTotal = frames;
    v.fadeDone = 0;
    v.fadeRetire = retire;
    if (frames == 0) {
        v.volume = target;
        if (retire) v = Voice{};
    }
}

void AudioGraph::crossfade(int from, int to, std::uint32_t frames) {
    fade(from, 0.f, frames, true);
    fade(to, 1.f, frames, false);
}

void AudioGraph::mix(std::int16_t* out, std::size_t frames) {
    for (std::size_t f = 0; f < frames; ++f) {
        float accL = 0.f, accR = 0.f;
        for (auto& v : voices_) {
            if (v.cls == VoiceClass::Free) continue;
            if (v.exhausted >= 2) { v = Voice{}; continue; }
            if (v.fadeTotal != 0) {
                if (v.fadeDone >= v.fadeTotal) {
                    v.volume = v.fadeTarget;
                    v.fadeTotal = 0;
                    if (v.fadeRetire) { v = Voice{}; continue; }
                } else {
                    v.volume = v.fadeStart + (v.fadeTarget - v.fadeStart) *
                                                 (static_cast<float>(v.fadeDone) / static_cast<float>(v.fadeTotal));
                    ++v.fadeDone;
                }
            }
            const float frac = static_cast<float>(v.phaseQ16) / 65536.f;
            const float s = static_cast<float>(v.prev) + static_cast<float>(v.cur - v.prev) * frac;
            StereoGain g = panLinear(v.pan);
            if (v.mono || masterMono_) {
                const float m = 0.5f * (g.l + g.r);
                g.l = m;
                g.r = m;
            }
            const float a = s * v.volume * master_;
            accL += a * g.l;
            accR += a * g.r;
            v.phaseQ16 += v.stepQ16;
            while (v.phaseQ16 >= 65536u && v.exhausted < 2) {
                v.phaseQ16 -= 65536u;
                advance(v);
            }
            if (v.exhausted >= 2) v = Voice{};
        }
        out[2 * f] = static_cast<std::int16_t>(std::clamp(std::lround(accL), -32768L, 32767L));
        out[2 * f + 1] = static_cast<std::int16_t>(std::clamp(std::lround(accR), -32768L, 32767L));
    }
}

int AudioGraph::activeVoices() const {
    int n = 0;
    for (const auto& v : voices_) n += v.cls != VoiceClass::Free;
    return n;
}

VoiceView AudioGraph::view(int voice) const {
    VoiceView w;
    if (voice < 0 || voice >= kMaxVoices) return w;
    const Voice& v = voices_[static_cast<std::size_t>(voice)];
    w.cls = v.cls;
    w.volume = v.volume;
    w.pan = v.pan;
    w.priority = v.priority;
    w.serial = v.serial;
    w.tag = v.tag;
    w.owner = v.owner;
    w.fading = v.fadeTotal != 0;
    return w;
}

std::uint64_t AudioGraph::stateHash() const {
    std::uint64_t h = 1469598103934665603ull;
    auto mixIn = [&h](std::uint64_t x) {
        for (int i = 0; i < 8; ++i) { h ^= (x >> (8 * i)) & 0xFFu; h *= 1099511628211ull; }
    };
    for (const auto& v : voices_) {
        std::uint32_t volBits = 0;
        std::memcpy(&volBits, &v.volume, sizeof volBits);
        mixIn(static_cast<std::uint64_t>(v.cls));
        mixIn(v.serial);
        mixIn(v.tag);
        mixIn(static_cast<std::uint64_t>(static_cast<std::int64_t>(v.priority)));
        mixIn(volBits);
    }
    return h;
}

// ------------------------------------------------------------- sequence player

bool SeqPlayer::fetch(Track& t, std::uint8_t& b) const {
    if (t.pc >= file_.size()) return false;
    b = file_[t.pc++];
    return true;
}

int SeqPlayer::start(std::uint32_t pc, std::uint8_t channel, std::uint8_t priority) {
    if (pc >= file_.size()) return -1;
    for (int i = 0; i < kSeqTrackSlots; ++i) {
        Track& t = trk_[static_cast<std::size_t>(i)];
        if (t.active) continue;
        t = Track{};
        t.active = true;
        t.pc = pc;
        t.countdown = 0;  // first tick() decrements to -1 and reads
        t.channel = channel;
        t.pri = priority;
        return i;
    }
    return -1;
}

bool SeqPlayer::active(int slot) const {
    return slot >= 0 && slot < kSeqTrackSlots && trk_[static_cast<std::size_t>(slot)].active;
}

int SeqPlayer::activeCount() const {
    int n = 0;
    for (const auto& t : trk_) n += t.active;
    return n;
}

void SeqPlayer::volumes(const Track& t, std::int16_t& l, std::int16_t& r) const {
    l = sndmodSideVolume(t.vol, t.panL, master_);
    r = sndmodSideVolume(t.vol, t.panR, master_);
    if (mono_) sndmodMonoFold(l, r);
}

void SeqPlayer::tick(std::vector<SeqEvent>& out) {
    for (int i = 0; i < kSeqTrackSlots; ++i) {
        Track& t = trk_[static_cast<std::size_t>(i)];
        if (!t.active) continue;
        t.countdown = static_cast<std::int16_t>(t.countdown - 1);  // addiu -1 / sh (0x52C4..0x52CC)
        if (t.countdown != -1) continue;                           // beq -1 (0x52DC)
        run(i, out);
    }
    ++tick_;
}

void SeqPlayer::run(int slot, std::vector<SeqEvent>& out) {
    Track& t = trk_[static_cast<std::size_t>(slot)];
    auto ev = [&](SeqEventKind k) {
        SeqEvent e;
        e.tick = tick_;
        e.track = slot;
        e.kind = k;
        e.freq = t.freq;
        e.program = t.program;
        e.priority = t.pri;
        e.reverb = t.reverb;
        volumes(t, e.volL, e.volR);
        return e;
    };
    auto endTrack = [&](int reason, std::uint8_t op) {
        if (t.sounding) {
            out.push_back(ev(SeqEventKind::NoteOff));
            t.sounding = false;
        }
        SeqEvent e = ev(SeqEventKind::End);
        e.aux = reason;
        e.key = op;
        out.push_back(e);
        t.active = false;
    };
    auto u16 = [&](std::uint16_t& v) {
        std::uint8_t lo = 0, hi = 0;
        if (!fetch(t, lo) || !fetch(t, hi)) return false;
        v = static_cast<std::uint16_t>(lo | (hi << 8));
        return true;
    };
    for (int guard = 0; guard < 4096; ++guard) {
        std::uint8_t b = 0;
        if (!fetch(t, b)) { endTrack(2, 0); return; }
        if (b < 0x80) {  // step_t: countdown = D - 1
            t.countdown = static_cast<std::int16_t>(b - 1);
            return;
        }
        if (b < 0xE0) {  // key_on
            t.freq = applyBend(sndmodFreq(b & 0x7F), t.bend);
            SeqEvent e = ev(SeqEventKind::NoteOn);
            e.key = static_cast<std::uint8_t>(b & 0x7F);
            out.push_back(e);
            t.sounding = true;
            continue;
        }
        std::uint8_t a = 0, c = 0;
        std::uint16_t w = 0;
        switch (b) {
            case 0xE0:
                if (!fetch(t, a)) { endTrack(2, 0); return; }
                t.vol = a;
                if (t.sounding) out.push_back(ev(SeqEventKind::Volume));
                break;
            case 0xE1:
                if (!fetch(t, a) || !fetch(t, c)) { endTrack(2, 0); return; }
                t.panL = a;
                t.panR = c;
                if (t.sounding) out.push_back(ev(SeqEventKind::Volume));
                break;
            case 0xE2: {  // tone_chg: lb (sign-extended) | channel << 8
                if (!fetch(t, a)) { endTrack(2, 0); return; }
                const int lo = static_cast<std::int8_t>(a);
                t.program = static_cast<std::uint16_t>(lo | (t.channel << 8));
                break;
            }
            case 0xE3:  // tempo: two bytes consumed, nothing stored
                if (!fetch(t, a) || !fetch(t, c)) { endTrack(2, 0); return; }
                break;
            case 0xE4:  // pitch: freq = operand (+bend), no retrigger
                if (!u16(w)) { endTrack(2, 0); return; }
                t.freq = applyBend(w, t.bend);
                if (t.sounding) out.push_back(ev(SeqEventKind::Pitch));
                break;
            case 0xE5:  // key_pitch: freq = operand (+bend) and (re)trigger
                if (!u16(w)) { endTrack(2, 0); return; }
                t.freq = applyBend(w, t.bend);
                out.push_back(ev(SeqEventKind::NoteOn));
                t.sounding = true;
                break;
            case 0xE6:
                if (!fetch(t, a)) { endTrack(2, 0); return; }
                t.revMode = a;
                break;
            case 0xE7:
                if (!fetch(t, a)) { endTrack(2, 0); return; }
                t.revDepth = a;
                break;
            case 0xE8: t.reverb = true; break;
            case 0xE9: t.reverb = false; break;
            case 0xEA:  // pitch_cent: u16 Q12 bend multiplier at +0xC
                if (!u16(w)) { endTrack(2, 0); return; }
                t.bend = w;
                break;
            case 0xF0:
                if (t.sounding) {
                    out.push_back(ev(SeqEventKind::NoteOff));
                    t.sounding = false;
                }
                break;
            case 0xF1:
                if (!fetch(t, a)) { endTrack(2, 0); return; }
                t.pri = a;
                break;
            case 0xF8: {
                if (!u16(w)) { endTrack(2, 0); return; }
                const auto target = static_cast<std::int64_t>(t.pc) + static_cast<std::int16_t>(w);
                if (target < 0 || target >= static_cast<std::int64_t>(file_.size())) { endTrack(2, 0); return; }
                t.pc = static_cast<std::uint32_t>(target);
                break;
            }
            case 0xF9: {  // ext_chan: fork at pc + s16 (pc after both operands)
                if (!u16(w)) { endTrack(2, 0); return; }
                int best = t.pri;
                int pick = -1;
                for (int k = 8; k < kSeqTrackSlots; ++k) {
                    const Track& s = trk_[static_cast<std::size_t>(k)];
                    const int p = s.active ? s.pri : 0;
                    if (best < p) continue;
                    best = p;
                    pick = k;
                }
                if (pick < 0 || pick == slot) break;
                const auto target = static_cast<std::int64_t>(t.pc) + static_cast<std::int16_t>(w);
                if (target < 0 || target >= static_cast<std::int64_t>(file_.size())) break;
                Track copy = t;
                copy.pc = static_cast<std::uint32_t>(target);
                copy.countdown = 0;
                copy.sounding = false;
                trk_[static_cast<std::size_t>(pick)] = copy;
                SeqEvent e = ev(SeqEventKind::Fork);
                e.aux = pick;
                out.push_back(e);
                break;
            }
            default:  // 0xFF and every other unlisted opcode -> data_end
                endTrack(b == 0xFF ? 0 : 1, b);
                return;
        }
    }
    endTrack(3, 0);
}

void applySeqEvents(AudioGraph& g, SeqSampleProvider& prov, std::span<const SeqEvent> events) {
    for (const SeqEvent& e : events) {
        switch (e.kind) {
            case SeqEventKind::NoteOn: {
                const SeqSample s = prov.sample(e.program, e.key);
                if (s.src == nullptr) break;
                VoiceParams p;
                p.srcRate = s.srcRate;
                p.loop = s.loop;
                p.priority = e.priority;
                p.pitchQ16 = static_cast<std::uint32_t>(std::lround(freqToRatio(e.freq) * 65536.0));
                const float gl = spuGain(e.volL), gr = spuGain(e.volR);
                p.volume = std::max(gl, gr);
                p.pan = p.volume > 0.f ? (gr - gl) / p.volume : 0.f;
                g.playNote(e.track, s.src, p);
                break;
            }
            case SeqEventKind::NoteOff:
            case SeqEventKind::End:
                g.releaseOwner(e.track);
                break;
            default: break;  // Pitch/Volume/Fork: host applies via view/setVolume as needed
        }
    }
}

// -------------------------------------------------------------- rumble opcodes

namespace {

constexpr RumbleStep kFx1[] = {{RumbleOp::SmallOn, 0}, {RumbleOp::LargeStrength, 0}, {RumbleOp::Hold, 2},
                               {RumbleOp::SmallOff, 0}, {RumbleOp::Hold, 6}, {RumbleOp::End, 0}};
constexpr RumbleStep kFx0[] = {{RumbleOp::End, 0}};
constexpr RumbleStep kFx2[] = {{RumbleOp::SmallOn, 0}, {RumbleOp::LargeStrength, 0}, {RumbleOp::Hold, 4},
                               {RumbleOp::End, 0}};
constexpr RumbleStep kFx3[] = {{RumbleOp::SmallOn, 0}, {RumbleOp::Hold, 1}, {RumbleOp::SmallOff, 0},
                               {RumbleOp::LargeLit, 0xFF}, {RumbleOp::Hold, 2}, {RumbleOp::End, 0}};
constexpr RumbleStep kFx4[] = {{RumbleOp::LargeLit, 0xFF}, {RumbleOp::Hold, 1}, {RumbleOp::SmallOn, 0},
                               {RumbleOp::Hold, 1}, {RumbleOp::End, 0}};
constexpr RumbleStep kFx5[] = {{RumbleOp::SmallOn, 0}, {RumbleOp::LargeLit, 0xFF}, {RumbleOp::Hold, 2},
                               {RumbleOp::End, 0}};
constexpr RumbleStep kFx6[] = {{RumbleOp::LargeStrength, 0}, {RumbleOp::Hold, 4}, {RumbleOp::SmallOn, 0},
                               {RumbleOp::Hold, 2}, {RumbleOp::SmallOff, 0}, {RumbleOp::Hold, 2},
                               {RumbleOp::End, 0}};
constexpr RumbleStep kFx7[] = {{RumbleOp::LargeLit, 0x7F}, {RumbleOp::Hold, 30}, {RumbleOp::End, 0}};

}  // namespace

std::span<const RumbleStep> rumbleEffectSteps(int effect) {
    switch (effect) {
        case 0: return kFx0;
        case 1: return kFx1;
        case 2: return kFx2;
        case 3: return kFx3;
        case 4: return kFx4;
        case 5: return kFx5;
        case 6: return kFx6;
        case 7: return kFx7;
        default: return {};
    }
}

RumbleRecord assembleRumble(std::span<const RumbleStep> steps) {
    RumbleRecord r{};
    std::size_t n = 0;
    auto put = [&](std::uint8_t b) { if (n < r.size()) r[n++] = b; };
    for (const RumbleStep& s : steps) {
        switch (s.op) {
            case RumbleOp::SmallOn: put(kRumbleSmallOn); break;
            case RumbleOp::SmallOff: put(kRumbleSmallOff); break;
            case RumbleOp::LargeLit: put(kRumbleLargeLit); put(s.arg); break;
            case RumbleOp::LargeOff: put(kRumbleLargeOff); break;
            case RumbleOp::LargeStrength: put(kRumbleLargeStrength); break;
            case RumbleOp::Hold: put(s.arg); break;
            case RumbleOp::End: put(kRumbleEnd); break;
        }
    }
    return r;
}

const RumbleTable& rumbleBuiltinTable() {
    static const RumbleTable t = [] {
        RumbleTable x{};
        for (int i = 0; i < kRumbleEffects; ++i) x[static_cast<std::size_t>(i)] = assembleRumble(rumbleEffectSteps(i));
        return x;
    }();
    return t;
}

int rumbleScriptFrames(const RumbleRecord& rec) {
    int frames = 0;
    for (std::size_t pc = 0; pc < rec.size();) {
        const std::uint8_t op = rec[pc++];
        if (op == kRumbleEnd) break;
        if (op == kRumbleLargeLit) { ++pc; continue; }
        if (op >= kRumbleSmallOn && op <= kRumbleLargeStrength) continue;
        frames += op;
    }
    return frames;
}

RumbleSdl rumbleToSdl(const RumbleMotors& m) {
    return {static_cast<std::uint16_t>(m.large * 257u), static_cast<std::uint16_t>(m.small ? 0xFFFFu : 0u)};
}

RumbleInterpreter::RumbleInterpreter() : table_(rumbleBuiltinTable()) {}

void RumbleInterpreter::setGate(std::uint8_t master, std::uint8_t player0, std::uint8_t player1) {
    master_ = master;
    enable_ = {player0, player1};
}

bool RumbleInterpreter::trigger(int player, int effectId, std::int32_t strength) {
    if (player < 0 || player >= kRumblePlayers) return false;
    if ((enable_[static_cast<std::size_t>(player)] & master_) == 0) return false;  // 0x20ACC8
    if (strength == 0) return false;                                                // 0x20ACD4
    if (effectId < 0 || effectId >= kRumbleEffects) return false;                   // native bound
    if (strength >= 0x100) strength = 0xFF;                                         // 0x20ACDC/F4
    PlayerState& s = st_[static_cast<std::size_t>(player)];
    s.playing = true;  // last caller wins: unconditional overwrite
    s.effect = effectId;
    s.pc = 0;
    s.hold = 0;
    s.strength = static_cast<std::uint8_t>(strength & 0xFF);  // sb
    return true;
}

void RumbleInterpreter::stop(int player) {
    if (player < 0 || player >= kRumblePlayers) return;
    st_[static_cast<std::size_t>(player)] = PlayerState{};
    motors_[static_cast<std::size_t>(player)] = RumbleMotors{};
}

void RumbleInterpreter::tick() {
    for (int p = 0; p < kRumblePlayers; ++p) {
        PlayerState& s = st_[static_cast<std::size_t>(p)];
        RumbleMotors& m = motors_[static_cast<std::size_t>(p)];
        if (!s.playing) continue;                       // 0x20A6A8: null pointer -> skip
        if (s.hold != 0) {                              // 0x20A6B0..0x20A6C4
            --s.hold;
            if (s.hold != 0) continue;
        }
        const RumbleRecord& rec = table_[static_cast<std::size_t>(s.effect)];
        for (int guard = 0; guard < 64; ++guard) {      // 0x20A708 loop
            const std::uint8_t op = s.pc < kRumbleRecordBytes ? rec[static_cast<std::size_t>(s.pc)] : kRumbleEnd;
            if (op == kRumbleEnd) {                     // 0x20A7C8
                m = RumbleMotors{};
                s.playing = false;
                break;
            }
            if (op == kRumbleLargeLit) {                // 0x20A780
                const int lit = s.pc + 1;
                m.large = lit < kRumbleRecordBytes ? rec[static_cast<std::size_t>(lit)] : 0;
                s.pc += 2;
            } else if (op == kRumbleSmallOn) {
                m.small = true;
                ++s.pc;
            } else if (op == kRumbleSmallOff) {
                m.small = false;
                ++s.pc;
            } else if (op == kRumbleLargeOff) {
                m.large = 0;
                ++s.pc;
            } else if (op == kRumbleLargeStrength) {
                m.large = s.strength;
                ++s.pc;
            } else {                                    // hold literal (0x20A7E0)
                s.hold = op;
                ++s.pc;
                if (s.hold != 0) break;
            }
        }
    }
}

// ----------------------------------------------------------- ELF table loader

namespace {

bool rd32(std::span<const std::uint8_t> elf, std::uint32_t vaddr, std::uint32_t& v) {
    if (vaddr < kElfBase) return false;
    const std::uint64_t off = vaddr - kElfBase;
    if (off + 4 > elf.size()) return false;
    v = static_cast<std::uint32_t>(elf[off]) | (static_cast<std::uint32_t>(elf[off + 1]) << 8) |
        (static_cast<std::uint32_t>(elf[off + 2]) << 16) | (static_cast<std::uint32_t>(elf[off + 3]) << 24);
    return true;
}

}  // namespace

bool readElfAudioTables(std::span<const std::uint8_t> elf, ElfAudioTables& out) {
    for (std::size_t i = 0; i < out.files.size(); ++i) {
        const auto a = kFileTableVaddr + static_cast<std::uint32_t>(i) * 12u;
        auto& r = out.files[i];
        if (!rd32(elf, a, r.field0) || !rd32(elf, a + 4, r.lbn) || !rd32(elf, a + 8, r.sectors)) return false;
    }
    for (std::size_t i = 0; i < out.raceBgm.size(); ++i)
        if (!rd32(elf, kRaceBgmTableVaddr + static_cast<std::uint32_t>(i) * 4u, out.raceBgm[i])) return false;
    for (std::size_t i = 0; i < out.rumbleRecordPtr.size(); ++i) {
        std::uint32_t ptr = 0;
        if (!rd32(elf, kRumblePtrTableVaddr + static_cast<std::uint32_t>(i) * 4u, ptr)) return false;
        out.rumbleRecordPtr[i] = ptr;
        if (ptr != kRumbleRecordsVaddr + static_cast<std::uint32_t>(i) * kRumbleRecordBytes) return false;
        for (std::size_t b = 0; b < kRumbleRecordBytes; ++b) {
            const std::uint64_t off = static_cast<std::uint64_t>(ptr - kElfBase) + b;
            if (off >= elf.size()) return false;
            out.rumble[i][b] = elf[off];
        }
    }
    return true;
}

}  // namespace rt::native::gameplay::audio_graph
