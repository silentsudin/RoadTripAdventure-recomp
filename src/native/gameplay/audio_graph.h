// W2-09 audio runtime: voice graph with per-voice pan, BGM/SE/radio voice
// allocation, a SNDMOD-faithful TSQ sequence player, and the rumble/haptics
// opcode interpreter (0x80..0x84). Pure host C++; no SDL, no device, no guest
// memory, no ROM bytes (tables are read from the user's ELF at run time or are
// authored semantics). Guest addresses appear in comments only.
//
// Evidence (SLUS_203.98: file offset = vaddr - 0x1FF000; SNDMOD.IRX LOAD
// segment: file offset = vaddr + 0xA0). Every pin below was re-read from the
// binaries this pass; raw words are listed in .qoder/wave2/W2-09-report.md.
//   CONFIRMED (ELF):
//   - gs_music_play_bgm 0x258EC8: cmd 0xF(0), cmd 0xE(0, id), cmd 7(PP, &file
//     table[(id>>16)&0xFF]) ; table 0x29ADF0 = 29 x {0x00333388, LBN, sectors}
//     (stride 12: sll1 + addu + sll2 at 0x258F00..0x258F14).
//   - race BGM table 0x29F5B0 (18 x u32) + chooser 0x22DDD0 (mode[+0x20]==0x1A
//     -> 0x0A170201; mode[+0x22]<0x12 -> table; ==0x1C -> 0x0A160201; else
//     0x0A0D0201).
//   - PlayUiSound 0x258E50: arg = id | 0x40400000 -> SndRpcPlayOrStop 0x258308.
//   - rumbleTrigger 0x20ACB8 (gate, strength clamp, per-player ptr/strength),
//     8 effect records 0x2EA3D0 + 8n via pointer table 0x29AD60, per-frame
//     interpreter 0x20A660..0x20A878 (opcodes 0x80..0x84, 0xFF, hold literal).
//   CONFIRMED (SNDMOD.IRX, new this pass; disassembled with llvm-objdump):
//   - play() 0x5294: countdown s16 @track+0x14 is decremented every tick and the
//     stream is read only when it reaches -1; step_t 0x47F0 stores (D - 1), so a
//     delta byte D (1..0x7F) waits exactly D ticks and D == 0 stores -1 and then
//     stalls for 65535 ticks (s16 wrap). Opcodes >= 0xE0 that are not in the
//     dispatch list fall into data_end (0x4940), which stops the track.
//   - volume() 0x4F74 / panpot() 0x4DAC: volL = ((vol * panL) * master) / 256,
//     volR likewise with panR (truncating toward zero), stored as s16; when the
//     global mono flag is set both become (volL + volR) / 2.
//   - key_on 0x4BAC: freq = freqtbl[key]; if bend(+0xC) != 0, freq = freq * bend
//     >> 12. E5 key_pitch 0x4A40 stores the u16 operand as the *absolute* freq
//     and re-triggers; E4 pitch 0x5184 stores it without re-trigger; EA
//     pitch_cent 0x48BC stores the u16 as the Q12 bend multiplier (+0xC).
//   - ext_chan (F9) 0x45D4: forks a copy of the track at pc + s16 (pc taken after
//     both operand bytes), countdown 0, into the lowest-priority slot among
//     8..0x2F whose priority <= the caller's.
//   - freqtbl @ vaddr 0x8A9C: 72 x u16, floor(4096 * 2^((key-48)/12)).
//   OPEN / AUTHORED (labelled where used): E3 tempo consumes two bytes and has
//   no state effect in play() (value is not stored); SPU gain scale (vol/16383);
//   default master 0x100; fork slot table layout beyond the loop bound; voice
//   stealing policy (the original SNDMOD policy is driver-internal).
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <span>
#include <vector>

namespace rt::native::gameplay::audio_graph {

// ---------------------------------------------------------------- pan / gain

struct StereoGain {
    float l = 1.f;
    float r = 1.f;
};

// Same law as audio::PanLaw (mixer.h): centre = (1,1), hard right = (0,1).
StereoGain panLinear(float pan);
// Equal-power alternative (centre = 1/sqrt2 each). AUTHORED, offered for hosts.
StereoGain panConstantPower(float pan);
// SNDMOD E1 pan bytes, 0x40 = unity per side (center E1 40 40).
StereoGain balanceFromE1(std::uint8_t ll, std::uint8_t rr);

// Exact IRX volume()/panpot() arithmetic for one side (0x4FD8..0x500C).
std::int16_t sndmodSideVolume(std::uint8_t vol, std::uint8_t pan, std::int16_t master);
// IRX mono fold (0x507C..0x50B0): both sides = (l + r) / 2, truncating to zero.
void sndmodMonoFold(std::int16_t& l, std::int16_t& r);
inline constexpr std::int16_t kSeqMasterRef = 0x100;  // AUTHORED neutral master
inline constexpr float kSpuFullScale = 16383.f;       // AUTHORED: 0x3FFF = 1.0
inline float spuGain(std::int16_t v) { return static_cast<float>(v) / kSpuFullScale; }

inline constexpr int kFreqTableSize = 72;
// freqtbl[key] = floor(4096 * 2^((key-48)/12)); 0 for key outside 0..71.
std::uint16_t sndmodFreq(int key);
// freq * bend >> 12 with the signed-product + 0xFFF bias of the IRX (bend 0 = off).
std::uint16_t applyBend(std::uint16_t freq, std::uint16_t bend);
// Playback-rate ratio of an SPU pitch register value (0x1000 = natural rate).
inline double freqToRatio(std::uint16_t freq) { return static_cast<double>(freq) / 4096.0; }

// ----------------------------------------------------------- ids / BGM / SFX

// Packed BGM id (0x258EC8: sra 16 / sra 8 / the whole word):
//   (id >> 16) & 0xFF -> gs_music_file_table row; (id >> 8) & 0xFF -> cmd 7 PP;
//   the whole id goes to cmd 0xE. CORRECTION to audio-map.md: the byte that varies
//   across the four ROOM_1 call sites (0x0A0B0101..04) is the LOW byte (sub-song),
//   PP is 1 for ROOM_1 and 2 for every other id; so `song` = id & 0xFF (INFERRED
//   meaning: song index inside the TSQ) and PP looks like a bank-slot selector.
struct BgmId {
    std::uint32_t fileIndex = 0;
    std::uint32_t param = 0;  // PP
    std::uint32_t song = 0;   // id & 0xFF
    bool wellFormed = false;  // top byte 0x0A (every observed id)
};
BgmId unpackBgmId(std::uint32_t id);
constexpr std::uint32_t packBgmId(std::uint32_t fileIndex, std::uint32_t pp, std::uint32_t song = 1) {
    return 0x0A000000u | ((fileIndex & 0xFFu) << 16) | ((pp & 0xFFu) << 8) | (song & 0xFFu);
}

inline constexpr int kRaceBgmEntries = 18;
// FUN_0022DDD0: mode20 = mode[+0x20], mode22 = (signed) mode[+0x22]. The table
// argument is the 18 u32 ids read from the ELF (or raceBgmDefaultTable()).
std::uint32_t raceBgmId(std::uint8_t mode20, std::int8_t mode22,
                        std::span<const std::uint32_t, kRaceBgmEntries> table);
// Authored mirror of the ELF table (file indices; pp = 2, low byte 1).
const std::array<std::uint32_t, kRaceBgmEntries>& raceBgmDefaultTable();

// PlayUiSound arg and the 0x8000 stop/loop bit of tyre/horn ids (0x217C68, 0x217F10).
constexpr std::uint32_t uiSoundArg(std::uint32_t id) { return id | 0x40400000u; }
struct SfxId {
    std::uint16_t base = 0;  // id with 0x8000 cleared
    bool stop = false;       // id & 0x8000
};
SfxId decodeSfxId(std::uint32_t id);

// Radio (ApplySoundVolumes 0x2590B0: station>0 -> cmd 0x33 index station-1).
// INFERRED binding: station 1 = 1CH pair (file rows 4/5), 2 = 3CH (rows 8/9).
struct RadioRows {
    bool valid = false;
    int rowL = -1;
    int rowR = -1;
};
RadioRows radioRows(std::uint8_t station);

// ---------------------------------------------------------------- voice graph

// Pull-style mono source. `read` returns frames produced (0 = end of data).
class PcmSource {
public:
    virtual ~PcmSource() = default;
    virtual std::size_t read(std::int16_t* dst, std::size_t frames) = 0;
    virtual void rewind() = 0;
};

// Non-owning in-memory source.
class BufferSource final : public PcmSource {
public:
    BufferSource() = default;
    BufferSource(const std::int16_t* data, std::size_t frames) : data_(data), frames_(frames) {}
    std::size_t read(std::int16_t* dst, std::size_t frames) override;
    void rewind() override { pos_ = 0; }
private:
    const std::int16_t* data_ = nullptr;
    std::size_t frames_ = 0;
    std::size_t pos_ = 0;
};

enum class VoiceClass : std::uint8_t { Free, Bgm, RadioL, RadioR, Se, Seq };

// Voice pool layout (AUTHORED, sized like engine.h kMaxVoices = 24):
//   0..3 BGM, 4 radio L, 5 radio R, 6..23 SE / sequencer notes.
inline constexpr int kBgmVoices = 4;
inline constexpr int kRadioLVoice = 4;
inline constexpr int kRadioRVoice = 5;
inline constexpr int kSeVoiceStart = 6;
inline constexpr int kMaxVoices = 24;

struct VoiceParams {
    float volume = 1.f;
    float pan = 0.f;             // -1..+1
    bool loop = false;
    bool mono = false;           // ignore pan (both sides equal)
    int priority = 0;            // higher steals lower; equal steals the oldest
    std::uint32_t srcRate = 48000;  // Hz of the source PCM
    std::uint32_t pitchQ16 = 65536; // extra pitch ratio (65536 = 1.0)
    std::uint32_t tag = 0;       // BGM/SFX id for introspection
    int owner = -1;              // sequencer track id, -1 = none
};

struct VoiceView {
    VoiceClass cls = VoiceClass::Free;
    float volume = 0.f;
    float pan = 0.f;
    int priority = 0;
    std::uint32_t serial = 0;
    std::uint32_t tag = 0;
    int owner = -1;
    bool fading = false;
};

class AudioGraph {
public:
    explicit AudioGraph(std::uint32_t outRate = 48000);

    // BGM slot 0..3 owns voice index == slot. Replaces whatever plays there.
    // Per-voice pan is honoured (closes the engine.h playBgm gap).
    bool playBgm(int slot, std::uint32_t packedId, PcmSource* src, const VoiceParams& p);
    void stopBgm(int slot);

    // Stereo radio pair: L file -> voice 4 pan -1, R file -> voice 5 pan +1 (closes
    // the "stereo-right unrouted" gap). Both voices start together or not at all.
    bool playRadioPair(PcmSource* left, PcmSource* right, float volume, std::uint32_t srcRate);
    void stopRadio();

    // SE / sequencer note. Free voice with the lowest index first; otherwise steal
    // the lowest (priority, serial, index) victim when victim.priority <= p.priority,
    // else drop. Returns voice index or -1.
    int playSe(std::uint32_t id, PcmSource* src, const VoiceParams& p);
    int playNote(int owner, PcmSource* src, const VoiceParams& p);  // class Seq, one per owner
    void releaseOwner(int owner, std::uint32_t releaseFrames = 64);

    void stop(int voice);
    void stopAll();
    void setVolume(int voice, float v);
    void setPan(int voice, float pan);
    // Linear ramp: gain at frame i of the ramp = start + (target-start) * i / frames,
    // i = 0..frames; frame `frames` is exactly `target`. retire = free voice then.
    void fade(int voice, float target, std::uint32_t frames, bool retire = false);
    // Equal-length cross-fade: `from` -> 0 (retired), `to` -> 1.
    void crossfade(int from, int to, std::uint32_t frames);

    void setMasterVolume(float v) { master_ = v; }
    void setMasterMono(bool m) { masterMono_ = m; }

    // Mix `frames` stereo frames (interleaved L,R int16) into out[0 .. 2*frames).
    void mix(std::int16_t* out, std::size_t frames);

    int activeVoices() const;
    VoiceView view(int voice) const;
    std::uint32_t outRate() const { return outRate_; }
    // FNV-1a over occupancy/serial/tag/volume bits (determinism tests).
    std::uint64_t stateHash() const;

private:
    struct Voice {
        VoiceClass cls = VoiceClass::Free;
        PcmSource* src = nullptr;
        std::uint32_t srcRate = 0;
        std::uint32_t pitchQ16 = 65536;
        std::uint32_t stepQ16 = 0;
        std::uint32_t phaseQ16 = 0;
        std::int16_t prev = 0;
        std::int16_t cur = 0;
        int exhausted = 0;
        bool loop = false;
        bool mono = false;
        float volume = 1.f;
        float pan = 0.f;
        int priority = 0;
        std::uint32_t serial = 0;
        std::uint32_t tag = 0;
        int owner = -1;
        float fadeStart = 0.f, fadeTarget = 0.f;
        std::uint32_t fadeTotal = 0, fadeDone = 0;
        bool fadeRetire = false;
        std::array<std::int16_t, 256> chunk{};
        std::size_t chunkPos = 0, chunkLen = 0;
    };
    bool bind(int idx, VoiceClass cls, PcmSource* src, const VoiceParams& p);
    bool pull(Voice& v, std::int16_t& s);
    void advance(Voice& v);
    int pickSeVoice(int priority) const;

    std::array<Voice, kMaxVoices> voices_{};
    std::uint32_t outRate_;
    std::uint32_t serial_ = 0;
    float master_ = 1.f;
    bool masterMono_ = false;
};

// ------------------------------------------------------------- sequence player

enum class SeqEventKind : std::uint8_t { NoteOn, NoteOff, Pitch, Volume, Fork, End };
// End.aux: 0 = FF data_end, 1 = unknown opcode (key = opcode), 2 = pc out of range,
// 3 = runaway guard (>4096 ops in one tick, AUTHORED).
struct SeqEvent {
    std::uint32_t tick = 0;
    int track = 0;
    SeqEventKind kind = SeqEventKind::End;
    std::uint8_t key = 0;
    std::uint16_t freq = 0;
    std::int16_t volL = 0;
    std::int16_t volR = 0;
    std::uint16_t program = 0;
    std::uint8_t priority = 0;
    bool reverb = false;
    int aux = 0;  // Fork: new track index; End: reason
};

inline constexpr int kSeqTrackSlots = 0x30;  // IRX fork loop covers slots 8..0x2F
inline constexpr int kSeqTicksPerSecond = 60;

class SeqPlayer {
public:
    // `file` is the whole TSQ image (non-owning; jumps stay inside it).
    explicit SeqPlayer(std::span<const std::uint8_t> file, std::int16_t master = kSeqMasterRef)
        : file_(file), master_(master) {}

    // Start a track at file offset `pc`. Returns slot or -1. First process tick = next tick().
    int start(std::uint32_t pc, std::uint8_t channel = 0, std::uint8_t priority = 0);
    // One 1/60 s tick for every active track; events appended in track order.
    void tick(std::vector<SeqEvent>& out);
    bool active(int slot) const;
    int activeCount() const;
    std::uint32_t ticks() const { return tick_; }
    std::int16_t countdown(int slot) const { return trk_[static_cast<std::size_t>(slot)].countdown; }
    std::uint32_t pc(int slot) const { return trk_[static_cast<std::size_t>(slot)].pc; }
    void setMono(bool m) { mono_ = m; }

private:
    struct Track {
        bool active = false;
        std::uint32_t pc = 0;
        std::int16_t countdown = 0;
        std::uint8_t channel = 0;
        std::uint8_t vol = 0x7F;  // AUTHORED default until the first E0
        std::uint8_t panL = 0x40, panR = 0x40;
        std::uint8_t pri = 0;
        std::uint16_t program = 0;
        std::uint16_t bend = 0;
        std::uint16_t freq = 0;
        bool reverb = false;
        std::uint8_t revMode = 0, revDepth = 0;
        bool sounding = false;
    };
    void run(int slot, std::vector<SeqEvent>& out);
    void volumes(const Track& t, std::int16_t& l, std::int16_t& r) const;
    bool fetch(Track& t, std::uint8_t& b) const;

    std::span<const std::uint8_t> file_;
    std::int16_t master_;
    bool mono_ = false;
    std::uint32_t tick_ = 0;
    std::array<Track, kSeqTrackSlots> trk_{};
};

// Bridges SeqPlayer events to AudioGraph voices. `provider(program, key)` supplies
// the sample for a note (TVB slot lookup lives with the host).
struct SeqSample {
    PcmSource* src = nullptr;
    std::uint32_t srcRate = 44100;  // TVB samples are assumed natural 44100 Hz (tsq-tvb-format.md)
    bool loop = false;
};
class SeqSampleProvider {
public:
    virtual ~SeqSampleProvider() = default;
    virtual SeqSample sample(std::uint16_t program, std::uint8_t key) = 0;
};
void applySeqEvents(AudioGraph& g, SeqSampleProvider& prov, std::span<const SeqEvent> events);

// -------------------------------------------------------------- rumble opcodes

inline constexpr std::uint8_t kRumbleSmallOn = 0x80;
inline constexpr std::uint8_t kRumbleSmallOff = 0x81;
inline constexpr std::uint8_t kRumbleLargeLit = 0x82;  // + literal byte
inline constexpr std::uint8_t kRumbleLargeOff = 0x83;
inline constexpr std::uint8_t kRumbleLargeStrength = 0x84;
inline constexpr std::uint8_t kRumbleEnd = 0xFF;
inline constexpr int kRumbleEffects = 8;
inline constexpr int kRumbleRecordBytes = 8;
inline constexpr int kRumblePlayers = 2;

enum class RumbleOp : std::uint8_t { SmallOn, SmallOff, LargeLit, LargeOff, LargeStrength, Hold, End };
struct RumbleStep {
    RumbleOp op;
    std::uint8_t arg;  // literal for LargeLit / frames for Hold
};
using RumbleRecord = std::array<std::uint8_t, kRumbleRecordBytes>;
using RumbleTable = std::array<RumbleRecord, kRumbleEffects>;

// Authored decode (rumble-haptics.md table) assembled to the bytecode; the test
// proves it equals ELF 0x2EA3D0.. byte for byte.
std::span<const RumbleStep> rumbleEffectSteps(int effect);
RumbleRecord assembleRumble(std::span<const RumbleStep> steps);
const RumbleTable& rumbleBuiltinTable();
int rumbleScriptFrames(const RumbleRecord& rec);  // sum of hold literals up to the end op

struct RumbleMotors {
    bool small = false;       // binary high-frequency motor
    std::uint8_t large = 0;   // 0..255 low-frequency motor
    bool active() const { return small || large != 0; }
};
// SDL_RumbleGamepad arguments: low = large motor, high = small motor.
struct RumbleSdl {
    std::uint16_t low = 0;
    std::uint16_t high = 0;
};
RumbleSdl rumbleToSdl(const RumbleMotors& m);

class RumbleInterpreter {
public:
    RumbleInterpreter();
    // Gate struct @0x0177AC50: +0x0E master mask, +0x0C/+0x0D per-player enables.
    void setGate(std::uint8_t master, std::uint8_t player0, std::uint8_t player1);
    void setTable(const RumbleTable& t) { table_ = t; }
    // 0x20ACB8. Returns whether the event armed (gate open, strength != 0, id valid).
    bool trigger(int player, int effectId, std::int32_t strength);
    // 0x20A660 per-frame interpreter for both ports; one call = one 60 Hz frame.
    void tick();
    RumbleMotors motors(int player) const { return motors_[static_cast<std::size_t>(player)]; }
    bool playing(int player) const { return st_[static_cast<std::size_t>(player)].playing; }
    void stop(int player);

private:
    struct PlayerState {
        bool playing = false;
        int effect = 0;
        int pc = 0;
        std::uint8_t hold = 0;
        std::uint8_t strength = 0;
    };
    RumbleTable table_;
    std::uint8_t master_ = 0;
    std::array<std::uint8_t, kRumblePlayers> enable_{};
    std::array<PlayerState, kRumblePlayers> st_{};
    std::array<RumbleMotors, kRumblePlayers> motors_{};
};

// ----------------------------------------------------------- ELF table loader

struct ElfAudioTables {
    struct FileRow {
        std::uint32_t field0 = 0;
        std::uint32_t lbn = 0;
        std::uint32_t sectors = 0;
    };
    std::array<FileRow, 29> files{};
    std::array<std::uint32_t, kRaceBgmEntries> raceBgm{};
    std::array<std::uint32_t, kRumbleEffects> rumbleRecordPtr{};
    RumbleTable rumble{};
};
inline constexpr std::uint32_t kElfBase = 0x1FF000;  // file offset = vaddr - kElfBase
inline constexpr std::uint32_t kFileTableVaddr = 0x29ADF0;
inline constexpr std::uint32_t kRaceBgmTableVaddr = 0x29F5B0;
inline constexpr std::uint32_t kRumblePtrTableVaddr = 0x29AD60;
inline constexpr std::uint32_t kRumbleRecordsVaddr = 0x2EA3D0;
// False when a table lies outside the image or a pointer disagrees with the layout.
bool readElfAudioTables(std::span<const std::uint8_t> elf, ElfAudioTables& out);

}  // namespace rt::native::gameplay::audio_graph
