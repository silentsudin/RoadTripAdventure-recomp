// Dialogue / text engine of the original game, ported from the ELF
// (docs/native-reference/text-script.md; batch W2-07).  Pure C++: no SDL, no GPU,
// no guest memory.  Draw output is a list of GS sprite rectangles in the 640x448
// UI space, byte-for-byte what the original hands to uiSpriteWrite 0x2063C8.
//
// Ported functions (every one is also executed from the ELF by a MIPS
// interpreter to generate the test fixtures, see the batch report):
//   uiDrawGlyph      0x2017B0   one character -> sprite rect + pen advance
//   uiTextWidth      0x203F40   string advance sum
//   uiDrawTextShade  0x204230   string draw (used by the `$` escape codes)
//   uiDrawNumberAuto 0x204460   right-aligned number, width in 10-px cells
//   RenderEscapeText 0x23A8E8   one 3-line page: `$X` codes, typewriter limit,
//                               scroll window, command-byte detection
//   dialogueTask     0x23B168   message-window state machine (states 0..6)
//   uiDrawDialogueFrameA/B 0x2028B0/0x202B70, uiDrawIcon20 0x203E70
//   CopyDialogueTextBounded 0x24E7D8   ASCII -> full-width Shift-JIS field copy
//   SubstituteNameTokens    0x264CB0   first `$N` -> player name (dead code in
//                                      the original: 0 static call sites)
//   ExtractDelimitedToken   0x24EA00
//
// Findings that correct earlier notes (all re-derived from the ELF): the
// original has NO automatic word wrap - lines are broken by '\n' in the data;
// a page is exactly 3 lines of 24 px; the glyph advance is per-character
// (proportional table at 0x29A880), not 16 px; CopyDialogueTextBounded widens
// ASCII to full-width Shift-JIS, it is not a pass-through copy.
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace rt::native::gameplay::text {

// ---------------------------------------------------------------------------
// 1. Glyph metrics (uiGlyphWidths @ 0x29A860 + c; table entries 0x20..0x7F)
// ---------------------------------------------------------------------------
inline constexpr int kGlyphCellW = 16;  // FONTE atlas cell (glyph_atlas.h)
inline constexpr int kGlyphCellH = 24;
inline constexpr int kSpaceAdvance = 8;             // uiDrawGlyph: c == 0x20
inline constexpr int kWideAdvance = 20;             // Shift-JIS glyph (0x14)
inline constexpr int kIdeographicSpaceAdvance = 10; // SJIS 0x8140
inline constexpr int kWideDotAdvance = 16;          // SJIS 0x818B (sprite u=0xC0 v=0x60)
inline constexpr unsigned kAdvanceFirst = 0x20;
inline constexpr unsigned kAdvanceLast = 0x7F;
inline constexpr std::size_t kAdvanceCount = kAdvanceLast - kAdvanceFirst + 1;  // 96

extern const std::array<std::int8_t, kAdvanceCount> kAdvanceTable;

// Table advance of `c`; 0 outside 0x20..0x7F (the ELF bytes there are not a
// glyph table; accent-follower bytes >= 0x80 are not modelled).
int tableAdvance(unsigned c);

struct Sprite {
    std::int16_t x = 0, y = 0, u = 0, v = 0, w = 0, h = 0;
    bool operator==(const Sprite&) const = default;
};

struct GlyphDraw {
    bool hasSprite = false;
    Sprite sprite;
    int nextX = 0;
    bool kanjiUvUnmodelled = false;  // SJIS other than 0x8140/0x818B: u,v come from uiKana/KanjiGlyphUV tables (GAP)
};

// uiDrawGlyph: `c0` is the glyph byte, `c1` the following byte (accent
// followers '/e', '`x', '^x', '~x' and SJIS trail bytes read it).  `y2` is the
// already doubled y (callers pass y<<1); `shade` shrinks the sprite height.
GlyphDraw drawGlyph(int x, int y2, int shade, std::uint8_t c0, std::uint8_t c1);

// Bytes the string drawers (uiDrawText/uiDrawTextShade/uiTextWidth) consume
// for the character starting with `c`: 2 for '/', '`', '^', '~' and any byte
// >= 0x80, else 1.  (RenderEscapeText steps 1 for the four ASCII prefixes.)
inline constexpr int stringStep(std::uint8_t c) {
    return (c == 0x2f || c == 0x60 || c == 0x5e || c == 0x7e || (c & 0x80)) ? 2 : 1;
}

// uiTextWidth: NUL-terminated (or span-end-terminated) advance sum.
int textWidth(std::span<const std::uint8_t> s);
int textWidth(std::string_view s);

// uiDrawNumberAuto cell count: digits plus thousands separators, 10 px each.
int numberAutoCells(std::int64_t v);

// ---------------------------------------------------------------------------
// 2. Draw events
// ---------------------------------------------------------------------------
struct DrawEvent {
    enum class Kind : std::uint8_t { Sprite, Number, Texa };
    Kind kind = Kind::Sprite;
    // Sprite: a..f = x,y,u,v,w,h.  Number: a..d = x,y,value,base.  Texa: a = value.
    std::int64_t a = 0, b = 0, c = 0, d = 0, e = 0, f = 0;
    bool operator==(const DrawEvent&) const = default;
};
DrawEvent spriteEvent(const Sprite& s);

// ---------------------------------------------------------------------------
// 3. `$X` escape codes and RenderEscapeText
// ---------------------------------------------------------------------------
// Data the codes read.  Addresses are the original's.
class IEscapeHost {
public:
    virtual ~IEscapeHost() = default;
    virtual std::int32_t amountWord(std::uint8_t id) const = 0;   // $A<id>: word at 0x177FCFC + 4*id, shown /1000
    virtual std::string_view stringG() const = 0;                 // $G: string at 0x177FDA6 (progress+0x646)
    virtual std::string_view playerName() const = 0;              // $N: string at 0x177FD8C (progress+0x62C)
    virtual std::string_view courseName() const = 0;              // $R: gs_game_course_names[*(byte*)(ctx+0x10)->...]
    virtual std::int32_t bankBalance() const = 0;                 // $S: word at 0x1782A6C, drawn by uiDrawNumberAuto
    virtual std::string_view stringU() const = 0;                 // $U: string at 0x177FD9F (progress+0x63F)
    virtual std::int32_t clockWord(std::uint8_t id) const = 0;    // $T<id>: id '7' -> word 0x1780A10, else 0x1787008 + 4*id
};

// Task fields RenderEscapeText reads/writes.
struct TextState {
    std::uint8_t scroll = 0;   // task +0x26: pixels*? scrolled; lines skipped = scroll / 12
    std::int16_t limit = 0;    // task +0x02: typewriter limit in elements; 0x7FFF = unlimited
};
inline constexpr std::int16_t kLimitUnlimited = 0x7FFF;
inline constexpr int kLineUnits = 12;            // scroll units per line (24 px = 12 * 2)
inline constexpr int kLinesPerPage = 3;
inline constexpr int kTextOriginX = 0x46;        // 70
inline constexpr int kTextOriginUnitsY = 0xAD;   // 173 (y2 = units * 2 = 346)
inline constexpr int kLinePitchPx = 24;

enum class PageStatus : std::uint8_t {
    LimitReached,  // typewriter pause; cursor at the next printable element
    ControlByte,   // text ended on a non-newline control byte (the command byte); cursor at it
    PageComplete,  // three newline-terminated lines drawn (original returns NULL)
};

struct PageResult {
    PageStatus status = PageStatus::ControlByte;
    std::size_t cursor = 0;        // meaningful for LimitReached / ControlByte
    int charCount = 0;             // lVar6 / s4 at exit
    std::int16_t newLimit = 0;     // task +2 after the call
    bool readPastEnd = false;      // buffer ended before a terminator (reads returned 0)
    bool runaway = false;          // line-skip ran past the buffer (the original loops forever here)
    std::vector<DrawEvent> events;
};

// `buf` is the text; `cursor` the start index.  Reads at or beyond buf.size()
// return 0 (a control byte), which terminates the page.
PageResult renderEscapeText(std::span<const std::uint8_t> buf, std::size_t cursor, TextState st,
                            const IEscapeHost& host);

// ---------------------------------------------------------------------------
// 4. Window frame (uiDrawDialogueFrameA/B, uiDrawIcon20)
// ---------------------------------------------------------------------------
inline constexpr int kFrameY = 0x158;       // 344, the argument dialogueTask passes
inline constexpr int kWindowX = 0x20;       // 32
inline constexpr int kWindowTop = kFrameY - 0x10;     // 328
inline constexpr int kWindowW = 576;
inline constexpr int kWindowH = 104;                  // 328..432
inline constexpr int kTextRightEdge = 0x238;          // 568: right corner sprite x
inline constexpr int kTextMaxAdvance = kTextRightEdge - kTextOriginX;  // 498
inline constexpr int kPlateRightX = 0x23E;            // 574: name plate right alignment
inline constexpr int kPlateTextY = 0xA0;              // uiDrawText y arg (y2 = 320)
inline constexpr int kArrowIconX = 0x136, kArrowIconY = 0x1A4, kArrowIconIndex = 7;

std::vector<Sprite> frameASprites(int frameY = kFrameY);                       // 7 sprites
std::vector<Sprite> frameBSprites(int plateWidth, int frameY = kFrameY);       // 3 sprites
Sprite icon20Sprite(int x, int y, int index);

// uiDrawText(x, y, str) glyph sprites (y is the half-y argument; y2 = 2*y).
std::vector<DrawEvent> drawTextEvents(int x, int yArg, std::span<const std::uint8_t> s, int shade = 0);

// ---------------------------------------------------------------------------
// 5. Message window (dialogueTask 0x23B168)
// ---------------------------------------------------------------------------
class ITextMemory {
public:
    virtual ~ITextMemory() = default;
    virtual bool readU8(std::uint32_t vaddr, std::uint8_t& out) const = 0;
    virtual bool readU32(std::uint32_t vaddr, std::uint32_t& out) const = 0;
};

enum class WinState : std::uint8_t {
    Open = 0,         // fade-in, 16 ticks
    Text = 1,         // prefix ops, page render, command dispatch
    Scroll = 2,       // +2 scroll units per tick until a multiple of 12
    Close = 3,        // fade-out, 16 ticks, then result to the parent and kill
    ChildMenu = 4,    // child task running; window keeps drawing
    ChildResult = 5,  // child result is a branch index (or -1 = continue at +0x27)
    ChildNext = 6,    // waiting for a child; draws nothing meanwhile
};

enum class CmdKind : std::uint8_t {
    Spawn,        // arrow-gate optional, spawn child `fn` with args pointer, ->ChildMenu
    SpawnFixed,   // spawn 0x239900 with a constant (cmd 4: 9, cmd 6: 8), ->ChildResult
    CloseWait,    // 0, 0xF, 0x18, 3: wait for confirm then fade out
    Goto,         // 0xC, 0x13(spawn+goto), 0xE, 0x11, 0x15: set next message
    Stamps,       // 0xD
    Warp,         // 0x14
};
struct CommandInfo {
    std::uint8_t cmd;
    const char* name;
    CmdKind kind;
    std::uint32_t childFn;   // 0 when none
    bool arrowGate;          // draws the blinking arrow and waits for confirm first
    std::uint8_t argBytes;   // fixed operand bytes after the command byte (0xD: NUL-terminated list)
};
inline constexpr std::size_t kCommandCount = 25;  // commands 0x00..0x18
extern const std::array<CommandInfo, kCommandCount> kCommands;
const CommandInfo* commandInfo(std::uint8_t cmd);  // nullptr for >= 0x19 and 0x0A (typewriter default)

struct Effect {
    enum class Kind : std::uint8_t {
        Sound,         // a = sound id
        SpawnChild,    // a = child fn vaddr, b = argument (vaddr or constant)
        ParentDone,    // a = result value written to parent+4 (parent+0x24 = 1)
        CtxFlag400,    // a = 1 set, 0 clear (ctx+0x28 bit 0x400)
        TalkAnim,      // ctx+0x18 = a
        CtxByte20,     // ctx+0x20 = a
        AwardStamp,    // a = stamp id
        ReleaseStamp,  // ReleaseStampFlag(ctx)
        WarpTo,        // ctx+0x23 = a (town), ctx+0x21 = b (shop)
        SetObjectAttr, // a, b = args (object 0x177F760)
        TakePhoto,     // a = photo id
        TownVar,       // DAT_01780A3C = a
        NextMsg,       // printf("next msg #%d"), a = new cursor
        Kill,          // gs_tasks_kill(self)
    };
    Kind kind;
    std::uint32_t a = 0, b = 0;
    bool operator==(const Effect&) const = default;
};

struct TickOutput {
    std::vector<DrawEvent> events;
    std::vector<Effect> effects;
};

class MessageWindow {
public:
    // `table` = shop level-2 table (task +0x1c): [0] name string, [1] entry
    // script, [2..] branch targets.  `runPrefix` = InterpretScriptCommands
    // bound to the host (called every tick on the cursor like the original).
    MessageWindow(const ITextMemory& mem, const IEscapeHost& host, std::uint32_t table,
                  std::function<std::uint32_t(std::uint32_t cursor)> runPrefix);

    // One task callback; `confirm` = (ctx+0x30 & 0x50) != 0.
    TickOutput tick(bool confirm);
    // A child task finished: parent+0x24 pulse + parent+4 result.  The pulse is
    // visible to the next tick() only (the task walker clears it after each call).
    void signalChildDone(std::int32_t result);

    WinState state() const { return state_; }
    std::uint8_t scroll() const { return scroll_; }
    std::int16_t limit() const { return limit_; }
    std::uint32_t cursor() const { return cursor_; }
    std::uint8_t pending() const { return pend_; }
    std::uint32_t tickCount() const { return tick_; }
    bool killed() const { return killed_; }

private:
    void render(TickOutput& out, bool confirm);
    void drawFrame(TickOutput& out, int plateWidth);
    std::string readName() const;
    std::uint32_t branch(std::uint32_t idx) const;
    std::uint8_t byteAt(std::uint32_t v) const;
    void nextMsg(TickOutput& out, std::uint32_t target);

    const ITextMemory& mem_;
    const IEscapeHost& host_;
    std::uint32_t table_;
    std::function<std::uint32_t(std::uint32_t)> runPrefix_;
    std::uint32_t tick_ = 0;              // task +8
    std::int16_t limit_ = 0;              // task +2
    std::int32_t result_ = 0;             // task +4
    bool childDone_ = false;              // task +0x24
    WinState state_ = WinState::Open;     // task +0x25
    std::uint8_t scroll_ = 0;             // task +0x26
    std::uint8_t pend_ = 0;               // task +0x27
    std::uint32_t cursor_ = 0;            // task +0x28
    bool killed_ = false;
};

// ---------------------------------------------------------------------------
// 6. String utilities
// ---------------------------------------------------------------------------
// CopyDialogueTextBounded 0x24E7D8.  Widens ASCII to full-width Shift-JIS
// (digits 0x824F.., A-Z 0x8260.., a-z 0x8281.., punctuation per table, any
// other byte -> 0x8140), copies 0x80+ lead/trail pairs verbatim.  `limit` counts
// CHARACTERS (a pair counts 1); 0 = until the source ends.  The source is
// treated as followed by zero bytes, so a short source with limit > length is
// padded with 0x8140 (the original reads on past the NUL).  `dstCapacity`
// (bytes, including the terminator) is never exceeded and a pair is never
// split; returns the number of characters written.
std::string copyDialogueTextBounded(std::span<const std::uint8_t> src, int limit,
                                    std::size_t dstCapacity, int* charsWritten = nullptr);
// Full-width code of an ASCII byte as the original's table yields it.
std::uint16_t fullwidthSjis(std::uint8_t c);

// SubstituteNameTokens 0x264CB0: replaces the FIRST "$N" with `name`.  Returns
// false when there is no token (original returns NULL) or the result would not
// fit `capacity` bytes including the NUL (the original has no bound).
bool substituteNameTokens(std::string& text, std::string_view name, std::size_t capacity);

// ExtractDelimitedToken 0x24EA00: skips (k+1) delimiter-separated groups (k < 0
// skips none), then copies up to the next delimiter.  false = original NULL.
bool extractDelimitedToken(std::string_view s, std::string_view delims, int k, std::string& out);

// Native-authored text only (the original never wraps): greedy word wrap on
// glyph advances.  Lines break at spaces; a word longer than `maxAdvance` is
// hard-split between characters.  Escape codes are measured as zero width.
std::vector<std::string> wrapGreedy(std::string_view text, int maxAdvance = kTextMaxAdvance);

}  // namespace rt::native::gameplay::text
