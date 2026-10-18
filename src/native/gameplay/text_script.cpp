#include "gameplay/text_script.h"

#include <algorithm>

namespace rt::native::gameplay::text {

// uiGlyphWidths 0x29A880..0x29A8DF (c = 0x20..0x7F), read from the ELF.
const std::array<std::int8_t, kAdvanceCount> kAdvanceTable = {
    8, 16, 6, 16, 16, 16, 16, 6, 8, 10, 16, 16, 10, 10, 12, 16,
    12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 16, 16, 16, 16, 16, 16,
    16, 15, 14, 15, 16, 12, 12, 16, 14, 12, 16, 16, 16, 16, 16, 16,
    14, 16, 14, 14, 14, 16, 16, 16, 16, 14, 16, 16, 16, 16, 16, 16,
    16, 14, 14, 14, 14, 14, 14, 14, 14, 8, 14, 14, 8, 16, 12, 13,
    14, 14, 12, 12, 12, 12, 12, 14, 14, 14, 13, 16, 16, 16, 16, 16,
};

int tableAdvance(unsigned c) {
    if (c < kAdvanceFirst || c > kAdvanceLast) return 0;
    return kAdvanceTable[c - kAdvanceFirst];
}

namespace {

// C `/` on ints (truncation toward zero) is what the MIPS code does.
inline int cdiv(int a, int b) { return a / b; }

inline std::int16_t s16(long v) { return static_cast<std::int16_t>(static_cast<std::uint16_t>(v & 0xFFFF)); }

// Sprite for an ASCII glyph whose atlas selector `n` is the byte the table is
// indexed with (uiDrawGlyph's uVar4).
GlyphDraw asciiGlyph(int x, int y2, int shade, unsigned n, int u, int v) {
    const int w = static_cast<std::int8_t>(tableAdvance(n));
    GlyphDraw g;
    g.hasSprite = true;
    // u + (0x10 - w)/2 ; width = (w + (w>>7)) >> 1) + 8 with w sign-extended.
    const int uoff = cdiv(0x10 - w, 2);
    const int ww = ((w >> 1) + 8);  // table widths are 0..16, so the (w>>7) sign term is 0
    g.sprite = {s16(x), s16(y2), s16(u + uoff), s16(v), s16(ww), s16(0x18 - shade)};
    g.nextX = x + w;
    return g;
}

}  // namespace

GlyphDraw drawGlyph(int x, int y2, int shade, std::uint8_t c0, std::uint8_t c1) {
    GlyphDraw g;
    if (c0 > 0x7f) {
        const unsigned code = (static_cast<unsigned>(c0) << 8) | c1;
        if (code == 0x8140) {
            g.nextX = x + kIdeographicSpaceAdvance;
            return g;
        }
        if (code == 0x818b) {
            g.hasSprite = true;
            g.sprite = {s16(x), s16(y2), 0xC0, 0x60, 0x10, s16(0x18 - shade)};
            g.nextX = x + kWideDotAdvance;
            return g;
        }
        g.hasSprite = true;
        g.kanjiUvUnmodelled = true;
        g.sprite = {s16(x), s16(y2 + 2), 0, 0, 0x14, s16(0x14 - shade)};
        g.nextX = x + kWideAdvance;
        return g;
    }
    if (c0 == 0x20) {
        g.nextX = x + kSpaceAdvance;
        return g;
    }
    if (c0 == 0x2f) {  // '/' ; "/e" is e-acute
        if (c1 == 0x65) return asciiGlyph(x, y2, shade, 0x65, 0x60, 0x90);
        return asciiGlyph(x, y2, shade, 0x2f, 0xE0, 0);
    }
    if (c0 == 0x60) {  // '`' grave: "`e" "`a" "`u"; others fall to the default cell
        const unsigned n = c1;
        if (n == 0x65) return asciiGlyph(x, y2, shade, n, 0x70, 0x90);
        if (n < 0x66) {
            if (n == 0x61) return asciiGlyph(x, y2, shade, n, 0x1E0, 0x78);
            return asciiGlyph(x, y2, shade, n, 0x1B0, 0x60);
        }
        if (n == 0x75) return asciiGlyph(x, y2, shade, n, 0x180, 0x90);
        return asciiGlyph(x, y2, shade, n, 0x1B0, 0x60);
    }
    if (c0 == 0x5e) {  // '^' circumflex
        const unsigned n = c1;
        int u = 0x1B0, v = 0x60;
        switch (n) {
            case 0x61: u = 0x000; v = 0x90; break;
            case 0x65: u = 0x080; v = 0x90; break;
            case 0x69: u = 0x0C0; v = 0x90; break;
            case 0x6F: u = 0x120; v = 0x90; break;
            case 0x75: u = 0x190; v = 0x90; break;
            default: break;
        }
        return asciiGlyph(x, y2, shade, n, u, v);
    }
    if (c0 == 0x7e) {  // '~' tilde / cedilla group
        const unsigned n = c1;
        int u = 0x1B0, v = 0x60;
        switch (n) {
            case 0x41: u = 0x010; v = 0x78; break;
            case 0x43: u = 0x040; v = 0x78; break;
            case 0x4F: u = 0x130; v = 0x78; break;
            case 0x52: u = 0x0A0; v = 0x60; break;
            case 0x55: u = 0x190; v = 0x78; break;
            case 0x61: u = 0x020; v = 0x90; break;
            case 0x63: u = 0x050; v = 0x90; break;
            case 0x65: u = 0x090; v = 0x90; break;
            case 0x69: u = 0x0D0; v = 0x90; break;
            case 0x6F: u = 0x130; v = 0x90; break;
            case 0x73: u = 0x1C0; v = 0x78; break;
            case 0x75: u = 0x1A0; v = 0x90; break;
            default: break;
        }
        return asciiGlyph(x, y2, shade, n, u, v);
    }
    // Plain cell: g = c - 0x21, 31 glyphs per 24-px row.
    const int gi = static_cast<int>(c0) - 0x21;
    return asciiGlyph(x, y2, shade, c0, (gi % 0x1f) << 4, (gi / 0x1f) * 0x18);
}

int textWidth(std::span<const std::uint8_t> s) {
    int w = 0;
    std::size_t i = 0;
    auto at = [&](std::size_t k) -> std::uint8_t { return k < s.size() ? s[k] : 0; };
    while (at(i) != 0) {
        const std::uint8_t c = at(i);
        if (c == 0x7e || c == 0x5e || c == 0x2f || c == 0x60) {
            w += tableAdvance(at(i + 1));
            i += 2;
        } else if (c & 0x80) {
            w += kWideAdvance;
            i += 2;
        } else {
            w += tableAdvance(c);
            i += 1;
        }
    }
    return w;
}

int textWidth(std::string_view s) {
    return textWidth(std::span<const std::uint8_t>(reinterpret_cast<const std::uint8_t*>(s.data()), s.size()));
}

int numberAutoCells(std::int64_t v) {
    if (v >= 1000000000) return 0xd;
    if (v >= 100000000) return 0xb;
    if (v >= 10000000) return 10;
    if (v >= 1000000) return 9;
    if (v >= 100000) return 7;
    if (v >= 10000) return 6;
    if (v >= 1000) return 5;
    int n = 1;
    if (v > 9) n = 2;
    if (v > 99) n = 3;
    return n;
}

DrawEvent spriteEvent(const Sprite& s) {
    DrawEvent e;
    e.kind = DrawEvent::Kind::Sprite;
    e.a = s.x; e.b = s.y; e.c = s.u; e.d = s.v; e.e = s.w; e.f = s.h;
    return e;
}

// ---------------------------------------------------------------------------
// uiDrawTextShade / uiDrawText (string drawers)
// ---------------------------------------------------------------------------
namespace {

// Draws `s` glyph by glyph from pen `x`; y2/shade already in glyph units.
int drawStringGlyphs(std::vector<DrawEvent>& out, int x, int y2, int shade, std::span<const std::uint8_t> s) {
    std::size_t i = 0;
    auto at = [&](std::size_t k) -> std::uint8_t { return k < s.size() ? s[k] : 0; };
    while (i < s.size() && at(i) != 0) {
        const GlyphDraw g = drawGlyph(x, y2, shade, at(i), at(i + 1));
        if (g.hasSprite) out.push_back(spriteEvent(g.sprite));
        x = g.nextX;
        i += static_cast<std::size_t>(stringStep(at(i)));
    }
    return x;
}

std::span<const std::uint8_t> bytes(std::string_view s) {
    return {reinterpret_cast<const std::uint8_t*>(s.data()), s.size()};
}

}  // namespace

std::vector<DrawEvent> drawTextEvents(int x, int yArg, std::span<const std::uint8_t> s, int shade) {
    std::vector<DrawEvent> out;
    drawStringGlyphs(out, x, yArg * 2, shade, s);
    return out;
}

// ---------------------------------------------------------------------------
// RenderEscapeText 0x23A8E8
// ---------------------------------------------------------------------------
namespace {

std::string formatClock(std::int32_t v) {
    // sprintf("%d:%02d", v / 9000, (v % 9000) / 0x96) with C signed division.
    const int h = v / 9000;
    const int m = (v % 9000) / 0x96;
    std::string s = std::to_string(h) + ":";
    if (m >= 0 && m < 10) s += "0";
    s += std::to_string(m);
    return s;
}

}  // namespace

PageResult renderEscapeText(std::span<const std::uint8_t> buf, std::size_t cursor, TextState st,
                            const IEscapeHost& host) {
    PageResult R;
    auto at = [&](std::size_t i) -> std::uint8_t { return i < buf.size() ? buf[i] : 0; };
    auto neg = [](std::uint8_t c) -> int { return (c & 0x80) ? 1 : 0; };
    std::size_t p = cursor;
    int s4 = 0;  // element counter (lVar6)

    // --- skip `scroll/12` lines (0x23A92C..0x23A9B0) -----------------------
    const int skipLines = st.scroll / kLineUnits;
    if (skipLines != 0) {
        // The original has no end check here: with fewer lines than scroll/12 (or
        // a text that ends right after its last newline) the cursor runs on through
        // zero bytes forever.  Native bound: stop once the cursor is past the buffer.
        auto runaway = [&]() {
            if (p <= buf.size()) return false;
            R.status = PageStatus::ControlByte;
            R.cursor = buf.size();
            R.charCount = s4;
            R.newLimit = st.limit;
            R.readPastEnd = true;
            R.runaway = true;
            return true;
        };
        int s5 = skipLines - 1;
        std::uint8_t a0 = at(p);
        for (;;) {  // 0x23A950
            if (a0 < 0x20) {
                ++p;  // 0x23A958 (delay slot of the likely branch)
            } else {
                do {  // 0x23A960
                    ++s4;
                    p += static_cast<std::size_t>(neg(a0)) + 1;
                    a0 = at(p);
                } while (a0 >= 0x20);
                ++p;  // 0x23A990
            }
            if (runaway()) return R;
            a0 = at(p);  // 0x23A994
            for (;;) {
                if (a0 == 10) { --s5; break; }          // 0x23A9AC
                if (a0 >= 0x20) { --s5; break; }        // 0x23A988 taken
                ++p;                                    // 0x23A990
                if (runaway()) return R;
                a0 = at(p);
            }
            if (s5 == -1) break;  // 0x23A9B0
        }
    }

    // --- draw up to three lines (0x23A9B8..) ---------------------------------
    const int k = st.scroll % kLineUnits;
    int s1 = std::max(0, k - 4);           // shade/offset of the first line
    int s3 = kTextOriginUnitsY - k;        // y base in half-pixel units
    int s5 = 0;                            // lines drawn
    for (;;) {
        int x = kTextOriginX;
        std::uint8_t b = at(p);
        while (b >= 0x20) {
            if (s4 == st.limit) {
                R.status = PageStatus::LimitReached;
                R.cursor = p;
                R.charCount = s4;
                R.newLimit = st.limit;
                R.readPastEnd = p >= buf.size();
                return R;
            }
            const int y2 = (s3 + s1) * 2;
            if (b != 0x24) {
                const GlyphDraw g = drawGlyph(x, y2, s16(s1 * 2), b, at(p + 1));
                if (g.hasSprite) R.events.push_back(spriteEvent(g.sprite));
                x = g.nextX;
                p += static_cast<std::size_t>(neg(b)) + 1;
                ++s4;
                b = at(p);
                continue;
            }
            // '$' code
            std::size_t q = p + 1;  // at the code byte
            const std::uint8_t code = at(q);
            std::string tmp;
            auto drawStr = [&](std::string_view s) {
                x = drawStringGlyphs(R.events, x, y2, s16(s1 * 2), bytes(s));
            };
            const unsigned idx = static_cast<unsigned>(code) - 0x41u;
            if (idx < 0x15) {
                switch (code) {
                    case 0x41: {  // $A<id>: amount / 1000
                        q = p + 2;
                        ++s4;
                        const std::int32_t w = host.amountWord(at(q));
                        tmp = std::to_string(w / 1000);
                        drawStr(tmp);
                        break;
                    }
                    case 0x47: drawStr(host.stringG()); break;
                    case 0x4e: drawStr(host.playerName()); break;
                    case 0x52: drawStr(host.courseName()); break;
                    case 0x53: {
                        const std::int64_t v = host.bankBalance();
                        const int cells = numberAutoCells(v);
                        x += cells * 10;
                        DrawEvent e;
                        e.kind = DrawEvent::Kind::Number;
                        e.a = x; e.b = y2; e.c = v; e.d = 10;
                        R.events.push_back(e);
                        break;
                    }
                    case 0x54: {  // $T<id>
                        q = p + 2;
                        const std::uint8_t id = at(q);
                        drawStr(formatClock(host.clockWord(id)));
                        break;
                    }
                    case 0x55: drawStr(host.stringU()); break;
                    default: break;  // B-F, H-M, O-Q: consumed, no output
                }
            }
            p = q + static_cast<std::size_t>(neg(at(q))) + 1;
            ++s4;
            b = at(p);
        }
        if (b != 10) {
            R.status = PageStatus::ControlByte;
            R.cursor = p;
            R.charCount = s4;
            R.newLimit = st.limit;
            R.readPastEnd = p >= buf.size();
            return R;
        }
        ++s5;
        ++p;
        s3 += kLineUnits;
        s1 = 0;
        if (s5 >= kLinesPerPage) break;
    }
    R.status = PageStatus::PageComplete;
    R.charCount = s4;
    R.newLimit = st.limit;
    if ((st.scroll == 0 || (st.scroll / kLineUnits) % 3 == 0) && st.limit == kLimitUnlimited) {
        R.newLimit = s16(s4);
    }
    return R;
}

// ---------------------------------------------------------------------------
// Frame
// ---------------------------------------------------------------------------
std::vector<Sprite> frameASprites(int y) {
    return {
        {0x20, s16(y - 0x10), 0x00, 0x100, 0x28, 0x38},
        {0x20, s16(y + 0x28), 0x78, 0x100, 0x28, 0x30},
        {0x48, s16(y - 0x10), 0x50, 0x100, 0x1f0, 0x38},
        {0x48, s16(y + 0x28), 200, 0x100, 0x1f0, 0x30},
        {0x238, s16(y - 0x10), 0x28, 0x100, 0x28, 0x38},
        {0x238, s16(y + 0x28), 0xa0, 0x100, 0x28, 0x30},
        {0x130, s16(y + 0x48), 0x58, 0x100, 0x20, 0x18},
    };
}

std::vector<Sprite> frameBSprites(int w, int y) {
    return {
        {s16(0x232 - w), s16(y - 0x1c), 0x58, 0x118, 8, 0x1c},
        {s16(0x23a - w), s16(y - 0x1c), 0x60, 0x118, s16(w + 4), 0x1c},
        {0x23e, s16(y - 0x1c), 0xd0, 0x100, 0x18, 0x1c},
    };
}

Sprite icon20Sprite(int x, int y, int index) {
    return {s16(x), s16(y), s16((index * 0x140000 + 0x780000) >> 16), 0xb4, 0x14, 0x14};
}

// ---------------------------------------------------------------------------
// Commands (dialogueTask switch on the byte RenderEscapeText stopped at)
// ---------------------------------------------------------------------------
const std::array<CommandInfo, kCommandCount> kCommands = {{
    {0x00, "wait-close", CmdKind::CloseWait, 0, false, 0},
    {0x01, "menu-message", CmdKind::Spawn, 0x239260, false, 0},
    {0x02, "yes-no-menu", CmdKind::Spawn, 0x23BC90, false, 0},
    {0x03, "close-set-ctx20", CmdKind::CloseWait, 0, false, 1},
    {0x04, "child-0x239900-9", CmdKind::SpawnFixed, 0x239900, false, 1},
    {0x05, "child-0x23a320", CmdKind::Spawn, 0x23A320, false, 0},
    {0x06, "child-0x239900-8", CmdKind::SpawnFixed, 0x239900, false, 1},
    {0x07, "gated-child-0x239f68", CmdKind::Spawn, 0x239F68, true, 0},
    {0x08, "child-0x239a80", CmdKind::Spawn, 0x239A80, false, 0},
    {0x09, "tab-list-menu", CmdKind::Spawn, 0x23BCD0, false, 0},
    {0x0A, "typewriter-default", CmdKind::Spawn, 0, false, 0},  // never dispatched (not in the switch)
    {0x0B, "play-sound-cue", CmdKind::Spawn, 0x23BE88, false, 0},
    {0x0C, "gated-goto", CmdKind::Goto, 0, true, 1},
    {0x0D, "award-stamps", CmdKind::Stamps, 0, false, 0},
    {0x0E, "spawn-0x229538-goto", CmdKind::Goto, 0x229538, false, 1},
    {0x0F, "close-result-6", CmdKind::CloseWait, 0, false, 0},
    {0x10, "sequenced-messages", CmdKind::Spawn, 0x239748, false, 0},
    {0x11, "photo-goto", CmdKind::Goto, 0x22BFD0, true, 2},
    {0x12, "text-box", CmdKind::Spawn, 0x23BE18, false, 0},
    {0x13, "gated-spawn-0x269e88-goto", CmdKind::Goto, 0x269E88, true, 1},
    {0x14, "warp", CmdKind::Warp, 0, false, 2},
    {0x15, "set-attr-goto", CmdKind::Goto, 0, true, 3},
    {0x16, "gated-prize-screen", CmdKind::Spawn, 0x23A140, true, 0},
    {0x17, "amount-adjuster", CmdKind::Spawn, 0x23A460, false, 0},
    {0x18, "close-result-0xf", CmdKind::CloseWait, 0, false, 0},
}};

const CommandInfo* commandInfo(std::uint8_t cmd) {
    if (cmd >= kCommandCount || cmd == 0x0A) return nullptr;
    return &kCommands[cmd];
}

// ---------------------------------------------------------------------------
// MessageWindow (dialogueTask 0x23B168)
// ---------------------------------------------------------------------------
MessageWindow::MessageWindow(const ITextMemory& mem, const IEscapeHost& host, std::uint32_t table,
                             std::function<std::uint32_t(std::uint32_t)> runPrefix)
    : mem_(mem), host_(host), table_(table), runPrefix_(std::move(runPrefix)) {}

std::uint8_t MessageWindow::byteAt(std::uint32_t v) const {
    std::uint8_t b = 0;
    mem_.readU8(v, b);
    return b;
}

std::uint32_t MessageWindow::branch(std::uint32_t idx) const {
    std::uint32_t v = 0;
    mem_.readU32(table_ + idx * 4, v);
    return v;
}

std::string MessageWindow::readName() const {
    std::uint32_t a = 0;
    mem_.readU32(table_, a);
    std::string s;
    std::uint8_t b = 0;
    for (int i = 0; i < 256 && mem_.readU8(a + static_cast<std::uint32_t>(i), b) && b != 0; ++i) {
        s.push_back(static_cast<char>(b));
    }
    return s;
}

void MessageWindow::signalChildDone(std::int32_t result) {
    childDone_ = true;
    result_ = result;
}

void MessageWindow::nextMsg(TickOutput& out, std::uint32_t target) {
    cursor_ = target;
    out.effects.push_back({Effect::Kind::NextMsg, target, 0});
    scroll_ = 0;
    limit_ = 0;
}

void MessageWindow::drawFrame(TickOutput& out, int plateWidth) {
    for (const Sprite& s : frameBSprites(plateWidth)) out.events.push_back(spriteEvent(s));
}

void MessageWindow::render(TickOutput& out, bool confirm) {
    cursor_ = runPrefix_ ? runPrefix_(cursor_) : cursor_;
    for (const Sprite& s : frameASprites()) out.events.push_back(spriteEvent(s));

    // Text bytes from the cursor onward (bounded read; zero past a failed read).
    std::vector<std::uint8_t> buf;
    buf.reserve(4096);
    for (std::uint32_t i = 0; i < 4096; ++i) {
        std::uint8_t b = 0;
        if (!mem_.readU8(cursor_ + i, b)) break;
        buf.push_back(b);
    }
    const PageResult page = renderEscapeText(buf, 0, TextState{scroll_, limit_}, host_);
    out.events.insert(out.events.end(), page.events.begin(), page.events.end());
    limit_ = page.newLimit;

    const std::string name = readName();
    const int w = textWidth(name);
    drawFrame(out, w);
    for (const DrawEvent& e : drawTextEvents(kPlateRightX - w, kPlateTextY, bytes(name))) out.events.push_back(e);

    if (state_ != WinState::Text) return;

    auto arrow = [&]() {
        if (tick_ & 0x10) out.events.push_back(spriteEvent(icon20Sprite(kArrowIconX, kArrowIconY, kArrowIconIndex)));
    };
    auto sound = [&](std::uint32_t id) { out.effects.push_back({Effect::Kind::Sound, id, 0}); };

    if (page.status == PageStatus::PageComplete) {
        if (scroll_ == 0 || (scroll_ / kLineUnits) % 3 == 0) {
            arrow();
            if (!confirm) return;
            sound(0x1a);
        }
        state_ = WinState::Scroll;
        return;
    }

    const std::uint32_t p = cursor_ + static_cast<std::uint32_t>(page.cursor);
    const std::uint8_t cmd = byteAt(p);
    auto arg = [&](std::uint32_t i) { return byteAt(p + 1 + i); };
    auto spawn = [&](std::uint32_t fn, std::uint32_t a) {
        out.effects.push_back({Effect::Kind::SpawnChild, fn, a});
        state_ = WinState::ChildMenu;
    };
    auto gate = [&](bool withArrow) -> bool {
        if (withArrow) arrow();
        if (!confirm) return false;
        sound(0x1a);
        return true;
    };

    switch (cmd) {
        case 0x01: spawn(0x239260, p + 1); return;
        case 0x02: spawn(0x23BC90, p + 1); return;
        case 0x03:
            if (!confirm) return;
            tick_ = 0;
            state_ = WinState::Close;
            sound(0x26);
            pend_ = 4;
            if (arg(0) != 0) out.effects.push_back({Effect::Kind::CtxByte20, arg(0), 0});
            return;
        case 0x04:
        case 0x06:
            out.effects.push_back({Effect::Kind::SpawnChild, 0x239900, cmd == 0x04 ? 9u : 8u});
            state_ = WinState::ChildResult;
            pend_ = arg(0);
            return;
        case 0x05: spawn(0x23A320, p + 1); return;
        case 0x07:
            if (!gate(true)) return;
            spawn(0x239F68, p + 1);
            return;
        case 0x08: spawn(0x239A80, p + 1); return;
        case 0x09: spawn(0x23BCD0, p + 1); return;
        case 0x0B: spawn(0x23BE88, p + 1); return;
        case 0x0C:
            if (!gate(true)) return;
            nextMsg(out, branch(arg(0)));
            return;
        case 0x0D:
            if (!confirm) return;
            tick_ = 0;
            state_ = WinState::Close;
            sound(0x26);
            for (std::uint32_t i = 0;; ++i) {
                const std::uint8_t id = arg(i);
                if (id == 0) break;
                out.effects.push_back({Effect::Kind::AwardStamp, id, 0});
            }
            return;
        case 0x0E:
            out.effects.push_back({Effect::Kind::TownVar, 0, 0});  // a patched by the host (ctx+0x23)
            out.effects.push_back({Effect::Kind::SpawnChild, 0x229538, 0});
            nextMsg(out, branch(arg(0)));
            state_ = WinState::ChildNext;
            return;
        case 0x0F:
            pend_ = 6;
            [[fallthrough]];
        case 0x00:
            if (!confirm) return;
            tick_ = 0;
            state_ = WinState::Close;
            sound(0x26);
            return;
        case 0x10: spawn(0x239748, p + 1); return;
        case 0x11:
            if (!gate(true)) return;
            out.effects.push_back({Effect::Kind::TakePhoto, arg(0), 0});
            out.effects.push_back({Effect::Kind::SpawnChild, 0x22BFD0, (arg(0) - 1u) | 0x10000u});
            nextMsg(out, branch(arg(1)));
            sound(0x306);
            state_ = WinState::ChildNext;
            return;
        case 0x12: spawn(0x23BE18, p + 1); return;
        case 0x13:
            if (!gate(true)) return;
            out.effects.push_back({Effect::Kind::SpawnChild, 0x269E88, 0});
            nextMsg(out, branch(arg(0)));
            state_ = WinState::ChildNext;
            return;
        case 0x14:
            if (!confirm) return;
            tick_ = 0;
            state_ = WinState::Close;
            sound(0x26);
            out.effects.push_back({Effect::Kind::ReleaseStamp, 0, 0});
            pend_ = 0x0b;
            out.effects.push_back({Effect::Kind::WarpTo, arg(0), arg(1)});
            return;
        case 0x15:
            if (!gate(true)) return;
            out.effects.push_back({Effect::Kind::SetObjectAttr, arg(0), arg(1)});
            sound(0x303);
            nextMsg(out, branch(arg(2)));
            return;
        case 0x16:
            if (!gate(true)) return;
            spawn(0x23A140, p + 1);
            return;
        case 0x17: spawn(0x23A460, p + 1); return;
        case 0x18:
            if (!confirm) return;
            tick_ = 0;
            state_ = WinState::Close;
            sound(0x26);
            pend_ = 0x0f;
            return;
        default: {  // typewriter (printable stop, 0x0A, and bytes >= 0x19)
            if ((tick_ & 1) == 0) {
                ++limit_;
                if ((tick_ & 3) == 0) {
                    sound(0x23);
                    out.effects.push_back({Effect::Kind::TalkAnim, 2, 0});
                }
            }
            if (confirm) limit_ = kLimitUnlimited;
            return;
        }
    }
}

TickOutput MessageWindow::tick(bool confirm) {
    TickOutput out;
    auto finish = [&]() {
        childDone_ = false;  // task walker clears +0x24 after every callback
        ++tick_;
    };

    if (tick_ == 0) {
        mem_.readU32(table_ + 4, cursor_);
        out.effects.push_back({Effect::Kind::Sound, 0x25, 0});
    }

    switch (state_) {
        case WinState::Open: {
            DrawEvent t;
            t.kind = DrawEvent::Kind::Texa;
            t.a = static_cast<std::int32_t>(tick_) << 3;
            out.events.push_back(t);
            for (const Sprite& s : frameASprites()) out.events.push_back(spriteEvent(s));
            const int w = textWidth(readName()) * static_cast<int>(tick_);
            drawFrame(out, w >> 4);
            if (tick_ == 0x10) state_ = WinState::Text;
            finish();
            return out;
        }
        case WinState::Close: {
            DrawEvent e;
            e.kind = DrawEvent::Kind::Texa;
            e.a = static_cast<std::int32_t>(tick_) * -8 + 0x80;
            out.events.push_back(e);
            for (const Sprite& s : frameASprites()) out.events.push_back(spriteEvent(s));
            drawFrame(out, textWidth(readName()));
            if (tick_ == 0x10) {
                out.effects.push_back({Effect::Kind::CtxFlag400, pend_ == 4 ? 0u : 1u, 0});
                if (pend_ != 0xFF) {
                    out.effects.push_back({Effect::Kind::ParentDone, pend_ == 0 ? 5u : static_cast<std::uint32_t>(pend_), 0});
                }
                out.effects.push_back({Effect::Kind::Kill, 0, 0});
                killed_ = true;
            }
            finish();
            return out;
        }
        case WinState::Scroll:
            scroll_ = static_cast<std::uint8_t>(scroll_ + 2);
            if (scroll_ % kLineUnits == 0) state_ = WinState::Text;
            break;
        case WinState::ChildMenu:
            if (childDone_) {
                state_ = WinState::Text;
                nextMsg(out, branch(static_cast<std::uint32_t>(result_)));
            }
            break;
        case WinState::ChildResult:
            if (childDone_) {
                if (result_ != -1) {
                    out.effects.push_back({Effect::Kind::ParentDone, static_cast<std::uint32_t>(result_), 0});
                    pend_ = 0xFF;
                    state_ = WinState::Close;
                    tick_ = 0;
                    out.effects.push_back({Effect::Kind::Sound, 0x26, 0});
                    finish();
                    return out;
                }
                state_ = WinState::Text;
                nextMsg(out, branch(pend_));
                pend_ = 0;
            }
            break;
        case WinState::ChildNext:
            if (!childDone_) {
                finish();
                return out;
            }
            state_ = WinState::Text;
            break;
        case WinState::Text:
        default:
            break;
    }
    render(out, confirm);
    finish();
    return out;
}

// ---------------------------------------------------------------------------
// String utilities
// ---------------------------------------------------------------------------
std::uint16_t fullwidthSjis(std::uint8_t c) {
    if (c >= 0x30 && c <= 0x39) return static_cast<std::uint16_t>(0x824F + (c - 0x30));
    if (c >= 0x41 && c <= 0x5A) return static_cast<std::uint16_t>(0x8260 + (c - 0x41));
    if (c >= 0x61 && c <= 0x7A) return static_cast<std::uint16_t>(0x8281 + (c - 0x61));
    // Punctuation rows read from the ELF (0x2A9A82.., 0x2A9AB6.., 0x2A9AEA.., 0x2A9AFE..).
    static constexpr std::uint16_t kLow[16] = {0x8140, 0x8149, 0x8168, 0x8194, 0x8190, 0x8193, 0x8195, 0x8166,
                                                0x8169, 0x816a, 0x8196, 0x817b, 0x8143, 0x817c, 0x8144, 0x815e};
    static constexpr std::uint16_t kColon[7] = {0x8146, 0x8147, 0x8183, 0x8181, 0x8184, 0x8148, 0x8197};
    static constexpr std::uint16_t kBracket[6] = {0x816d, 0x818f, 0x816e, 0x814f, 0x8151, 0x8165};
    static constexpr std::uint16_t kBrace[4] = {0x816f, 0x8162, 0x8170, 0x8160};
    if (c >= 0x20 && c <= 0x2f) return kLow[c - 0x20];
    if (c >= 0x3a && c <= 0x40) return kColon[c - 0x3a];
    if (c >= 0x5b && c <= 0x60) return kBracket[c - 0x5b];
    if (c >= 0x7b && c <= 0x7e) return kBrace[c - 0x7b];
    return 0x8140;  // DAT_00334498
}

std::string copyDialogueTextBounded(std::span<const std::uint8_t> src, int limit, std::size_t dstCapacity,
                                    int* charsWritten) {
    std::string out;
    int written = 0;
    auto at = [&](std::size_t i) -> std::uint8_t { return i < src.size() ? src[i] : 0; };
    std::size_t i = 0;
    int remaining = limit;
    while (true) {
        const std::uint8_t c = at(i);
        if (limit == 0) {
            if (c == 0) break;
        } else {
            if (remaining-- <= 0) break;
        }
        if (out.size() + 3 > dstCapacity) break;  // two bytes + terminator must fit
        if (c & 0x80) {
            out.push_back(static_cast<char>(c));
            out.push_back(static_cast<char>(at(i + 1)));
            i += 2;
        } else {
            const std::uint16_t f = fullwidthSjis(c);
            out.push_back(static_cast<char>(f >> 8));
            out.push_back(static_cast<char>(f & 0xFF));
            i += 1;
        }
        ++written;
    }
    if (charsWritten) *charsWritten = written;
    return out;
}

bool substituteNameTokens(std::string& text, std::string_view name, std::size_t capacity) {
    std::size_t pos = 0;
    while (pos < text.size()) {
        if (text[pos] == '$' && pos + 1 < text.size() && text[pos + 1] == 'N') break;
        ++pos;
    }
    if (pos >= text.size()) return false;
    const std::size_t newLen = pos + name.size() + (text.size() - (pos + 2));
    if (newLen + 1 > capacity) return false;
    text.replace(pos, 2, name);
    return true;
}

bool extractDelimitedToken(std::string_view s, std::string_view delims, int k, std::string& out) {
    out.clear();
    std::size_t i = 0;
    auto isDelim = [&](char c) { return delims.find(c) != std::string_view::npos && c != 0; };
    if (k + 1 > 0) {
        for (int group = 0; group < k + 1; ++group) {
            // skip non-delimiters
            while (true) {
                if (i >= s.size()) return false;
                if (isDelim(s[i])) break;
                ++i;
            }
            // skip the delimiter run
            while (i < s.size() && isDelim(s[i])) ++i;
            if (i >= s.size()) return false;
        }
    }
    while (i < s.size() && !isDelim(s[i])) out.push_back(s[i++]);
    return true;
}

std::vector<std::string> wrapGreedy(std::string_view text, int maxAdvance) {
    std::vector<std::string> lines;
    std::string line;
    int lineW = 0;
    std::string word;
    int wordW = 0;
    auto adv = [](unsigned char c) { return c == 0x20 ? kSpaceAdvance : (c & 0x80 ? kWideAdvance : tableAdvance(c)); };
    auto flushWord = [&]() {
        if (word.empty()) return;
        const int spaceW = line.empty() ? 0 : kSpaceAdvance;
        if (!line.empty() && lineW + spaceW + wordW > maxAdvance) {
            lines.push_back(line);
            line.clear();
            lineW = 0;
        }
        if (!line.empty()) {
            line.push_back(' ');
            lineW += kSpaceAdvance;
        }
        line += word;
        lineW += wordW;
        word.clear();
        wordW = 0;
    };
    for (std::size_t i = 0; i < text.size(); ++i) {
        const unsigned char c = static_cast<unsigned char>(text[i]);
        if (c == '\n') {
            flushWord();
            lines.push_back(line);
            line.clear();
            lineW = 0;
            continue;
        }
        if (c == ' ') {
            flushWord();
            continue;
        }
        if (c == '$' && i + 1 < text.size()) {  // escape code: measured as zero width, kept whole
            word.push_back(static_cast<char>(c));
            word.push_back(text[i + 1]);
            ++i;
            if ((text[i] == 'A' || text[i] == 'T') && i + 1 < text.size()) word.push_back(text[++i]);
            continue;
        }
        const int a = adv(c);
        if (wordW + a > maxAdvance && !word.empty()) {  // over-long word: hard split
            flushWord();
            if (!line.empty()) {
                lines.push_back(line);
                line.clear();
                lineW = 0;
            }
        }
        word.push_back(static_cast<char>(c));
        wordW += a;
    }
    flushWord();
    if (!line.empty() || lines.empty()) lines.push_back(line);
    return lines;
}

}  // namespace rt::native::gameplay::text
