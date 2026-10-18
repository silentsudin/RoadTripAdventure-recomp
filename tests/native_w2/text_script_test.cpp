// W2-07 acceptance: dialogue & text engine (gameplay/text_script.{h,cpp}).
//   w2_text_script_test [--elf PATH] [--data DIR] [--allow-skip] [--verbose]
//
// ELF: --elf, else $RT_ELF, else ~/Library/Application Support/RoadTripRecomp/disc/SLUS_203.98.
// Without an ELF the ROM-independent part (oracle vectors, fuzz, determinism) still runs; the
// ROM-dependent keys print NO_ELF and the exit code is 0 when everything that could run passed.
// Prints exactly one `W2-07 SCORE ...` line.  Exit 0 only on PASS (every threshold met).
//
// No ROM bytes are embedded.  tests/native_w2/data/text_script_vectors.txt holds numbers that were
// produced by EXECUTING the original text-engine functions from the ELF in a MIPS interpreter on
// synthetic inputs (generator in .qoder/wave2/W2-07-report.md); text_extents.csv holds pixel
// extents measured on the oracle golden PNGs.

#include "assets/glyph_atlas.h"
#include "assets/script_image.h"
#include "gameplay/text_script.h"

#include <algorithm>
#include <array>
#include <cstdint>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <optional>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

using namespace rt::native;
using namespace rt::native::gameplay::text;

namespace {

bool gVerbose = false;

constexpr std::uint64_t kFnvOffset = 0xcbf29ce484222325ULL;
constexpr std::uint64_t kFnvPrime = 0x100000001b3ULL;
struct Fnv {
    std::uint64_t h = kFnvOffset;
    void add(std::uint64_t v) {
        for (int i = 0; i < 8; ++i) h = (h ^ ((v >> (8 * i)) & 0xff)) * kFnvPrime;
    }
};

std::uint64_t eventsHash(const std::vector<DrawEvent>& evs) {
    Fnv f;
    for (const DrawEvent& e : evs) {
        f.add(static_cast<std::uint64_t>(e.kind));
        for (std::int64_t v : {e.a, e.b, e.c, e.d, e.e, e.f}) f.add(static_cast<std::uint64_t>(v));
    }
    return f.h;
}

std::vector<std::uint8_t> unhex(const std::string& s) {
    std::vector<std::uint8_t> out;
    if (s == "-") return out;
    for (std::size_t i = 0; i + 1 < s.size(); i += 2) {
        out.push_back(static_cast<std::uint8_t>(std::stoi(s.substr(i, 2), nullptr, 16)));
    }
    return out;
}
std::string hexs(const std::string& s) {
    static const char* d = "0123456789abcdef";
    if (s.empty()) return "-";
    std::string o;
    for (unsigned char c : s) { o.push_back(d[c >> 4]); o.push_back(d[c & 15]); }
    return o;
}
std::string sv(const std::vector<std::uint8_t>& b) { return std::string(b.begin(), b.end()); }

std::vector<std::string> split(const std::string& line, char sep = ' ') {
    std::vector<std::string> t;
    std::string cur;
    for (char c : line) {
        if (c == sep) { if (!cur.empty()) t.push_back(cur); cur.clear(); }
        else cur.push_back(c);
    }
    if (!cur.empty()) t.push_back(cur);
    return t;
}

// "S,x,y,u,v,w,h" / "N,x,y,v,b" / "T,v" tokens -> events
DrawEvent parseEvent(const std::string& tok) {
    const auto f = split(tok, ',');
    DrawEvent e;
    auto num = [&](std::size_t i) { return std::stoll(f.at(i)); };
    if (f[0] == "S") { e.kind = DrawEvent::Kind::Sprite; e.a = num(1); e.b = num(2); e.c = num(3); e.d = num(4); e.e = num(5); e.f = num(6); }
    else if (f[0] == "N") { e.kind = DrawEvent::Kind::Number; e.a = num(1); e.b = num(2); e.c = num(3); e.d = num(4); }
    else { e.kind = DrawEvent::Kind::Texa; e.a = num(1); }
    return e;
}
std::vector<DrawEvent> parseEvents(const std::vector<std::string>& t, std::size_t from) {
    std::vector<DrawEvent> v;
    for (std::size_t i = from; i < t.size(); ++i) if (t[i] != "-") v.push_back(parseEvent(t[i]));
    return v;
}

// --------------------------------------------------------------------------
// Host used by every fixture (values from the HOST line).
// --------------------------------------------------------------------------
class FixtureHost final : public IEscapeHost {
public:
    std::string name, u, g;
    std::int32_t s = 0, t7 = 0;
    std::map<int, std::int32_t> a, t;
    std::vector<std::string> course;
    int courseIdx = 0;
    std::int32_t amountWord(std::uint8_t id) const override { auto it = a.find(id); return it == a.end() ? 0 : it->second; }
    std::string_view stringG() const override { return g; }
    std::string_view playerName() const override { return name; }
    std::string_view courseName() const override { return course.at(static_cast<std::size_t>(courseIdx)); }
    std::int32_t bankBalance() const override { return s; }
    std::string_view stringU() const override { return u; }
    std::int32_t clockWord(std::uint8_t id) const override {
        if (id == 0x37) return t7;
        auto it = t.find(id);
        return it == t.end() ? 0 : it->second;
    }
};

class SparseMemory final : public ITextMemory {
public:
    void put(std::uint32_t a, const std::vector<std::uint8_t>& b) { for (std::size_t i = 0; i < b.size(); ++i) bytes_[a + static_cast<std::uint32_t>(i)] = b[i]; }
    void put32(std::uint32_t a, std::uint32_t v) { for (int i = 0; i < 4; ++i) bytes_[a + i] = static_cast<std::uint8_t>(v >> (8 * i)); }
    bool readU8(std::uint32_t a, std::uint8_t& o) const override {
        // The emulator's RAM reads 0 everywhere; one text window is 2 KiB so a bound is enough.
        const auto it = bytes_.find(a);
        o = it == bytes_.end() ? 0 : it->second;
        return true;
    }
    bool readU32(std::uint32_t a, std::uint32_t& o) const override {
        o = 0;
        for (int i = 0; i < 4; ++i) { std::uint8_t b = 0; readU8(a + i, b); o |= static_cast<std::uint32_t>(b) << (8 * i); }
        return true;
    }

private:
    std::map<std::uint32_t, std::uint8_t> bytes_;
};

std::string fxString(const Effect& e) {
    char b[96];
    using K = Effect::Kind;
    switch (e.kind) {
        case K::Sound: std::snprintf(b, sizeof b, "snd:%x", e.a); break;
        case K::SpawnChild: std::snprintf(b, sizeof b, "spawn:%x:%x", e.a, e.b); break;
        case K::Kill: return "kill";
        case K::NextMsg: std::snprintf(b, sizeof b, "next:%x", e.a); break;
        case K::AwardStamp: std::snprintf(b, sizeof b, "stamp:%u", e.a); break;
        case K::TakePhoto: std::snprintf(b, sizeof b, "photo:%u", e.a); break;
        case K::SetObjectAttr: std::snprintf(b, sizeof b, "attr:%u:%u", e.a, e.b); break;
        case K::ReleaseStamp: return "rel";
        case K::ParentDone: std::snprintf(b, sizeof b, "pdone:%d", static_cast<int>(e.a)); break;
        case K::CtxFlag400: std::snprintf(b, sizeof b, "f400:%u", e.a); break;
        case K::TalkAnim: std::snprintf(b, sizeof b, "talk:%u", e.a); break;
        case K::CtxByte20: std::snprintf(b, sizeof b, "c20:%u", e.a); break;
        case K::WarpTo: std::snprintf(b, sizeof b, "warp:%u:%u", e.a, e.b); break;
        case K::TownVar: std::snprintf(b, sizeof b, "town:%u", e.a); break;
    }
    return b;
}
bool isMemoryFx(Effect::Kind k) {
    using K = Effect::Kind;
    return k == K::ParentDone || k == K::CtxFlag400 || k == K::TalkAnim || k == K::CtxByte20 || k == K::WarpTo || k == K::TownVar;
}

// --------------------------------------------------------------------------
// Results
// --------------------------------------------------------------------------
struct Tally {
    std::size_t pass = 0, total = 0;
    void check(bool ok) { ++total; if (ok) ++pass; }
    bool all() const { return pass == total && total > 0; }
};

struct Result {
    Tally glyph, width, numcells, frames, render, runaway, extract, copy, subst, window, windowScn;
    Tally advanceTable, fullwidthTable, escJump, escVectors, fuzz, elfMsgs, extents, positions, lineFit, escElf, fontGlyphs, fontLower, fontSlots;
    bool hadElf = false, hadFont = false;
    std::size_t firstFailLine = 0;
    std::uint64_t digest = 0;
    // census
    std::size_t msgs = 0, msgsUnclean = 0, linesTotal = 0, linesOver = 0, linesWithEsc = 0, wrapSame = 0, wrapMulti = 0;
    std::size_t pagesTotal = 0, cmdHist[32] = {};
    std::map<char, std::size_t> escHist;
    double meanDiff = 0, maxDiff = 0;
    std::size_t windowTicks = 0;
    std::size_t runawayNative = 0;
};

std::filesystem::path dataDir(const std::string& override_) {
    if (!override_.empty()) return override_;
    return std::filesystem::path(__FILE__).parent_path() / "data";
}

// --------------------------------------------------------------------------
// Part A: oracle vectors (ROM independent)
// --------------------------------------------------------------------------
void runVectors(const std::filesystem::path& file, Result& R, Fnv& dg) {
    std::ifstream in(file);
    if (!in) { std::cerr << "MISSING vectors: " << file << "\n"; R.glyph.check(false); return; }
    FixtureHost host;
    SparseMemory* mem = nullptr;
    std::unique_ptr<SparseMemory> memOwner;
    std::unique_ptr<MessageWindow> win;
    std::vector<std::vector<std::uint8_t>> scnMsgs;
    std::string line;
    std::size_t ln = 0;
    bool scnOk = true;
    std::size_t scnTicks = 0;
    auto finishScn = [&]() {
        if (win) { R.windowScn.check(scnOk); win.reset(); memOwner.reset(); mem = nullptr; }
    };
    auto fail = [&](const char* what) {
        if (R.firstFailLine == 0) R.firstFailLine = ln;
        if (gVerbose) std::cerr << "FAIL " << what << " @line " << ln << "\n";
    };
    while (std::getline(in, line)) {
        ++ln;
        if (line.empty() || line[0] == '#') continue;
        const auto t = split(line);
        const std::string& k = t[0];
        if (k == "HOST") {
            host.name = sv(unhex(t[1])); host.u = sv(unhex(t[2])); host.g = sv(unhex(t[3]));
            host.s = std::stoi(t[4]); host.t7 = std::stoi(t[5]);
            for (const auto& kv : split(t[6], ';')) { auto p = split(kv, ':'); host.a[std::stoi(p[0])] = static_cast<std::int32_t>(std::stoll(p[1])); }
            for (const auto& kv : split(t[7], ';')) { auto p = split(kv, ':'); host.t[std::stoi(p[0])] = static_cast<std::int32_t>(std::stoll(p[1])); }
            host.course.clear();
            for (const auto& c : split(t[8], ';')) host.course.push_back(sv(unhex(c)));
        } else if (k == "GLYPH") {
            const auto b = unhex(t[1]);
            const int x = std::stoi(t[2]), y = std::stoi(t[3]), sh = std::stoi(t[4]), nx = std::stoi(t[5]);
            const auto exp = parseEvents(t, 6);
            const GlyphDraw g = drawGlyph(x, y, sh, b[0], b.size() > 1 ? b[1] : 0);
            bool ok = g.nextX == nx && (g.hasSprite == !exp.empty());
            if (ok && g.hasSprite) ok = spriteEvent(g.sprite) == exp[0];
            R.glyph.check(ok);
            if (!ok) fail("glyph");
            dg.add(static_cast<std::uint64_t>(g.nextX)); dg.add(g.hasSprite ? eventsHash({spriteEvent(g.sprite)}) : 0);
        } else if (k == "WIDTH") {
            const auto b = unhex(t[1]);
            const bool ok = textWidth(std::span<const std::uint8_t>(b)) == std::stoi(t[2]);
            R.width.check(ok);
            if (!ok) fail("width");
        } else if (k == "NUMCELLS") {
            R.numcells.check(numberAutoCells(std::stoll(t[1])) == std::stoi(t[2]));
        } else if (k == "FRAMEA" || k == "FRAMEB" || k == "ICON") {
            std::vector<Sprite> got;
            std::size_t from = 0;
            if (k == "FRAMEA") { got = frameASprites(std::stoi(t[1])); from = 2; }
            else if (k == "FRAMEB") { got = frameBSprites(std::stoi(t[2]), std::stoi(t[1])); from = 3; }
            else { got = {icon20Sprite(std::stoi(t[1]), std::stoi(t[2]), std::stoi(t[3]))}; from = 4; }
            const auto exp = parseEvents(t, from);
            bool ok = exp.size() == got.size();
            for (std::size_t i = 0; ok && i < got.size(); ++i) ok = spriteEvent(got[i]) == exp[i];
            R.frames.check(ok);
            if (!ok) fail("frame");
        } else if (k == "RENDER" || k == "RENDERX" || k == "RUNAWAY") {
            const int sc = std::stoi(t[1]);
            const int lim = std::stoi(t[2]);
            host.courseIdx = std::stoi(t[3]);
            const auto text = unhex(t[4]);
            const PageResult r = renderEscapeText(text, 0, TextState{static_cast<std::uint8_t>(sc), static_cast<std::int16_t>(lim)}, host);
            dg.add(eventsHash(r.events)); dg.add(static_cast<std::uint64_t>(r.status)); dg.add(r.cursor);
            if (k == "RUNAWAY") {
                R.runaway.check(r.runaway);
                if (!r.runaway) fail("runaway");
                continue;
            }
            bool ok = !r.runaway;
            const bool expPage = t[5] == "-";
            ok = ok && ((r.status == PageStatus::PageComplete) == expPage);
            if (ok && !expPage) {
                ok = r.cursor == static_cast<std::size_t>(std::stoul(t[5]));
                const std::uint8_t at = r.cursor < text.size() ? text[r.cursor] : 0;
                ok = ok && ((at >= 0x20) == (r.status == PageStatus::LimitReached));
            }
            ok = ok && r.newLimit == static_cast<std::int16_t>(std::stoi(t[6]));
            if (k == "RENDERX") {
                const auto exp = parseEvents(t, 7);
                ok = ok && exp == r.events;
            } else {
                ok = ok && r.events.size() == static_cast<std::size_t>(std::stoul(t[7]));
                ok = ok && eventsHash(r.events) == std::stoull(t[8], nullptr, 16);
            }
            R.render.check(ok);
            if (!ok) fail("render");
        } else if (k == "EXTRACT") {
            const auto src = unhex(t[1]);
            const auto dl = unhex(t[2]);
            std::string o;
            const bool got = extractDelimitedToken(sv(src), sv(dl), std::stoi(t[3]), o);
            bool ok;
            if (t[4] == "NULL") ok = !got;
            else if (t[4] == "EMPTY") ok = got && o.empty();
            else ok = got && hexs(o) == t[4];
            R.extract.check(ok);
            if (!ok) fail("extract");
        } else if (k == "COPY") {
            const auto src = unhex(t[1]);
            const std::string o = copyDialogueTextBounded(src, std::stoi(t[2]), 4096);
            const bool ok = hexs(o) == t[3];
            R.copy.check(ok);
            if (!ok) fail("copy");
        } else if (k == "SUBST") {
            std::string s = sv(unhex(t[1]));
            const bool got = substituteNameTokens(s, host.name, 4096);
            bool ok;
            if (t[2] == "NONE") ok = !got;
            else if (t[2] == "EMPTY") ok = got && s.empty();
            else ok = got && hexs(s) == t[2];
            R.subst.check(ok);
            if (!ok) fail("subst");
        } else if (k == "SCN") {
            finishScn();
            memOwner = std::make_unique<SparseMemory>();
            mem = memOwner.get();
            scnOk = true;
            scnTicks = 0;
            constexpr std::uint32_t kTbl = 0x01400000, kName = 0x01500000;
            auto name = unhex(t[2]);
            name.push_back(0);
            mem->put(kName, name);
            for (std::size_t i = 4; i < t.size(); ++i) mem->put32(kTbl + 4 * static_cast<std::uint32_t>(i - 4), static_cast<std::uint32_t>(std::stoul(t[i], nullptr, 16)));
            host.courseIdx = 0;
            win = std::make_unique<MessageWindow>(*mem, host, kTbl, [](std::uint32_t c) { return c; });
        } else if (k == "MSG") {
            auto m = unhex(t[3]);
            for (int i = 0; i < 8; ++i) m.push_back(0);
            mem->put(0x02000000u + 0x800u * static_cast<std::uint32_t>(std::stoi(t[2])), m);
        } else if (k == "TICK") {
            if (!win) continue;
            const bool pad = t[3] == "1";
            if (t[4] != "-") win->signalChildDone(std::stoi(t[4]));
            const TickOutput o = win->tick(pad);
            ++scnTicks;
            ++R.windowTicks;
            std::vector<std::string> ord, memfx;
            for (const Effect& e : o.effects) (isMemoryFx(e.kind) ? memfx : ord).push_back(fxString(e));
            auto join = [](const std::vector<std::string>& v) { std::string s; for (const auto& x : v) { if (!s.empty()) s += ","; s += x; } return s.empty() ? std::string("-") : s; };
            char cur[16];
            std::snprintf(cur, sizeof cur, "%x", win->cursor());
            bool ok = static_cast<int>(win->state()) == std::stoi(t[5]) && win->scroll() == std::stoi(t[6]) &&
                      win->limit() == std::stoi(t[7]) && std::string(cur) == t[8] && win->pending() == std::stoi(t[9]) &&
                      o.events.size() == static_cast<std::size_t>(std::stoul(t[10])) &&
                      eventsHash(o.events) == std::stoull(t[11], nullptr, 16) && join(ord) == t[12] && join(memfx) == t[13];
            R.window.check(ok);
            if (!ok) { scnOk = false; fail("window tick"); if (gVerbose && R.window.total - R.window.pass < 3) {
                std::cerr << "  got state=" << int(win->state()) << " scroll=" << int(win->scroll()) << " limit=" << win->limit() << " cur=" << cur
                          << " pend=" << int(win->pending()) << " nev=" << o.events.size() << " fx=" << join(ord) << " mem=" << join(memfx) << "\n  exp " << line << "\n";
                for (const DrawEvent& e : o.events) std::cerr << "    ev k=" << int(e.kind) << " " << e.a << "," << e.b << "," << e.c << "," << e.d << "," << e.e << "," << e.f << "\n"; } }
            dg.add(eventsHash(o.events)); dg.add(win->cursor()); dg.add(static_cast<std::uint64_t>(win->state()));
        }
    }
    finishScn();
    (void)scnTicks;
}

// --------------------------------------------------------------------------
// Part B: bounded copy fuzz (independent reference) -- ROM independent
// --------------------------------------------------------------------------
struct Lcg {
    std::uint64_t s;
    std::uint32_t next() { s = s * 6364136223846793005ULL + 1442695040888963407ULL; return static_cast<std::uint32_t>(s >> 33); }
    std::uint32_t below(std::uint32_t n) { return next() % n; }
};

// Straight-line restatement of the decomp: per character emit 2 bytes.
std::string refCopy(const std::vector<std::uint8_t>& src, int limit) {
    std::string o;
    std::size_t i = 0;
    for (int n = 0;; ++n) {
        const std::uint8_t c = i < src.size() ? src[i] : 0;
        if (limit == 0 && c == 0) break;
        if (limit != 0 && n >= limit) break;
        if (c & 0x80) { o.push_back(static_cast<char>(c)); o.push_back(static_cast<char>(i + 1 < src.size() ? src[i + 1] : 0)); i += 2; continue; }
        std::uint16_t f = 0x8140;
        if (c >= '0' && c <= '9') f = static_cast<std::uint16_t>(0x824F + c - '0');
        else if (c >= 'A' && c <= 'Z') f = static_cast<std::uint16_t>(0x8260 + c - 'A');
        else if (c >= 'a' && c <= 'z') f = static_cast<std::uint16_t>(0x8281 + c - 'a');
        else f = fullwidthSjis(c);
        o.push_back(static_cast<char>(f >> 8)); o.push_back(static_cast<char>(f & 0xff));
        i += 1;
    }
    return o;
}

void runFuzz(Result& R, Fnv& dg) {
    Lcg g{0x57075707ULL};
    for (int it = 0; it < 10000; ++it) {
        std::vector<std::uint8_t> src(g.below(40));
        for (auto& b : src) {
            const std::uint32_t r = g.below(10);
            b = r < 2 ? static_cast<std::uint8_t>(0x80 + g.below(0x80)) : r < 3 ? static_cast<std::uint8_t>(g.below(0x20)) : static_cast<std::uint8_t>(0x20 + g.below(0x5f));
        }
        const int limit = static_cast<int>(g.below(4) == 0 ? 0 : g.below(30));
        const std::size_t cap = 1 + g.below(80);
        int chars = 0;
        const std::string o = copyDialogueTextBounded(src, limit, cap, &chars);
        bool ok = o.size() + 1 <= cap && (o.size() % 2) == 0 && static_cast<int>(o.size()) == chars * 2;
        // never splits: re-parse as pairs; each pair is either source pair or fullwidth code
        const std::string ref = refCopy(src, limit);
        ok = ok && ref.compare(0, o.size(), o) == 0;                       // prefix of the unbounded result
        const bool truncatedByCap = o.size() + 3 > cap && o.size() < ref.size();
        const bool complete = o.size() == ref.size();
        ok = ok && (complete || truncatedByCap);                           // stops only because of cap
        R.fuzz.check(ok);
        dg.add(std::hash<std::string>{}(o));
    }
}

// --------------------------------------------------------------------------
// Part C: ELF-dependent checks
// --------------------------------------------------------------------------
struct Elf {
    std::vector<std::uint8_t> d;
    bool u8(std::uint32_t v, std::uint8_t& o) const {
        if (v < 0x1FF000) return false;
        const std::size_t off = v - 0x1FF000;
        if (off >= d.size()) return false;
        o = d[off];
        return true;
    }
    std::uint32_t u32(std::uint32_t v) const {
        std::uint32_t r = 0;
        for (int i = 0; i < 4; ++i) { std::uint8_t b = 0; u8(v + i, b); r |= static_cast<std::uint32_t>(b) << (8 * i); }
        return r;
    }
    std::uint8_t b(std::uint32_t v) const { std::uint8_t x = 0; u8(v, x); return x; }
};

class ZeroHost final : public IEscapeHost {
public:
    std::int32_t amountWord(std::uint8_t) const override { return 0; }
    std::string_view stringG() const override { return "Gem"; }
    std::string_view playerName() const override { return "Ann"; }
    std::string_view courseName() const override { return "Course"; }
    std::int32_t bankBalance() const override { return 1; }
    std::string_view stringU() const override { return "Gold"; }
    std::int32_t clockWord(std::uint8_t) const override { return 0; }
};

// Control-prefix widths of ops 1..0x1D (docs/native-reference/script-vm.md §3); ops 9,10,12 unknown.
constexpr int kOpWidth[0x1E] = {0, 3, 3, 4, 2, 3, 3, 2, 3, 0, 0, 3, 0, 3, 7, 2, 2, 3, 1, 2, 3, 2, 2, 2, 2, 3, 3, 2, 3, 4};

std::int64_t prefixEnd(const assets::ScriptImage& img, std::uint32_t a) {
    for (int guard = 0; guard < 64; ++guard) {
        std::uint8_t op = 0;
        if (!img.readU8(a, op)) return -1;
        if (op >= 0x20) return a;
        if (op == 0 || op >= 0x1E || kOpWidth[op] == 0) return -1;
        a += static_cast<std::uint32_t>(kOpWidth[op]);
    }
    return -1;
}

void runElf(const Elf& elf, const assets::ScriptImage& img, const std::filesystem::path& fontPath, const std::filesystem::path& dir,
            Result& R, Fnv& dg) {
    // advance table 0x29A880 (96 entries)
    for (unsigned i = 0; i < kAdvanceCount; ++i) {
        R.advanceTable.check(static_cast<std::int8_t>(elf.b(0x29A880 + i)) == kAdvanceTable[i]);
    }
    // fullwidth table: pointers as in the decomp
    for (unsigned c = 0x20; c <= 0x7e; ++c) {
        std::uint32_t p = 0;
        if (c >= 0x30 && c <= 0x39) { p = 0x2a9b38; const std::uint16_t v = static_cast<std::uint16_t>((elf.b(p) << 8) | (elf.b(p + 1) + (c - 0x30))); R.fullwidthTable.check(v == fullwidthSjis(static_cast<std::uint8_t>(c))); continue; }
        if (c >= 0x41 && c <= 0x5a) { p = 0x2a9b3a; const std::uint16_t v = static_cast<std::uint16_t>((elf.b(p) << 8) | (elf.b(p + 1) + (c - 0x41))); R.fullwidthTable.check(v == fullwidthSjis(static_cast<std::uint8_t>(c))); continue; }
        if (c >= 0x61 && c <= 0x7a) { p = 0x2a9b3c; const std::uint16_t v = static_cast<std::uint16_t>((elf.b(p) << 8) | (elf.b(p + 1) + (c - 0x61))); R.fullwidthTable.check(v == fullwidthSjis(static_cast<std::uint8_t>(c))); continue; }
        if (c <= 0x2f) p = 0x2a9afe + c * 2;
        else if (c <= 0x40) p = 0x2a9aea + c * 2;
        else if (c <= 0x60) p = 0x2a9ab6 + c * 2;
        else p = 0x2a9a82 + c * 2;
        const std::uint16_t v = static_cast<std::uint16_t>((elf.b(p) << 8) | elf.b(p + 1));
        R.fullwidthTable.check(v == fullwidthSjis(static_cast<std::uint8_t>(c)));
    }
    R.fullwidthTable.check(((elf.b(0x334498) << 8) | elf.b(0x334499)) == 0x8140);

    // `$` jump table 0x2EC6C0 (code - 0x41, 21 entries): live handlers are exactly A G N R S T U
    {
        std::set<std::uint32_t> live;
        const std::uint32_t noop = 0x23ac48;
        std::map<char, std::uint32_t> target;
        for (int i = 0; i < 21; ++i) {
            const std::uint32_t w = elf.u32(0x2EC6C0 + 4 * i);
            target[static_cast<char>('A' + i)] = w;
        }
        const std::string liveCodes = "AGNRSTU";
        for (int i = 0; i < 21; ++i) {
            const char c = static_cast<char>('A' + i);
            const bool isLive = liveCodes.find(c) != std::string::npos;
            R.escJump.check((target[c] != noop) == isLive && target[c] >= 0x23aa50 && target[c] <= 0x23ac48);
            if (isLive) live.insert(target[c]);
        }
        R.escJump.check(live.size() == 7);  // seven distinct handler bodies
    }

    // census over every ELF message: decode prefix, page through the text
    const assets::ScriptCensus census = img.census();
    std::set<std::uint32_t> starts;
    for (const auto& tab : census.tables) {
        for (std::uint32_t w = 1; w < tab.length; ++w) {
            std::uint32_t s = 0;
            if (img.readU32(tab.vaddr + 4 * w, s)) starts.insert(s);
        }
    }
    ZeroHost host;
    std::vector<std::vector<std::string>> multiLine;  // messages without '$' for the wrap experiment
    for (std::uint32_t s : starts) {
        const std::int64_t pe = prefixEnd(img, s);
        if (pe < 0) { ++R.msgsUnclean; continue; }
        ++R.msgs;
        std::vector<std::uint8_t> buf;
        for (std::uint32_t i = 0; i < 4096; ++i) { std::uint8_t b = 0; if (!img.readU8(static_cast<std::uint32_t>(pe) + i, b)) break; buf.push_back(b); }
        // find the command byte: first control byte that is not '\n'
        std::size_t endText = 0;
        while (endText < buf.size() && (buf[endText] >= 0x20 || buf[endText] == 10)) ++endText;
        const std::uint8_t cmd = endText < buf.size() ? buf[endText] : 0xFF;
        // page through exactly like the window: scroll 0 -> wait -> +36 ...
        bool ok = cmd <= 0x18 && cmd != 0x0A;
        int scroll = 0;
        int guard = 0;
        bool endedOnCmd = false;
        while (guard++ < 200) {
            const PageResult r = renderEscapeText(buf, 0, TextState{static_cast<std::uint8_t>(scroll & 0xFF), kLimitUnlimited}, host);
            if (r.runaway) { ok = false; break; }
            ++R.pagesTotal;
            if (r.status == PageStatus::ControlByte) { endedOnCmd = (r.cursor == endText); break; }
            if (r.status == PageStatus::LimitReached) { ok = false; break; }
            scroll += 12;  // one line per auto-scroll step; the window waits at multiples of 36
        }
        ok = ok && endedOnCmd;
        if (cmd <= 0x18) ++R.cmdHist[cmd];
        R.elfMsgs.check(ok);
        // escape census + line widths
        std::vector<std::string> lines(1);
        for (std::size_t i = 0; i < endText; ++i) {
            if (buf[i] == 10) { lines.emplace_back(); continue; }
            lines.back().push_back(static_cast<char>(buf[i]));
        }
        bool hasEsc = false;
        for (const std::string& ln : lines) {
            ++R.linesTotal;
            std::size_t esc = ln.find('$');
            if (esc != std::string::npos) {
                ++R.linesWithEsc;
                hasEsc = true;
                for (std::size_t i = 0; i + 1 < ln.size(); ++i) if (ln[i] == '$') ++R.escHist[ln[i + 1]];
                continue;
            }
            if (textWidth(ln) > kTextMaxAdvance) ++R.linesOver;
        }
        if (!hasEsc && lines.size() >= 2) multiLine.push_back(lines);
    }
    for (const auto& kv : R.escHist) R.escElf.check(std::string("AGNRSTU").find(kv.first) != std::string::npos);
    R.lineFit.pass = (R.linesTotal - R.linesWithEsc) - R.linesOver;
    R.lineFit.total = R.linesTotal - R.linesWithEsc;
    for (const auto& lines : multiLine) {
        std::string para;
        for (std::size_t i = 0; i < lines.size(); ++i) { if (i) para.push_back(' '); para += lines[i]; }
        // trailing empty line (text ended with '\n') is dropped for the comparison
        std::vector<std::string> orig = lines;
        while (!orig.empty() && orig.back().empty()) orig.pop_back();
        if (orig.size() < 2 || std::find(orig.begin(), orig.end(), std::string()) != orig.end()) continue;  // single line / authored blank lines: not a wrap case
        ++R.wrapMulti;
        if (wrapGreedy(para) == orig) ++R.wrapSame;
    }
    dg.add(R.msgs); dg.add(R.pagesTotal); dg.add(R.linesOver);

    // ---- oracle pixel extents (needs FONTE for ink bounds; advance-only fallback)
    assets::GlyphAtlas atlas;
    {
        std::ifstream f(fontPath, std::ios::binary);
        if (f) {
            std::vector<std::uint8_t> g((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
            auto res = assets::decodeGlyphAtlas(g);
            if (res.err == assets::GlyphAtlasError::Ok) { atlas = std::move(res.value); R.hadFont = true; }
        }
    }
    if (R.hadFont) {
        for (unsigned c = 0x21; c <= 0x7e; ++c) {
            const int slot = assets::fonteSlotForChar(static_cast<char>(c));
            const bool nonEmpty = slot >= 0 && atlas.slotNonEmpty[static_cast<std::size_t>(slot)];
            R.fontGlyphs.check(nonEmpty);
            if (c >= 'a' && c <= 'z') R.fontLower.check(nonEmpty);
            const GlyphDraw g = drawGlyph(0, 0, 0, static_cast<std::uint8_t>(c), 0x41);
            const int uoff = (16 - tableAdvance(c)) / 2;
            if (c == 0x60 || c == 0x5e || c == 0x7e) continue;  // accent prefixes pick composite cells, never their own
            R.fontSlots.check(g.hasSprite && g.sprite.v == (slot / 32) * 24 && g.sprite.u - uoff == (slot % 32) * 16);
        }
    }
    std::ifstream ex(dir / "text_extents.csv");
    std::string line;
    while (std::getline(ex, line)) {
        if (line.empty() || line[0] == '#') continue;
        const auto t = split(line, ';');
        const std::string& str = t[1];
        const int pen = std::stoi(t[7]);
        const int mx0 = std::stoi(t[8]), mx1 = std::stoi(t[9]);
        const double measW = (mx1 - mx0 + 1) / 2.0;
        const int adv = textWidth(str);
        double predW = adv - 4.0, predX0 = 0;
        int penX = pen == -2 ? 0x23E - adv : pen;
        if (R.hadFont) {
            int inkL = 1 << 20, inkR = -(1 << 20);
            int x = 0;
            for (std::size_t i = 0; i < str.size(); ++i) {
                const GlyphDraw g = drawGlyph(x, 0, 0, static_cast<std::uint8_t>(str[i]), static_cast<std::uint8_t>(i + 1 < str.size() ? str[i + 1] : 0));
                if (g.hasSprite) {
                    for (int row = 0; row < g.sprite.h; ++row) {
                        for (int col = 0; col < g.sprite.w; ++col) {
                            const std::size_t px = (static_cast<std::size_t>(g.sprite.v + row) * atlas.width + static_cast<std::size_t>(g.sprite.u + col)) * 4;
                            if (px + 3 < atlas.pixels.size() && atlas.pixels[px + 3] != 0 && atlas.pixels[px] >= 150) {
                                inkL = std::min(inkL, x + col);
                                inkR = std::max(inkR, x + col);
                            }
                        }
                    }
                }
                x = g.nextX;
            }
            predW = inkR - inkL + 1;
            predX0 = inkL;
        } else {
            predX0 = 1;
        }
        const double diff = predW - measW;
        R.extents.check(std::abs(diff) <= 2.0);
        R.meanDiff += diff;
        R.maxDiff = std::max(R.maxDiff, std::abs(diff));
        if (gVerbose) std::cerr << "  extent " << str << " meas=" << measW << " pred=" << predW << " diff=" << diff << "\n";
        if (pen != -1) {
            const double predStart = penX + predX0;
            const double measStart = mx0 / 2.0;
            R.positions.check(std::abs(predStart - measStart) <= 2.0);
            if (gVerbose) std::cerr << "  position " << str << " meas=" << measStart << " pred=" << predStart << "\n";
        }
    }
    if (R.extents.total) R.meanDiff /= static_cast<double>(R.extents.total);
}

// --------------------------------------------------------------------------
// Part D: `$` escape vectors against an independent expectation (ROM independent)
// --------------------------------------------------------------------------
void runEscapeVectors(Result& R, Fnv& dg) {
    FixtureHost h;
    h.name = "Ann"; h.u = "Gold"; h.g = "Gem"; h.s = 1234567; h.t7 = 54321;
    h.a = {{0, 5000}, {1, 123456}, {2, -4500}};
    h.t = {{0x30, 7 * 9000 + 150 * 5}, {1, -9000}};
    h.course = {"Alpha", "Bravo"};
    auto render = [&](const std::string& s) {
        return renderEscapeText(std::span<const std::uint8_t>(reinterpret_cast<const std::uint8_t*>(s.data()), s.size()), 0, TextState{0, kLimitUnlimited}, h);
    };
    auto plain = [&](const std::string& s) { return eventsHash(render(s).events); };
    auto eq = [&](const std::string& code, const std::string& expandedPlain) {
        const PageResult a = render(code), b = render(expandedPlain);
        return a.events == b.events;
    };
    // $G/$N/$U/$R draw exactly the host string (same x/y/shade as typing it)
    R.escVectors.check(eq("$G", "Gem"));
    R.escVectors.check(eq("$N", "Ann"));
    R.escVectors.check(eq("$U", "Gold"));
    R.escVectors.check(eq("$R", "Alpha"));
    // $A<id>: amount/1000 with C truncation
    R.escVectors.check(eq(std::string("$A\x00", 3), "5"));
    R.escVectors.check(eq(std::string("$A\x01", 3), "123"));
    R.escVectors.check(eq(std::string("$A\x02", 3), "-4"));
    // $T7 and $T<id>: "%d:%02d" of (v/9000, (v%9000)/150)
    R.escVectors.check(eq("$T7", "6:02"));      // 54321 = 6*9000 + 321 -> 321/150 = 2
    R.escVectors.check(eq("$T0", "7:05"));
    R.escVectors.check(eq("$T\x01", "-1:00"));
    // $S: number event, 10-px cells, right-aligned at x + cells*10
    {
        const PageResult r = render("$S");
        const DrawEvent& e = r.events.at(0);
        R.escVectors.check(r.events.size() == 1 && e.kind == DrawEvent::Kind::Number && e.a == kTextOriginX + 9 * 10 && e.b == 346 && e.c == 1234567 && e.d == 10);
    }
    // no-op codes B-F H-M O-Q consume two bytes and draw nothing; one element each
    for (char c : std::string("BCDEFHIJKLMOPQ")) {
        const std::string s = std::string("$") + c + "x";
        const PageResult r = render(s);
        R.escVectors.check(r.events == render("x").events && r.charCount == 2);
    }
    // unknown code (below 'A' / above 'U' / digit / space): consumed as a pair, nothing drawn
    for (char c : std::string("0V z ~")) {
        const std::string s = std::string("$") + c + "x";
        R.escVectors.check(render(s).events == render("x").events);
    }
    // high-bit code byte is skipped with its trail (2-byte step)
    R.escVectors.check(render(std::string("$\x81\x40y")).events == render("y").events);
    // element counting: $A counts twice, everything else once ('$X' = 1)
    R.escVectors.check(render("a$Gb").charCount == 3 && render(std::string("a$A\x00", 5)).charCount == 3 + 0 && render("$T7").charCount == 1);
    // typewriter limit stops BEFORE the element and returns its index
    {
        const PageResult r = renderEscapeText(std::span<const std::uint8_t>(reinterpret_cast<const std::uint8_t*>("abc"), 3), 0, TextState{0, 2}, h);
        R.escVectors.check(r.status == PageStatus::LimitReached && r.cursor == 2 && r.events.size() == 2);
    }
    // page end records the character count into an unlimited limit only on page-aligned scroll
    {
        const std::string s = "a\nb\nc\nd";
        PageResult r = renderEscapeText(std::span<const std::uint8_t>(reinterpret_cast<const std::uint8_t*>(s.data()), s.size()), 0, TextState{0, kLimitUnlimited}, h);
        R.escVectors.check(r.status == PageStatus::PageComplete && r.newLimit == 3);
        r = renderEscapeText(std::span<const std::uint8_t>(reinterpret_cast<const std::uint8_t*>(s.data()), s.size()), 0, TextState{12, kLimitUnlimited}, h);
        R.escVectors.check(r.status == PageStatus::ControlByte);  // lines b c d then the NUL
    }
    (void)plain;
    dg.add(R.escVectors.pass);
}

// Layout facts that must hold by construction (window geometry, command table).
void runStructure(Result& R) {
    R.frames.check(kWindowTop == 328 && kWindowTop + kWindowH == 432 && kTextMaxAdvance == 498 && kLinePitchPx == 24);
    R.frames.check(commandInfo(0x0A) == nullptr && commandInfo(0x19) == nullptr && commandInfo(0x18) != nullptr);
    for (std::size_t i = 0; i < kCommandCount; ++i) R.frames.check(kCommands[i].cmd == i);
}

Result runAll(const std::filesystem::path& dir, const std::filesystem::path& vectors, const Elf* elf,
              const assets::ScriptImage* img, const std::filesystem::path& font) {
    Result R;
    Fnv dg;
    runVectors(vectors, R, dg);
    runFuzz(R, dg);
    runEscapeVectors(R, dg);
    runStructure(R);
    if (elf && img) {
        R.hadElf = true;
        runElf(*elf, *img, font, dir, R, dg);
    }
    R.digest = dg.h;
    return R;
}

}  // namespace

int main(int argc, char** argv) {
    std::string elfPath, dataOverride;
    bool allowSkip = false;
    for (int i = 1; i < argc; ++i) {
        const std::string a = argv[i];
        if (a == "--elf" && i + 1 < argc) elfPath = argv[++i];
        else if (a == "--data" && i + 1 < argc) dataOverride = argv[++i];
        else if (a == "--allow-skip") allowSkip = true;
        else if (a == "--verbose") gVerbose = true;
    }
    (void)allowSkip;
    // RT_ELF overrides --elf (documented harness behaviour).
    if (const char* e = std::getenv("RT_ELF")) elfPath = e;
    if (elfPath.empty()) {
        if (const char* home = std::getenv("HOME")) elfPath = std::string(home) + "/Library/Application Support/RoadTripRecomp/disc/SLUS_203.98";
    }
    const std::filesystem::path dir = dataDir(dataOverride);
    std::error_code ec;
    Elf elf;
    std::optional<assets::ScriptImage> img;
    const bool haveElf = !elfPath.empty() && std::filesystem::exists(elfPath, ec);
    std::filesystem::path font;
    if (haveElf) {
        std::ifstream f(elfPath, std::ios::binary);
        elf.d.assign(std::istreambuf_iterator<char>(f), std::istreambuf_iterator<char>());
        img = assets::ScriptImage::open(elfPath);
        font = std::filesystem::path(elfPath).parent_path() / "SYS" / "FONTE.GSL";
    } else {
        std::cout << "NO_ELF: " << elfPath << " (ROM-dependent checks skipped)\n";
    }
    const std::filesystem::path vectors = dir / "text_script_vectors.txt";
    const Result a = runAll(dir, vectors, haveElf && img ? &elf : nullptr, img ? &*img : nullptr, font);
    const Result b = runAll(dir, vectors, haveElf && img ? &elf : nullptr, img ? &*img : nullptr, font);
    const bool deterministic = a.digest == b.digest;

    if (gVerbose) {
        auto t = [](const char* n, const Tally& x) { std::cerr << "  " << n << " " << x.pass << "/" << x.total << "\n"; };
        t("glyph", a.glyph); t("width", a.width); t("numcells", a.numcells); t("frames+structure", a.frames); t("render", a.render);
        t("runaway(native guard)", a.runaway); t("extract", a.extract); t("copy", a.copy); t("subst", a.subst);
        t("window ticks", a.window); t("window scenarios", a.windowScn); t("escape vectors", a.escVectors); t("fuzz", a.fuzz);
        t("advance", a.advanceTable); t("fullwidth", a.fullwidthTable); t("esc jump", a.escJump); t("elf msgs", a.elfMsgs);
        t("extents", a.extents); t("positions", a.positions); t("lineFit", a.lineFit);
        std::cerr << "  firstFailLine=" << a.firstFailLine << " hadFont=" << a.hadFont << "\n";
        std::cerr << "  msgs=" << a.msgs << " unclean=" << a.msgsUnclean << " pages=" << a.pagesTotal << " lines=" << a.linesTotal
                  << " withEsc=" << a.linesWithEsc << " over498=" << a.linesOver << " wrapSame=" << a.wrapSame << "/" << a.wrapMulti
                  << " meanDiff=" << a.meanDiff << " maxDiff=" << a.maxDiff << " windowTicks=" << a.windowTicks << "\n  esc:";
        for (const auto& kv : a.escHist) std::cerr << ' ' << kv.first << ':' << kv.second;
        std::cerr << "\n  cmd:";
        for (int c = 0; c < 32; ++c) if (a.cmdHist[c]) std::cerr << ' ' << std::hex << c << std::dec << ':' << a.cmdHist[c];
        std::cerr << "\n";
    }

    const bool indep = a.glyph.all() && a.width.all() && a.numcells.all() && a.frames.pass == a.frames.total && a.render.all() &&
                       a.runaway.all() && a.extract.all() && a.copy.all() && a.subst.all() && a.window.all() && a.windowScn.all() &&
                       a.escVectors.pass == a.escVectors.total && a.fuzz.pass == 10000 && a.fuzz.total == 10000;
    bool elfOk = true;
    if (a.hadElf) {
        const double within = a.extents.total ? static_cast<double>(a.extents.pass) / static_cast<double>(a.extents.total) : 0.0;
        elfOk = a.escElf.pass == a.escElf.total && a.escElf.total >= 1 &&
                (!a.hadFont || (a.fontGlyphs.pass == 94 && a.fontLower.pass == 26 && a.fontSlots.pass == 91 && a.fontSlots.total == 91)) && a.advanceTable.all() && a.fullwidthTable.pass == a.fullwidthTable.total && a.escJump.pass == a.escJump.total &&
                a.elfMsgs.pass == a.elfMsgs.total && a.elfMsgs.total > 2000 && within >= 0.90 && a.extents.total >= 8 &&
                a.positions.pass == a.positions.total && a.positions.total >= 2 &&
                a.lineFit.total > 5000 && static_cast<double>(a.lineFit.pass) / static_cast<double>(a.lineFit.total) >= 0.99;
    }
    const bool pass = indep && elfOk && deterministic;

    std::ostringstream sc;
    sc << "W2-07 SCORE ";
    if (a.hadElf) {
        sc << "advance_table=" << a.advanceTable.pass << "/" << a.advanceTable.total;
        char buf[32];
        std::snprintf(buf, sizeof buf, "%.2f", a.extents.total ? static_cast<double>(a.extents.pass) / static_cast<double>(a.extents.total) : 0.0);
        sc << " wrap_oracle_within2px=" << buf << "(" << a.extents.pass << "/" << a.extents.total << ")";
    } else {
        sc << "advance_table=NO_ELF wrap_oracle_within2px=NO_ELF";
    }
    const std::size_t escPass = a.escVectors.pass + (a.hadElf ? a.escJump.pass + a.escElf.pass : 0);
    const std::size_t escTotal = a.escVectors.total + (a.hadElf ? a.escJump.total + a.escElf.total : 0);
    sc << " escapes=" << escPass << "/" << escTotal << " bounded_copy_fuzz=" << a.fuzz.pass << "/" << a.fuzz.total
       << " deterministic=" << (deterministic ? 1 : 0);
    sc << " oracle_vectors=" << (a.glyph.pass + a.width.pass + a.numcells.pass + a.render.pass + a.runaway.pass + a.extract.pass + a.copy.pass + a.subst.pass + a.window.pass)
       << "/" << (a.glyph.total + a.width.total + a.numcells.total + a.render.total + a.runaway.total + a.extract.total + a.copy.total + a.subst.total + a.window.total);
    if (a.hadElf) {
        sc << " elf_msgs=" << a.elfMsgs.pass << "/" << a.elfMsgs.total << " positions=" << a.positions.pass << "/" << a.positions.total
           << " fullwidth=" << a.fullwidthTable.pass << "/" << a.fullwidthTable.total;
        if (a.hadFont) sc << " font_glyphs=" << a.fontGlyphs.pass << "/" << a.fontGlyphs.total << " font_lower=" << a.fontLower.pass << "/" << a.fontLower.total << " font_slots=" << a.fontSlots.pass << "/" << a.fontSlots.total;
        else sc << " font=NO_FONTE";
    }
    std::cout << sc.str() << "\n";
    return pass ? 0 : 1;
}
