// W2-09 acceptance: audio graph (per-voice pan, voice allocation, SNDMOD-faithful
// TSQ sequence player) + rumble opcodes 0x80..0x84.
//
//   w2_audio_graph_test [--elf <SLUS_203.98>]      (or RT_ELF=<path>; RT_ELF wins)
//
// No ELF -> prints NO_ELF, still runs the pure golden checks, exits 0 when they pass.
// With an ELF the ELF/SNDMOD.IRX/TSQ pins are checked too. SNDMOD.IRX and SOUND/*.TSQ are
// looked up next to the ELF (disc directory). Exactly one `W2-09 SCORE ...` line; exit
// non-zero on any threshold miss. No ROM bytes are embedded: pins are FNV-1a hashes of
// instruction-word groups, counts and derived numbers (raw words: .qoder/wave2/W2-09-report.md).

#include "assets/music_seq.h"
#include "audio/mixer.h"
#include "gameplay/audio_graph.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <deque>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>

namespace ag = rt::native::gameplay::audio_graph;
namespace fs = std::filesystem;

namespace {

struct Tally {
    int pass = 0, total = 0;
    void check(bool ok, const char* what) {
        ++total;
        if (ok) ++pass;
        else std::fprintf(stderr, "FAIL: %s\n", what);
    }
};

std::uint64_t fnv(std::uint64_t h, std::uint64_t v) {
    for (int i = 0; i < 8; ++i) { h ^= (v >> (8 * i)) & 0xFFu; h *= 1099511628211ull; }
    return h;
}
std::uint64_t fnvStr(std::uint64_t h, const std::string& s) {
    for (unsigned char c : s) { h ^= c; h *= 1099511628211ull; }
    return h;
}

std::vector<std::uint8_t> readFile(const fs::path& p) {
    std::ifstream f(p, std::ios::binary);
    if (!f) return {};
    return std::vector<std::uint8_t>(std::istreambuf_iterator<char>(f), {});
}

// ------------------------------------------------------------ sequence vectors

std::string fmtEvent(const ag::SeqEvent& e) {
    char b[160];
    switch (e.kind) {
        case ag::SeqEventKind::NoteOn:
            std::snprintf(b, sizeof b, "%u s%d on k%u f%u %d/%d p%u pri%u r%d", e.tick, e.track, e.key, e.freq,
                          e.volL, e.volR, e.program, e.priority, e.reverb ? 1 : 0);
            break;
        case ag::SeqEventKind::NoteOff: std::snprintf(b, sizeof b, "%u s%d off", e.tick, e.track); break;
        case ag::SeqEventKind::Pitch: std::snprintf(b, sizeof b, "%u s%d pitch %u", e.tick, e.track, e.freq); break;
        case ag::SeqEventKind::Volume:
            std::snprintf(b, sizeof b, "%u s%d vol %d/%d", e.tick, e.track, e.volL, e.volR);
            break;
        case ag::SeqEventKind::Fork: std::snprintf(b, sizeof b, "%u s%d fork %d", e.tick, e.track, e.aux); break;
        case ag::SeqEventKind::End:
            if (e.aux == 1) std::snprintf(b, sizeof b, "%u s%d end 1 op%02X", e.tick, e.track, e.key);
            else std::snprintf(b, sizeof b, "%u s%d end %d", e.tick, e.track, e.aux);
            break;
    }
    return b;
}

std::string runSeq(const std::vector<std::uint8_t>& bytes, int ticks, std::int16_t master = ag::kSeqMasterRef,
                   bool mono = false) {
    ag::SeqPlayer p(bytes, master);
    p.setMono(mono);
    p.start(0);
    std::vector<ag::SeqEvent> ev;
    for (int i = 0; i < ticks; ++i) p.tick(ev);
    std::string s;
    for (const auto& e : ev) {
        if (!s.empty()) s += "; ";
        s += fmtEvent(e);
    }
    return s;
}

struct SeqVec {
    const char* name;
    std::vector<std::uint8_t> bytes;
    int ticks;
    const char* expect;
};

void seqVectors(Tally& t, std::uint64_t& h) {
    const std::vector<SeqVec> v = {
        {"basic phrase", {0xE0, 0xB4, 0xE1, 0x40, 0x40, 0xE2, 0x01, 0xA1, 0x64, 0xF0, 0xFF}, 120,
         "0 s0 on k33 f1722 11520/11520 p1 pri0 r0; 100 s0 off; 100 s0 end 0"},
        {"delta 3 = 3 ticks", {0xA1, 0x03, 0xF0, 0xFF}, 10,
         "0 s0 on k33 f1722 8128/8128 p0 pri0 r0; 3 s0 off; 3 s0 end 0"},
        {"delta 1 = 1 tick", {0xA1, 0x01, 0xF0, 0xFF}, 5,
         "0 s0 on k33 f1722 8128/8128 p0 pri0 r0; 1 s0 off; 1 s0 end 0"},
        {"delta 0 stalls 65535 ticks", {0xA1, 0x00, 0xF0, 0xFF}, 100,
         "0 s0 on k33 f1722 8128/8128 p0 pri0 r0"},
        {"delta 0 resumes at 65536", {0xA1, 0x00, 0xF0, 0xFF}, 65537,
         "0 s0 on k33 f1722 8128/8128 p0 pri0 r0; 65536 s0 off; 65536 s0 end 0"},
        {"FF only", {0xFF}, 2, "0 s0 end 0"},
        {"unknown opcode EB = data_end", {0xEB}, 2, "0 s0 end 1 opEB"},
        {"unknown opcode FA = data_end", {0xFA}, 2, "0 s0 end 1 opFA"},
        {"truncated note then eof", {0xA1}, 2,
         "0 s0 on k33 f1722 8128/8128 p0 pri0 r0; 0 s0 off; 0 s0 end 2"},
        {"EA bend 0.5 scales freq", {0xEA, 0x00, 0x08, 0xA1, 0x01, 0xFF}, 3,
         "0 s0 on k33 f861 8128/8128 p0 pri0 r0; 1 s0 off; 1 s0 end 0"},
        {"E4 pitch no retrigger", {0xA1, 0x01, 0xE4, 0x00, 0x10, 0xF0, 0xFF}, 4,
         "0 s0 on k33 f1722 8128/8128 p0 pri0 r0; 1 s0 pitch 4096; 1 s0 off; 1 s0 end 0"},
        {"E5 absolute pitch retriggers", {0xE5, 0x00, 0x10, 0x01, 0xF0, 0xFF}, 3,
         "0 s0 on k0 f4096 8128/8128 p0 pri0 r0; 1 s0 off; 1 s0 end 0"},
        {"E1 hard balance flip", {0xE0, 0xB4, 0xE1, 0x40, 0x00, 0xE2, 0x01, 0xA1, 0x02, 0xE1, 0x00, 0x40, 0xF0, 0xFF}, 4,
         "0 s0 on k33 f1722 11520/0 p1 pri0 r0; 2 s0 vol 0/11520; 2 s0 off; 2 s0 end 0"},
        {"E0 while sounding", {0xA1, 0x01, 0xE0, 0x40, 0xF0, 0xFF}, 3,
         "0 s0 on k33 f1722 8128/8128 p0 pri0 r0; 1 s0 vol 4096/4096; 1 s0 off; 1 s0 end 0"},
        {"E8 reverb on, E9 off, E6/E7 1 operand",
         {0xE8, 0xA1, 0x01, 0xE9, 0xE6, 0x05, 0xE7, 0x09, 0xF0, 0xFF}, 3,
         "0 s0 on k33 f1722 8128/8128 p0 pri0 r1; 1 s0 off; 1 s0 end 0"},
        {"E3 tempo eats 2 bytes", {0xE3, 0x12, 0x34, 0xA1, 0x01, 0xF0, 0xFF}, 3,
         "0 s0 on k33 f1722 8128/8128 p0 pri0 r0; 1 s0 off; 1 s0 end 0"},
        {"F1 priority", {0xF1, 0x05, 0xA1, 0x01, 0xF0, 0xFF}, 3,
         "0 s0 on k33 f1722 8128/8128 p0 pri5 r0; 1 s0 off; 1 s0 end 0"},
        {"F8 backward loop", {0xA1, 0x02, 0xF0, 0x01, 0xF8, 0xF9, 0xFF}, 9,
         "0 s0 on k33 f1722 8128/8128 p0 pri0 r0; 2 s0 off; 3 s0 on k33 f1722 8128/8128 p0 pri0 r0; "
         "5 s0 off; 6 s0 on k33 f1722 8128/8128 p0 pri0 r0; 8 s0 off"},
        {"F9 fork runs in the same tick", {0xF9, 0x01, 0x00, 0xFF, 0xA1, 0x01, 0xF0, 0xFF}, 3,
         "0 s0 fork 47; 0 s0 end 0; 0 s47 on k33 f1722 8128/8128 p0 pri0 r0; 1 s47 off; 1 s47 end 0"},
        {"E2 sign-extends the operand", {0xE2, 0x80, 0xA1, 0x01, 0xFF}, 2,
         "0 s0 on k33 f1722 8128/8128 p65408 pri0 r0; 1 s0 off; 1 s0 end 0"},
        {"mono fold", {0xE0, 0xB4, 0xE1, 0x40, 0x00, 0xA1, 0x01, 0xFF}, 2,
         "0 s0 on k33 f1722 5760/5760 p0 pri0 r0; 1 s0 off; 1 s0 end 0"},
        {"key 0 freq 256 / key 71 freq 15464 (new note, no implicit off)", {0x80, 0x01, 0xC7, 0x01, 0xFF}, 3,
         "0 s0 on k0 f256 8128/8128 p0 pri0 r0; 1 s0 on k71 f15464 8128/8128 p0 pri0 r0; "
         "2 s0 off; 2 s0 end 0"},
    };
    for (std::size_t i = 0; i < v.size(); ++i) {
        const bool mono = std::string(v[i].name) == "mono fold";
        std::string got = runSeq(v[i].bytes, v[i].ticks, ag::kSeqMasterRef, mono);
        t.check(got == v[i].expect, v[i].name);
        if (got != v[i].expect) std::fprintf(stderr, "  got:    %s\n  expect: %s\n", got.c_str(), v[i].expect);
        h = fnvStr(h, got);
    }
    // Explicit master 0x80: (255*64)*128 >> 8 = 8160 per side.
    {
        std::string got = runSeq({0xE0, 0xFF, 0xA1, 0x01, 0xFF}, 2, 0x80);
        t.check(got == "0 s0 on k33 f1722 8160/8160 p0 pri0 r0; 1 s0 off; 1 s0 end 0", "master 0x80");
        h = fnvStr(h, got);
    }
    // Two tracks keep independent state and event order is by slot.
    {
        const std::vector<std::uint8_t> bytes = {0xA1, 0x01, 0xF0, 0xFF};
        ag::SeqPlayer p(bytes);
        p.start(0);
        p.start(0);
        std::vector<ag::SeqEvent> ev;
        p.tick(ev);
        t.check(ev.size() == 2 && ev[0].track == 0 && ev[1].track == 1, "two tracks ordered by slot");
        h = fnv(h, ev.size());
    }
}

// ---------------------------------------------------------------- rumble vectors

std::string rumbleTrace(int effect, int strength, int frames, ag::RumbleInterpreter* in = nullptr) {
    ag::RumbleInterpreter local;
    ag::RumbleInterpreter& r = in ? *in : local;
    if (!in) r.setGate(2, 2, 2);
    r.trigger(0, effect, strength);
    std::string s;
    for (int i = 0; i < frames; ++i) {
        r.tick();
        const auto m = r.motors(0);
        char b[24];
        std::snprintf(b, sizeof b, "%s%d/%d", i ? " " : "", m.small ? 1 : 0, m.large);
        s += b;
    }
    return s;
}

void rumbleVectors(Tally& t, std::uint64_t& h) {
    struct V { int fx, str, frames; const char* expect; };
    const V v[] = {
        {0, 200, 2, "0/0 0/0"},
        {1, 200, 10, "1/200 1/200 0/200 0/200 0/200 0/200 0/200 0/200 0/0 0/0"},
        {2, 200, 6, "1/200 1/200 1/200 1/200 0/0 0/0"},
        {3, 200, 5, "1/0 0/255 0/255 0/0 0/0"},
        {4, 200, 4, "0/255 1/255 0/0 0/0"},
        {5, 200, 4, "1/255 1/255 0/0 0/0"},
        {6, 64, 10, "0/64 0/64 0/64 0/64 1/64 1/64 0/64 0/64 0/0 0/0"},
        {1, 300, 1, "1/255"},
        {1, 256, 1, "1/255"},
        {1, 255, 1, "1/255"},
        {1, 1, 1, "1/1"},
        {1, -1, 1, "1/255"},
    };
    for (const V& x : v) {
        const std::string got = rumbleTrace(x.fx, x.str, x.frames);
        t.check(got == x.expect, "rumble effect trace");
        if (got != x.expect) std::fprintf(stderr, "  fx%d got: %s\n  expect: %s\n", x.fx, got.c_str(), x.expect);
        h = fnvStr(h, got);
    }
    {  // effect 7: 30 frames of 127 then silence
        ag::RumbleInterpreter r;
        r.setGate(2, 2, 2);
        r.trigger(0, 7, 1);
        bool ok = true;
        for (int i = 1; i <= 31; ++i) {
            r.tick();
            const auto m = r.motors(0);
            ok = ok && (i <= 30 ? (!m.small && m.large == 127) : (!m.small && m.large == 0));
        }
        t.check(ok && !r.playing(0), "rumble effect 7 holds 30 frames then ends");
    }
    {  // totals per effect (script frames): doc table 0/8/4/3/2/2/8/30
        const int want[8] = {0, 8, 4, 3, 2, 2, 8, 30};
        bool ok = true;
        for (int i = 0; i < 8; ++i) ok = ok && ag::rumbleScriptFrames(ag::rumbleBuiltinTable()[static_cast<std::size_t>(i)]) == want[i];
        t.check(ok, "rumble script frame totals");
    }
    {  // gate arithmetic 0x20ACC8: (player & master) != 0
        ag::RumbleInterpreter r;
        r.setGate(2, 1, 1);
        t.check(!r.trigger(0, 1, 100), "gate closed when masks disjoint");
        r.setGate(0, 0xFF, 0xFF);
        t.check(!r.trigger(0, 1, 100), "gate closed when master off");
        r.setGate(4, 4, 0);
        t.check(r.trigger(0, 1, 100) && !r.trigger(1, 1, 100), "per-player enable byte");
        r.setGate(2, 2, 2);
        t.check(!r.trigger(0, 1, 0), "strength 0 is silence");
        t.check(!r.trigger(0, 8, 10) && !r.trigger(2, 1, 10), "native bounds: effect >= 8, player >= 2");
    }
    {  // last caller wins, hold counter reset, ports independent
        ag::RumbleInterpreter r;
        r.setGate(2, 2, 2);
        r.trigger(0, 1, 10);
        r.trigger(0, 5, 10);
        std::string s = rumbleTrace(5, 10, 3, &r);  // re-trigger again then 3 frames
        t.check(s == "1/255 1/255 0/0", "overwrite replays from start");
        ag::RumbleInterpreter q;
        q.setGate(2, 2, 2);
        q.trigger(0, 7, 1);
        for (int i = 0; i < 3; ++i) q.tick();
        q.trigger(0, 5, 1);
        q.tick();
        t.check(q.motors(0).small && q.motors(0).large == 255, "mid-play trigger restarts");
        q.trigger(1, 2, 77);
        q.tick();
        t.check(q.motors(1).large == 77 && q.motors(1).small, "port 1 independent");
        h = fnv(h, q.motors(1).large);
    }
    {
        t.check(ag::rumbleToSdl({true, 200}).low == 51400 && ag::rumbleToSdl({true, 200}).high == 0xFFFF &&
                    ag::rumbleToSdl({false, 255}).low == 0xFFFF && ag::rumbleToSdl({false, 0}).high == 0,
                "SDL motor mapping");
    }
}

// ------------------------------------------------------------------ pan / math

int panVectors(Tally& t) {
    int n = 0;
    auto eq = [&](float a, float b, float tol, const char* w) {
        t.check(std::fabs(a - b) <= tol, w);
        ++n;
    };
    auto g = [](float p) { return ag::panLinear(p); };
    eq(g(0.f).l, 1.f, 0, "pan 0 L");
    eq(g(0.f).r, 1.f, 0, "pan 0 R");
    eq(g(1.f).l, 0.f, 0, "pan +1 L");
    eq(g(1.f).r, 1.f, 0, "pan +1 R");
    eq(g(-1.f).l, 1.f, 0, "pan -1 L");
    eq(g(-1.f).r, 0.f, 0, "pan -1 R");
    eq(g(0.5f).l, 0.5f, 0, "pan +.5 L");
    eq(g(-0.25f).r, 0.75f, 0, "pan -.25 R");
    eq(g(2.f).l, 0.f, 0, "pan clamp");
    eq(ag::panConstantPower(0.f).l, 0.70710678f, 1e-6f, "const power centre L");
    eq(ag::panConstantPower(0.f).r, 0.70710678f, 1e-6f, "const power centre R");
    eq(ag::panConstantPower(-1.f).l, 1.f, 1e-6f, "const power hard L");
    eq(ag::panConstantPower(1.f).r, 1.f, 1e-6f, "const power hard R");
    eq(ag::panConstantPower(1.f).l, 0.f, 1e-6f, "const power hard R mutes L");
    eq(ag::balanceFromE1(0x40, 0x00).l, 1.f, 0, "E1 40 00 L");
    eq(ag::balanceFromE1(0x40, 0x00).r, 0.f, 0, "E1 40 00 R");
    eq(ag::balanceFromE1(0x20, 0x20).l, 0.5f, 0, "E1 20 20");
    // Bit-compat with the shipped mixer law on a 41-point sweep.
    bool same = true;
    for (int i = -20; i <= 20; ++i) {
        const float p = static_cast<float>(i) / 20.f;
        same = same && g(p).l == rt::native::audio::PanLaw::left(p) && g(p).r == rt::native::audio::PanLaw::right(p);
    }
    t.check(same, "panLinear == audio::PanLaw sweep");
    ++n;
    return n;
}

int sndmodMath(Tally& t) {
    int n = 0;
    auto eq = [&](long a, long b, const char* w) { t.check(a == b, w); ++n; };
    eq(ag::sndmodSideVolume(180, 64, 256), 11520, "vol 180 pan 64");
    eq(ag::sndmodSideVolume(255, 64, 256), 16320, "vol 255 pan 64");
    eq(ag::sndmodSideVolume(127, 64, 0x80), 4064, "vol 127 master 0x80");
    eq(ag::sndmodSideVolume(1, 1, -1), 0, "negative truncates to zero");
    eq(ag::sndmodSideVolume(3, 1, -100), -1, "negative -300/256 -> -1");
    eq(ag::sndmodSideVolume(180, 0, 256), 0, "pan 0 mutes");
    std::int16_t l = 11520, r = 0;
    ag::sndmodMonoFold(l, r);
    eq(l, 5760, "mono fold L");
    eq(r, 5760, "mono fold R");
    l = -3; r = 0;
    ag::sndmodMonoFold(l, r);
    eq(l, -1, "mono fold negative truncates");
    l = 5; r = 2;
    ag::sndmodMonoFold(l, r);
    eq(l, 3, "mono fold odd");
    eq(ag::sndmodFreq(48), 4096, "freq key 48");
    eq(ag::sndmodFreq(0), 256, "freq key 0");
    eq(ag::sndmodFreq(12), 512, "freq key 12");
    eq(ag::sndmodFreq(60), 8192, "freq key 60");
    eq(ag::sndmodFreq(71), 15464, "freq key 71");
    eq(ag::sndmodFreq(72), 0, "freq key 72 out of table");
    eq(ag::sndmodFreq(33), 1722, "freq key 33");
    eq(ag::applyBend(1722, 0x0800), 861, "bend 0.5");
    eq(ag::applyBend(1722, 0), 1722, "bend off");
    eq(ag::applyBend(4096, 0x1000), 4096, "bend unity");
    return n;
}

void idVectors(Tally& t) {
    const auto a = ag::unpackBgmId(0x0A180201);
    t.check(a.fileIndex == 24 && a.param == 2 && a.song == 1 && a.wellFormed, "unpack REPLAY id");
    const auto b = ag::unpackBgmId(0x0A0B0103);
    t.check(b.fileIndex == 11 && b.param == 1 && b.song == 3 && b.wellFormed, "unpack ROOM_1 sub-song 3");
    t.check(!ag::unpackBgmId(0x0B180201).wellFormed, "top byte must be 0x0A");
    t.check(ag::packBgmId(24, 2) == 0x0A180201 && ag::packBgmId(13, 2) == 0x0A0D0201 && ag::packBgmId(11, 1, 3) == 0x0A0B0103, "pack ids");
    const auto& tab = ag::raceBgmDefaultTable();
    std::span<const std::uint32_t, ag::kRaceBgmEntries> sp(tab);
    t.check(ag::raceBgmId(0x1A, 0, sp) == 0x0A170201, "mode20 0x1A -> BGM_12");
    t.check(ag::raceBgmId(0x1A, 5, sp) == 0x0A170201, "mode20 0x1A wins");
    t.check(ag::raceBgmId(0, 0, sp) == 0x0A0C0201 && ag::raceBgmId(0, 17, sp) == 0x0A0E0201, "table ends");
    t.check(ag::raceBgmId(0, 8, sp) == 0x0A110201 && ag::raceBgmId(0, 11, sp) == 0x0A120201, "table mid");
    t.check(ag::raceBgmId(0, 18, sp) == 0x0A0D0201 && ag::raceBgmId(0, 0x1C, sp) == 0x0A160201, "fallbacks");
    t.check(ag::raceBgmId(0, -1, sp) == 0x0A0D0201, "negative mode22 fallback");
    t.check(ag::uiSoundArg(0x1A) == 0x4040001A, "PlayUiSound arg");
    t.check(ag::decodeSfxId(0x8035).stop && ag::decodeSfxId(0x8035).base == 0x35 && !ag::decodeSfxId(0x1A).stop,
            "sfx stop bit");
    t.check(!ag::radioRows(0).valid && ag::radioRows(1).rowL == 4 && ag::radioRows(1).rowR == 5 &&
                ag::radioRows(2).rowL == 8 && ag::radioRows(2).rowR == 9 && !ag::radioRows(3).valid,
            "radio rows");
}

// ----------------------------------------------------------------- graph tests

struct Dc {
    std::vector<std::int16_t> d;
    ag::BufferSource src;
    Dc(std::int16_t v, std::size_t n) : d(n, v), src(d.data(), n) {}
};

int graphTests(Tally& t, std::uint64_t& h) {
    int n = 0;
    auto chk = [&](bool ok, const char* w) { t.check(ok, w); ++n; };
    {  // radio stereo pair routing (closes "stereo-right unrouted")
        ag::AudioGraph g(12000);
        Dc l(1000, 64), r(-2000, 64);
        chk(g.playRadioPair(&l.src, &r.src, 1.f, 12000), "radio pair starts");
        std::vector<std::int16_t> out(2 * 40);
        g.mix(out.data(), 40);
        bool ok = true;
        for (int i = 0; i < 40; ++i) ok = ok && out[2 * i] == 1000 && out[2 * i + 1] == -2000;
        chk(ok, "radio L file only left, R file only right");
        chk(g.view(ag::kRadioLVoice).pan == -1.f && g.view(ag::kRadioRVoice).pan == 1.f, "radio pans -1/+1");
        g.stopRadio();
        chk(g.activeVoices() == 0, "stopRadio frees both");
        Dc l2(1000, 8), r2(-2000, 8);
        g.playRadioPair(&l2.src, &r2.src, 0.5f, 12000);
        g.mix(out.data(), 2);
        chk(out[0] == 500 && out[1] == -1000, "radio volume 0.5");
        h = fnv(h, static_cast<std::uint64_t>(out[0]));
    }
    {  // looping radio keeps playing past the buffer end
        ag::AudioGraph g(12000);
        Dc l(300, 10), r(400, 10);
        g.playRadioPair(&l.src, &r.src, 1.f, 12000);
        std::vector<std::int16_t> out(2 * 100);
        g.mix(out.data(), 100);
        bool ok = true;
        for (int i = 0; i < 100; ++i) ok = ok && out[2 * i] == 300 && out[2 * i + 1] == 400;
        chk(ok && g.activeVoices() == 2, "radio loops");
    }
    {  // per-voice BGM pan (closes engine playBgm gap)
        ag::AudioGraph g(12000);
        Dc s(1000, 16);
        ag::VoiceParams p;
        p.srcRate = 12000;
        p.pan = 0.5f;
        chk(g.playBgm(0, 0x0A0D0201, &s.src, p), "bgm starts");
        std::int16_t out[4];
        g.mix(out, 2);
        chk(out[0] == 500 && out[1] == 1000, "bgm per-voice pan");
        chk(g.view(0).tag == 0x0A0D0201 && g.view(0).cls == ag::VoiceClass::Bgm, "bgm voice tag");
        chk(!g.playBgm(4, 1, &s.src, p) && !g.playBgm(-1, 1, &s.src, p), "bgm slot bound");
        g.stopBgm(0);
        chk(g.activeVoices() == 0, "stopBgm");
    }
    {  // identity resample is exact and the voice retires after N frames
        ag::AudioGraph g(48000);
        std::vector<std::int16_t> ramp(37);
        for (std::size_t i = 0; i < ramp.size(); ++i) ramp[i] = static_cast<std::int16_t>(100 * (i + 1));
        ag::BufferSource s(ramp.data(), ramp.size());
        ag::VoiceParams p;
        p.srcRate = 48000;
        g.playSe(0x1A, &s, p);
        std::vector<std::int16_t> out(2 * 40);
        g.mix(out.data(), 40);
        bool ok = true;
        for (std::size_t i = 0; i < ramp.size(); ++i) ok = ok && out[2 * i] == ramp[i] && out[2 * i + 1] == ramp[i];
        for (std::size_t i = ramp.size(); i < 40; ++i) ok = ok && out[2 * i] == 0;
        chk(ok && g.activeVoices() == 0, "identity resample exact + retire");
    }
    {  // 4x upsample: frame 4k = sample k; interpolation linear in between
        ag::AudioGraph g(48000);
        const std::int16_t src[4] = {0, 400, 800, 1200};
        ag::BufferSource s(src, 4);
        ag::VoiceParams p;
        p.srcRate = 12000;
        g.playSe(1, &s, p);
        std::vector<std::int16_t> out(2 * 12);
        g.mix(out.data(), 12);
        bool ok = true;
        for (int k = 0; k < 3; ++k) {
            ok = ok && out[2 * (4 * k)] == src[k];
            ok = ok && out[2 * (4 * k + 2)] == src[k] + 200;
        }
        chk(ok, "4x upsample linear");
    }
    {  // pitch ratio via E4 freq: freq 0x2000 = 2x speed
        ag::AudioGraph g(48000);
        std::vector<std::int16_t> ramp(8);
        for (std::size_t i = 0; i < 8; ++i) ramp[i] = static_cast<std::int16_t>(100 * i);
        ag::BufferSource s(ramp.data(), 8);
        ag::VoiceParams p;
        p.srcRate = 48000;
        p.pitchQ16 = static_cast<std::uint32_t>(ag::freqToRatio(0x2000) * 65536.0);
        g.playSe(2, &s, p);
        std::vector<std::int16_t> out(2 * 5);
        g.mix(out.data(), 5);
        chk(out[0] == 0 && out[2] == 200 && out[4] == 400 && out[6] == 600, "2x pitch skips samples");
    }
    {  // fade exact length
        ag::AudioGraph g(48000);
        Dc s(10000, 1000);
        ag::VoiceParams p;
        g.playSe(3, &s.src, p);
        g.fade(ag::kSeVoiceStart, 0.f, 100);
        std::vector<std::int16_t> out(2 * 102);
        g.mix(out.data(), 102);
        bool mono = true;
        for (int i = 1; i < 102; ++i) mono = mono && out[2 * i] <= out[2 * (i - 1)];
        chk(out[0] == 10000 && out[2 * 50] == 5000 && out[2 * 99] > 0 && out[2 * 100] == 0 && out[2 * 101] == 0 && mono,
            "fade hits target at exactly frame N");
        chk(g.view(ag::kSeVoiceStart).volume == 0.f && !g.view(ag::kSeVoiceStart).fading, "fade settled");
    }
    {  // crossfade: two DC sources, equal length, constant sum, tiny steps
        ag::AudioGraph g(48000);
        Dc a(8000, 4000), b(8000, 4000);
        ag::VoiceParams p;
        p.volume = 1.f;
        const int va = g.playSe(10, &a.src, p);
        p.volume = 0.f;
        const int vb = g.playSe(11, &b.src, p);
        g.crossfade(va, vb, 480);
        std::vector<std::int16_t> out(2 * 481);
        g.mix(out.data(), 480);
        const bool bStillRamping = g.view(vb).volume < 1.f && g.view(va).cls != ag::VoiceClass::Free;
        g.mix(out.data() + 2 * 480, 1);
        int maxDev = 0, maxStep = 0;
        for (int i = 0; i < 481; ++i) maxDev = std::max(maxDev, std::abs(out[2 * i] - 8000));
        for (int i = 1; i < 481; ++i) maxStep = std::max(maxStep, std::abs(out[2 * i] - out[2 * (i - 1)]));
        chk(maxDev <= 1, "crossfade gains sum to constant");
        chk(static_cast<double>(maxStep) / 32768.0 <= 0.02, "crossfade peak step <= 0.02 FS");
        chk(bStillRamping && g.view(vb).volume == 1.f && g.view(va).cls == ag::VoiceClass::Free,
            "crossfade length exact (480 frames, retire at frame 480)");
        h = fnv(h, static_cast<std::uint64_t>(maxDev));
    }
    {  // global mono and per-voice mono ignore pan
        ag::AudioGraph g(48000);
        Dc s(1000, 8);
        ag::VoiceParams p;
        p.pan = 1.f;
        p.mono = true;
        g.playSe(5, &s.src, p);
        std::int16_t out[2];
        g.mix(out, 1);
        chk(out[0] == 500 && out[1] == 500, "mono voice: 0.5*(L+R)");
        ag::AudioGraph g2(48000);
        Dc s2(1000, 8);
        p.mono = false;
        g2.setMasterMono(true);
        g2.playSe(5, &s2.src, p);
        g2.mix(out, 1);
        chk(out[0] == 500 && out[1] == 500, "master mono");
    }
    {  // clipping
        ag::AudioGraph g(48000);
        Dc a(30000, 8), b(30000, 8);
        ag::VoiceParams p;
        g.playSe(1, &a.src, p);
        g.playSe(1, &b.src, p);
        std::int16_t out[2];
        g.mix(out, 1);
        chk(out[0] == 32767, "mix clamps to int16");
    }
    {  // SE priority / stealing policy
        ag::AudioGraph g(48000);
        std::deque<Dc> pool;
        ag::VoiceParams p;
        p.priority = 1;
        for (int i = 0; i < 18; ++i) {
            pool.emplace_back(1, 100000);
            chk(g.playSe(static_cast<std::uint32_t>(i), &pool.back().src, p) == ag::kSeVoiceStart + i, "fill order");
        }
        pool.emplace_back(1, 100000);
        ag::VoiceParams low;
        low.priority = 0;
        chk(g.playSe(99, &pool.back().src, low) == -1, "lower priority dropped when full");
        pool.emplace_back(1, 100000);
        chk(g.playSe(100, &pool.back().src, p) == ag::kSeVoiceStart, "equal priority steals oldest");
        pool.emplace_back(1, 100000);
        ag::VoiceParams hi;
        hi.priority = 5;
        chk(g.playSe(101, &pool.back().src, hi) == ag::kSeVoiceStart + 1, "higher priority steals next oldest");
        pool.emplace_back(1, 100000);
        chk(g.playSe(102, &pool.back().src, p) == ag::kSeVoiceStart + 2, "stolen prio-5 voice is protected");
        g.stopAll();
        chk(g.activeVoices() == 0, "stopAll");
    }
    {  // sequencer bridge: one voice per owner, release ramps out
        struct Prov final : ag::SeqSampleProvider {
            Dc d{5000, 100000};
            ag::SeqSample sample(std::uint16_t, std::uint8_t) override { return {&d.src, 48000, true}; }
        } prov;
        ag::AudioGraph g(48000);
        const std::vector<std::uint8_t> bytes = {0xE0, 0xB4, 0xE1, 0x40, 0x40, 0xA1, 0x04, 0xF0, 0xFF};
        ag::SeqPlayer sp(bytes);
        sp.start(0);
        std::vector<ag::SeqEvent> ev;
        sp.tick(ev);
        ag::applySeqEvents(g, prov, ev);
        const int v = ag::kSeVoiceStart;
        chk(g.view(v).cls == ag::VoiceClass::Seq && g.view(v).owner == 0, "note-on allocates a Seq voice");
        chk(std::fabs(g.view(v).volume - 11520.f / 16383.f) < 1e-5f && g.view(v).pan == 0.f, "note gain from IRX volume");
        ev.clear();
        ag::applySeqEvents(g, prov, std::span<const ag::SeqEvent>{});
        ag::SeqEvent again;
        again.kind = ag::SeqEventKind::NoteOn;
        again.track = 0;
        again.freq = 4096;
        again.volL = again.volR = 8000;
        std::vector<ag::SeqEvent> two = {again};
        ag::applySeqEvents(g, prov, two);
        chk(g.activeVoices() == 1, "re-trigger replaces the owner's voice");
        ag::SeqEvent off;
        off.kind = ag::SeqEventKind::NoteOff;
        off.track = 0;
        std::vector<ag::SeqEvent> offs = {off};
        ag::applySeqEvents(g, prov, offs);
        std::vector<std::int16_t> out(2 * 70);
        g.mix(out.data(), 70);
        chk(g.activeVoices() == 0, "note-off releases within 64 frames");
    }
    return n;
}

// ------------------------------------------------------------- allocation test

struct AllocResult {
    std::uint64_t hash = 0;
    int refMatch = 0, refTotal = 0;
};

AllocResult allocRun(std::uint32_t seed) {
    AllocResult res;
    res.hash = 1469598103934665603ull;
    ag::AudioGraph g(48000);
    std::deque<Dc> pool;
    struct Slot { bool used = false; int prio = 0; std::uint32_t serial = 0; int remaining = 0; };
    std::array<Slot, ag::kMaxVoices> ref{};
    std::uint32_t serial = 0;
    std::uint32_t s = seed;
    auto rnd = [&s]() {  // fixed xorshift32: platform independent
        s ^= s << 13; s ^= s >> 17; s ^= s << 5;
        return s;
    };
    std::vector<std::int16_t> out(2 * 512);
    for (int op = 0; op < 3000; ++op) {
        if (rnd() % 3 != 0) {
            const int prio = static_cast<int>(rnd() % 4);
            const int len = 1 + static_cast<int>(rnd() % 900);
            pool.emplace_back(1, static_cast<std::size_t>(len));
            if (pool.size() > 64) pool.pop_front();  // sources are consumed fully before reuse? keep alive via deque cap
            ag::VoiceParams p;
            p.priority = prio;
            p.srcRate = 48000;
            const int got = g.playSe(static_cast<std::uint32_t>(op), &pool.back().src, p);
            // reference policy
            int expect = -1, victim = -1;
            for (int i = ag::kSeVoiceStart; i < ag::kMaxVoices; ++i) {
                if (!ref[static_cast<std::size_t>(i)].used) { expect = i; break; }
                if (victim < 0 || ref[static_cast<std::size_t>(i)].prio < ref[static_cast<std::size_t>(victim)].prio ||
                    (ref[static_cast<std::size_t>(i)].prio == ref[static_cast<std::size_t>(victim)].prio &&
                     ref[static_cast<std::size_t>(i)].serial < ref[static_cast<std::size_t>(victim)].serial))
                    victim = i;
            }
            if (expect < 0 && victim >= 0 && ref[static_cast<std::size_t>(victim)].prio <= prio) expect = victim;
            ++res.refTotal;
            if (got == expect) ++res.refMatch;
            if (expect >= 0) ref[static_cast<std::size_t>(expect)] = {true, prio, ++serial, len};
            res.hash = fnv(res.hash, static_cast<std::uint64_t>(got + 1));
        } else {
            const int frames = 1 + static_cast<int>(rnd() % 512);
            g.mix(out.data(), static_cast<std::size_t>(frames));
            for (auto& r : ref)
                if (r.used && (r.remaining -= frames) <= 0) r = Slot{};
            res.hash = fnv(res.hash, g.stateHash());
        }
    }
    return res;
}

// -------------------------------------------------------------------- ELF pins

std::uint32_t wordAt(const std::vector<std::uint8_t>& b, std::uint32_t off) {
    if (static_cast<std::uint64_t>(off) + 4 > b.size()) return 0;
    return static_cast<std::uint32_t>(b[off]) | (static_cast<std::uint32_t>(b[off + 1]) << 8) |
           (static_cast<std::uint32_t>(b[off + 2]) << 16) | (static_cast<std::uint32_t>(b[off + 3]) << 24);
}

std::uint64_t hashWords(const std::vector<std::uint8_t>& img, std::int64_t delta, const std::vector<std::uint32_t>& addrs) {
    std::uint64_t h = 0xcbf29ce484222325ull;
    for (std::uint32_t a : addrs) {
        const std::uint32_t w = wordAt(img, static_cast<std::uint32_t>(static_cast<std::int64_t>(a) + delta));
        for (int k = 0; k < 4; ++k) { h ^= (w >> (8 * k)) & 0xFFu; h *= 0x100000001b3ull; }
    }
    return h;
}

struct WordGroup {
    const char* name;
    std::vector<std::uint32_t> addrs;
    std::uint64_t hash;
};

constexpr std::int64_t kElfDelta = -0x1FF000;
constexpr std::int64_t kIrxDelta = 0xA0;

Tally elfPins(const std::vector<std::uint8_t>& elf) {
    Tally e;
    const std::vector<WordGroup> groups = {
        {"gs_music_play_bgm cmd sequence + table stride",
         {0x258ee4, 0x258ef8, 0x258f00, 0x258f04, 0x258f14, 0x258f1c, 0x258f20}, 0x24b204d8cbc5a7ceull},
        {"PlayUiSound |0x40400000", {0x258e54, 0x258e5c, 0x258e64}, 0xf218335f023f3dbfull},
        {"race BGM chooser 0x22DDD0",
         {0x22de70, 0x22de88, 0x22de90, 0x22de94, 0x22de98, 0x22dea0, 0x22deb8, 0x22dec0, 0x22dec4, 0x22ded8},
         0x49257b5b519d971cull},
        {"rumbleTrigger 0x20ACB8 gate/clamp/stores",
         {0x20acbc, 0x20acc4, 0x20acc8, 0x20accc, 0x20acd4, 0x20acdc, 0x20ace0, 0x20acec, 0x20acf4, 0x20acfc, 0x20ad04, 0x20ad0c},
         0x55d904c65f684a86ull},
        {"rumble interpreter opcode dispatch 0x20A660",
         {0x20a6b0, 0x20a6b8, 0x20a6d4, 0x20a6d8, 0x20a6f4, 0x20a6f8, 0x20a6fc, 0x20a710, 0x20a718, 0x20a724, 0x20a72c,
          0x20a740, 0x20a750, 0x20a780, 0x20a788, 0x20a7b4, 0x20a7c8},
         0xa3ebd878009b72ddull},
    };
    for (const auto& g : groups) e.check(hashWords(elf, kElfDelta, g.addrs) == g.hash, g.name);
    int nBgm = 0, nUi = 0;
    for (std::size_t o = 0; o + 4 <= elf.size(); o += 4) {
        const std::uint32_t w = wordAt(elf, static_cast<std::uint32_t>(o));
        nBgm += w == (0x0C000000u | (0x258EC8u >> 2));
        nUi += w == (0x0C000000u | (0x258E50u >> 2));
    }
    e.check(nBgm == 35, "35 jal gs_music_play_bgm call sites");
    e.check(nUi == 256, "256 jal PlayUiSound call sites");
    ag::ElfAudioTables tab;
    const bool loaded = ag::readElfAudioTables(elf, tab);
    e.check(loaded, "readElfAudioTables (pointer table matches 0x2EA3D0 + 8n)");
    if (!loaded) return e;
    std::uint64_t h = 0xcbf29ce484222325ull;
    bool field0 = true;
    for (const auto& r : tab.files) {
        field0 = field0 && r.field0 == 0x00333388;
        for (std::uint32_t w : {r.field0, r.lbn, r.sectors})
            for (int k = 0; k < 4; ++k) { h ^= (w >> (8 * k)) & 0xFFu; h *= 0x100000001b3ull; }
    }
    e.check(field0 && h == 0x78cabc97449c3dc0ull, "gs_music_file_table 29x12B hash + field0 constant");
    std::uint64_t hr = 0xcbf29ce484222325ull;
    for (std::uint32_t w : tab.raceBgm)
        for (int k = 0; k < 4; ++k) { hr ^= (w >> (8 * k)) & 0xFFu; hr *= 0x100000001b3ull; }
    e.check(hr == 0x88268b4a7485172eull, "race BGM table hash");
    e.check(tab.raceBgm == ag::raceBgmDefaultTable(), "race BGM table == authored mirror (18/18)");
    int rec = 0;
    for (std::size_t i = 0; i < ag::kRumbleEffects; ++i) rec += tab.rumble[i] == ag::rumbleBuiltinTable()[i];
    e.check(rec == 8, "authored rumble scripts assemble to ELF 0x2EA3D0 records (8/8)");
    ag::RumbleInterpreter a, b;
    a.setGate(2, 2, 2);
    b.setGate(2, 2, 2);
    b.setTable(tab.rumble);
    bool same = true;
    for (int fx = 1; fx < 8; ++fx) {
        a.trigger(0, fx, 123);
        b.trigger(0, fx, 123);
        for (int f = 0; f < 40; ++f) {
            a.tick();
            b.tick();
            same = same && a.motors(0).small == b.motors(0).small && a.motors(0).large == b.motors(0).large;
        }
    }
    e.check(same, "interpreter over ELF table == builtin");
    std::span<const std::uint32_t, ag::kRaceBgmEntries> sp(tab.raceBgm);
    bool chooser = true;
    for (int m = 0; m < 0x20; ++m) {
        const auto id = ag::raceBgmId(0, static_cast<std::int8_t>(m), sp);
        const std::uint32_t want =
            m < 18 ? tab.raceBgm[static_cast<std::size_t>(m)] : (m == 0x1C ? 0x0A160201u : 0x0A0D0201u);
        chooser = chooser && id == want;
    }
    e.check(chooser, "race chooser over ELF table (32 modes)");
    bool wf = true;
    for (std::uint32_t w : tab.raceBgm) {
        const auto b2 = ag::unpackBgmId(w);
        wf = wf && b2.wellFormed && b2.fileIndex >= 12 && b2.fileIndex <= 21;
    }
    e.check(wf, "race ids are well-formed BGM_01..BGM_10 rows");
    return e;
}

Tally irxPins(const std::vector<std::uint8_t>& irx) {
    Tally e;
    bool freq = true;
    for (int k = 0; k < ag::kFreqTableSize; ++k) {
        const std::size_t off = 0x8A9C + 0xA0 + static_cast<std::size_t>(k) * 2;
        const std::uint16_t v = irx.size() > off + 1 ? static_cast<std::uint16_t>(irx[off] | (irx[off + 1] << 8)) : 0;
        freq = freq && v == ag::sndmodFreq(k);
    }
    e.check(freq, "freqtbl 72/72 == floor(4096*2^((k-48)/12))");
    std::vector<int> imms;
    for (std::uint32_t a = 0x5294; a < 0x5678; a += 4) {
        const std::uint32_t w = wordAt(irx, a + 0xA0);
        if ((w & 0xFFFF0000u) == 0x24030000u && (w & 0xFFFFu) >= 0xE0u && (w & 0xFFFFu) < 0x100u) imms.push_back(static_cast<int>(w & 0xFFFFu));
    }
    std::sort(imms.begin(), imms.end());
    const std::vector<int> want = {0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xF0, 0xF1, 0xF8, 0xF9};
    e.check(imms == want, "play() dispatches exactly E0-EA,F0,F1,F8,F9 (FF/unknown -> data_end)");
    const std::vector<WordGroup> groups = {
        {"play countdown dec/-1 compare + step_t stores D-1", {0x52c4, 0x52cc, 0x52d8, 0x52dc, 0x4810, 0x4814}, 0xf020cef02ca195aaull},
        {"volume()/panpot() vol*pan*master>>8 + mono fold",
         {0x4fd8, 0x4ff4, 0x5004, 0x5008, 0x500c, 0x505c, 0x5060, 0x5064, 0x509c, 0x50a8, 0x50ac, 0x50b0}, 0xf6a4d0055f66d2d2ull},
        {"key_on freq = freqtbl[key] * bend >> 12", {0x4be4, 0x4bec, 0x4c28, 0x4c38, 0x4c3c, 0x4c40}, 0x32cdc2cb9606a94dull},
    };
    for (const auto& g : groups) e.check(hashWords(irx, kIrxDelta, g.addrs) == g.hash, g.name);
    return e;
}

// ------------------------------------------------------------------- TSQ corpus

struct TsqCorpus {
    int files = 0, tracks = 0, endFf = 0, endUnknown = 0, endOob = 0, runaway = 0, alive = 0, forks = 0, stalls = 0;
    std::uint64_t hash = 0;
};

unsigned rd16(const std::vector<std::uint8_t>& d, std::size_t o) {
    return o + 2 <= d.size() ? static_cast<unsigned>(d[o] | (d[o + 1] << 8)) : 0u;
}

// Track start offsets of one TSQ image. Flat files (ACTION, CQ_MAIN): u16 {seg,off} table from
// parseTsq. BGM family: entry 0 = {0, T} points at a second table at T of {0x0080, rel} entries
// whose targets are T + rel (offsets are relative to the table - found this pass; seg 0 entries
// are unused slots pointing at the end of the file).
std::vector<std::uint32_t> tsqTrackOffsets(const std::vector<std::uint8_t>& d) {
    std::vector<std::uint32_t> offs;
    if (d.size() < 8) return offs;
    if (rd16(d, 4) == 0) {
        const std::uint32_t base = rd16(d, 6);
        std::uint32_t first = 0xFFFFFFu;
        for (std::size_t pos = base; pos + 4 <= d.size() && pos < first; pos += 4) {
            if (rd16(d, pos) != 0x80) break;
            const std::uint32_t tgt = base + rd16(d, pos + 2);
            if (offs.empty()) first = tgt;
            offs.push_back(tgt);
        }
    } else if (auto parsed = rt::native::assets::parseTsq(d); parsed.has_value()) {
        for (const auto& tr : parsed->tracks) offs.push_back(tr.offset);
        std::sort(offs.begin(), offs.end());
        offs.erase(std::unique(offs.begin(), offs.end()), offs.end());
    }
    return offs;
}

TsqCorpus tsqCorpus(const fs::path& soundDir, int maxTicks) {
    TsqCorpus c;
    c.hash = 1469598103934665603ull;
    std::vector<fs::path> files;
    std::error_code ec;
    for (const auto& de : fs::directory_iterator(soundDir, ec))
        if (de.path().extension() == ".TSQ") files.push_back(de.path());
    std::sort(files.begin(), files.end());
    for (const auto& f : files) {
        const auto data = readFile(f);
        ++c.files;
        for (std::uint32_t off : tsqTrackOffsets(data)) {
            ag::SeqPlayer p(data);
            if (p.start(off) < 0) continue;
            ++c.tracks;
            std::vector<ag::SeqEvent> ev;
            bool stalled = false;
            for (int i = 0; i < maxTicks && p.activeCount() > 0; ++i) {
                ev.clear();
                p.tick(ev);
                if (p.active(0) && p.countdown(0) == -1) stalled = true;  // delta 0 executed
                for (const auto& e : ev) {
                    c.hash = fnv(c.hash, (static_cast<std::uint64_t>(e.tick) << 32) ^ (static_cast<std::uint64_t>(e.kind) << 8) ^ e.key ^
                                             (static_cast<std::uint64_t>(e.freq) << 16));
                    if (e.kind == ag::SeqEventKind::Fork) ++c.forks;
                    if (e.kind == ag::SeqEventKind::End && e.track == 0) {
                        if (e.aux == 0) ++c.endFf;
                        else if (e.aux == 1) ++c.endUnknown;
                        else if (e.aux == 2) ++c.endOob;
                        else ++c.runaway;
                    }
                }
            }
            if (stalled) ++c.stalls;
            if (p.active(0)) ++c.alive;
        }
    }
    return c;
}

// Fully traced ACTION.TSQ phrase (tsq-tvb-format.md): E9 F0 F9 0B 00 E1 40 00 E0 B4 E2 01 A1 64 F0 FF at 0x194.
// F9 0B 00: pc after operands 0x199 + 11 = 0x1A4 = the physically-next mirrored phrase (E1 00 40).
bool actionPhrase(const fs::path& soundDir) {
    const auto data = readFile(soundDir / "ACTION.TSQ");
    if (data.size() < 0x1B0) return false;
    ag::SeqPlayer p(data);
    if (p.start(0x194) != 0) return false;
    std::vector<ag::SeqEvent> ev;
    for (int i = 0; i < 101; ++i) p.tick(ev);
    int fork = -1;
    bool onL = false, onR = false, off0 = false, offF = false, end0 = false, endF = false;
    for (const auto& e : ev) {
        if (e.kind == ag::SeqEventKind::Fork && e.track == 0 && e.tick == 0) fork = e.aux;
    }
    if (fork < 0) return false;
    for (const auto& e : ev) {
        if (e.kind == ag::SeqEventKind::NoteOn && e.tick == 0 && e.key == 33 && e.program == 1) {
            if (e.track == 0) onL = e.volL == 0x40 * 0xB4 && e.volR == 0;
            if (e.track == fork) onR = e.volL == 0 && e.volR == 0x40 * 0xB4;
        }
        if (e.tick == 100 && e.kind == ag::SeqEventKind::NoteOff) (e.track == 0 ? off0 : offF) = true;
        if (e.tick == 100 && e.kind == ag::SeqEventKind::End && e.aux == 0) (e.track == 0 ? end0 : endF) = true;
    }
    return onL && onR && off0 && offF && end0 && endF && !p.active(0) && !p.active(fork);
}

struct PureResult {
    int seqPass = 0, seqTotal = 0;
    int rumblePass = 0, rumbleTotal = 0;
    int panPass = 0, panTotal = 0;
    int mathPass = 0, mathTotal = 0;
    int graphPass = 0, graphTotal = 0;
    int idPass = 0, idTotal = 0;
    std::uint64_t hash = 0;
};

PureResult runPure() {
    PureResult r;
    r.hash = 1469598103934665603ull;
    Tally t;
    seqVectors(t, r.hash);
    r.seqPass = t.pass, r.seqTotal = t.total;
    t = {};
    rumbleVectors(t, r.hash);
    r.rumblePass = t.pass, r.rumbleTotal = t.total;
    t = {};
    panVectors(t);
    r.panPass = t.pass, r.panTotal = t.total;
    t = {};
    sndmodMath(t);
    r.mathPass = t.pass, r.mathTotal = t.total;
    t = {};
    idVectors(t);
    r.idPass = t.pass, r.idTotal = t.total;
    t = {};
    graphTests(t, r.hash);
    r.graphPass = t.pass, r.graphTotal = t.total;
    return r;
}

std::string elfPathFromArgs(int argc, char** argv) {
    if (const char* e = std::getenv("RT_ELF"); e && *e) return e;
    for (int i = 1; i + 1 < argc; ++i)
        if (std::string(argv[i]) == "--elf") return argv[i + 1];
    return {};
}

constexpr int kTsqTracksPinned = 622;  // 75 flat (ACTION 25 + CQ_MAIN 50) + 547 second-layer tracks
constexpr std::uint64_t kAllocGolden = 0x720fe023f3c23092ull;  // xorshift32 seed 0xC0FFEE; integer ids + exact-float state hash

}  // namespace

int main(int argc, char** argv) {
    const std::string elfPath = elfPathFromArgs(argc, argv);
    std::vector<std::uint8_t> elf;
    if (!elfPath.empty()) elf = readFile(elfPath);
    const bool haveElf = elf.size() > 0x2B000;
    if (!haveElf) std::puts("NO_ELF");

    PureResult a = runPure();
    PureResult b = runPure();
    const AllocResult r1 = allocRun(0xC0FFEEu);
    const AllocResult r2 = allocRun(0xC0FFEEu);
    bool ok = a.seqPass == a.seqTotal && a.rumblePass == a.rumbleTotal && a.panPass == a.panTotal &&
              a.mathPass == a.mathTotal && a.graphPass == a.graphTotal && a.idPass == a.idTotal;
    const bool deterministic = a.hash == b.hash && r1.hash == r2.hash;
    const bool allocOk = r1.refMatch == r1.refTotal && r1.refTotal > 1000 && (kAllocGolden == 0 || r1.hash == kAllocGolden);
    ok = ok && deterministic && allocOk;

    std::string tsqField = "tsq=NA";
    std::string elfF = "elf_pins=NA", irxF = "irx_pins=NA";
    if (haveElf) {
        const Tally e = elfPins(elf);
        char buf[48];
        std::snprintf(buf, sizeof buf, "elf_pins=%d/%d", e.pass, e.total);
        elfF = buf;
        ok = ok && e.pass == e.total;
        const fs::path disc = fs::path(elfPath).parent_path();
        const auto irx = readFile(disc / "SNDMOD.IRX");
        if (irx.size() > 0x9000) {
            const Tally i = irxPins(irx);
            std::snprintf(buf, sizeof buf, "irx_pins=%d/%d", i.pass, i.total);
            irxF = buf;
            ok = ok && i.pass == i.total;
        }
        if (fs::exists(disc / "SOUND" / "ACTION.TSQ")) {
            const TsqCorpus c = tsqCorpus(disc / "SOUND", 3600);
            const bool phrase = actionPhrase(disc / "SOUND");
            const TsqCorpus c2 = tsqCorpus(disc / "SOUND", 3600);
            char tb[200];
            std::snprintf(tb, sizeof tb, "tsq_files=%d tsq_tracks=%d tsq_ff=%d tsq_unknown=%d tsq_oob=%d tsq_runaway=%d tsq_alive=%d tsq_forks=%d tsq_stalls=%d action_phrase=%d",
                          c.files, c.tracks, c.endFf, c.endUnknown, c.endOob, c.runaway, c.alive, c.forks, c.stalls, phrase ? 1 : 0);
            tsqField = tb;
            ok = ok && phrase && c.hash == c2.hash && c.files == 20 && c.tracks == kTsqTracksPinned && c.endUnknown == 0 && c.endOob == 0 &&
                 c.runaway == 0 && c.stalls == 0;
        }
    }
    std::printf("W2-09 SCORE seq_vectors=%d/%d rumble_vectors=%d/%d pan_vectors=%d/%d sndmod_math=%d/%d ids=%d/%d graph=%d/%d "
                "alloc_ref=%d/%d alloc_hash=%016llx %s %s %s deterministic=%d %s\n",
                a.seqPass, a.seqTotal, a.rumblePass, a.rumbleTotal, a.panPass, a.panTotal, a.mathPass, a.mathTotal, a.idPass,
                a.idTotal, a.graphPass, a.graphTotal, r1.refMatch, r1.refTotal, static_cast<unsigned long long>(r1.hash),
                elfF.c_str(), irxF.c_str(), tsqField.c_str(), deterministic ? 1 : 0, ok ? "PASS" : "FAIL");
    return ok ? 0 : 1;
}
