#pragma once

// Canonical opcode tables + deterministic record walker for the three byte-stream
// "languages" the original executables interpret.  Pure host C++: no guest memory,
// no ROM bytes, no I/O.  Every OpRow carries one grounding address.
//
//   Dialect::TownPrefix  town/shop dialogue pre-control prefix, ops 0x01..0x1D.
//                        Interpreter InterpretScriptCommands 0x0023AD00 (SLUS_203.98).
//   Dialect::PadPattern  per-frame DualShock rumble pattern bytes, interpreter inside
//                        0x0020A660 (SLUS_203.98).  0x85..0x8B are HOLD bytes here.
//   Dialect::TsqTrack    SNDMOD.IRX sequence track bytes (play 0x5294).  The EA bend,
//                        E5 absolute-pitch/retrigger, F9 fork and delta-0 stall facts
//                        live in THIS dialect, not in the other two.
//
// grounding_addr is a virtual address in the image named by groundingImage(dialect):
// SLUS_203.98 for TownPrefix/PadPattern, SNDMOD.IRX (file offset = vaddr + 0xA0) for
// TsqTrack.  width is the TOTAL record length in bytes including the opcode byte;
// width == 0 means UNKNOWN width: the walker never guesses, it stops and hands back
// the remaining bytes starting at the unknown byte.
//
// Branching town ops are walked on their not-taken ("skip") path: the static walker
// is a linear decoder, not an executor (the executor is gameplay/script_vm).

#include <cstddef>
#include <cstdint>
#include <functional>
#include <optional>
#include <span>

namespace rt::native::gameplay::script_ops {

enum class Dialect : std::uint8_t { TownPrefix, PadPattern, TsqTrack };

struct OpRow {
    std::uint8_t op;
    std::uint8_t width;          // total bytes incl. opcode; 0 = UNKNOWN (walker stops)
    const char* name;
    std::uint32_t grounding_addr;
};

constexpr std::size_t kOpCount = 256;

// 256 rows, indexed by opcode byte, for every dialect.
std::span<const OpRow, kOpCount> opTable(Dialect d);
const OpRow& lookup(Dialect d, std::uint8_t op);
const char* dialectName(Dialect d);
const char* groundingImage(Dialect d);

// Terminator rows: PadPattern 0xFF (clears the effect pointer), TsqTrack 0xFF (data_end).
bool isTerminator(Dialect d, std::uint8_t op);
// TownPrefix only: bytes >= 0x20 end the prefix and belong to the text renderer.
constexpr std::uint8_t kTownTextStart = 0x20;

// ------------------------------------------------------------------ walker
enum class WalkStatus : std::uint8_t {
    Exhausted,     // every byte consumed, no terminator seen
    Terminated,    // a terminator record (FF) was consumed; remaining = bytes after it
    TextStart,     // TownPrefix: byte >= 0x20; remaining starts at that byte
    UnknownWidth,  // width==0 op; remaining starts at that byte (NOT consumed)
    Truncated,     // known width exceeds the supplied bytes; remaining starts at record
    Stopped,       // onRecord returned false after consuming that record
};

struct Record {
    Dialect dialect;
    const OpRow* row;                    // never null
    std::uint8_t op;
    std::uint8_t width;
    std::size_t offset;                  // byte offset of the opcode within the walked span
    std::span<const std::uint8_t> bytes; // exactly `width` bytes, opcode first
};

struct WalkResult {
    WalkStatus status = WalkStatus::Exhausted;
    std::size_t records = 0;
    std::size_t consumed = 0;
    std::span<const std::uint8_t> remaining{};
};

using RecordFn = std::function<bool(const Record&)>;  // return false to stop the walk

// Deterministic: dispatch by table width only.  onRecord may be empty.
WalkResult walkRecord(Dialect d, std::span<const std::uint8_t> stream,
                      const RecordFn& onRecord = {});

// ------------------------------------------------------------------ PadPattern accessors
enum class PadKind : std::uint8_t {
    Hold, SmallOn, SmallOff, LargeValue, LargeOff, LargeInitial, End
};

struct PadRecord {
    PadKind kind = PadKind::Hold;
    // Hold: frames to wait; byte value is the frame count for EVERY hold byte
    //       (0x00..0x7F and 0x85..0xFE) because they share one default arm.
    //       Hold(0) is not a terminator: the loop continues in the same frame.
    // LargeValue: the literal motor level (0..255).
    // Others: 0.
    std::uint8_t value = 0;
    // LargeInitial (0x84) takes its strength from the trigger call (gp-0x5450+player),
    // not from an operand: true means `value` is meaningful.
    bool valueKnown = true;
};

PadKind padKind(std::uint8_t op);
bool padIsHold(std::uint8_t op);          // 0x00..0x7F and 0x85..0xFE
bool padIsHoldHighRange(std::uint8_t op); // 0x85..0xFE (includes the 0x85..0x8B band)
std::optional<PadRecord> decodePad(const Record& r);

// ------------------------------------------------------------------ TsqTrack accessors
enum class TsqKind : std::uint8_t {
    Delta,        // 00..7F
    NoteOn,       // 80..DF
    Volume,       // E0 vv
    Pan,          // E1 ll rr
    Program,      // E2 pp
    Tempo,        // E3 u16 consumed, nothing stored
    Pitch,        // E4 u16 -> +0xE (bend applied), no retrigger
    KeyPitch,     // E5 u16 absolute freq -> +0xE, retriggers
    RevMode,      // E6 b
    RevDepth,     // E7 b
    ReverbOn,     // E8
    ReverbOff,    // E9
    PitchCent,    // EA u16 -> +0xC Q12 bend multiplier
    KeyOff,       // F0
    Priority,     // F1 b
    Jump,         // F8 s16
    Fork,         // F9 s16
    End,          // FF
};

struct TsqRecord {
    TsqKind kind = TsqKind::End;
    std::uint8_t b0 = 0;       // first operand byte (or delta/key value)
    std::uint8_t b1 = 0;       // second operand byte (Pan right)
    std::uint16_t u16 = 0;     // little-endian u16 operand (E3/E4/E5/EA; raw bits for F8/F9)
    std::int16_t s16 = 0;      // signed form of u16 (F8/F9 relative offset)
};

std::optional<TsqRecord> decodeTsq(const Record& r);

// 00..7F: stored count is (u8)D - 1 truncated to u16; the next event is read when the
// s16 countdown, decremented once per tick, reaches -1.  Result = ticks after the delta
// byte until the next event: D for D in 1..0x7F, 65536 for D == 0 (the countdown must wrap
// the whole s16 range).  The "65535" quoted elsewhere counts ticks beyond the 1-tick minimum.
constexpr std::uint32_t tsqTicksUntilNextEvent(std::uint8_t delta) {
    return delta == 0 ? 65536u : delta;
}
constexpr std::uint16_t tsqStoredCountdown(std::uint8_t delta) {
    return static_cast<std::uint16_t>(static_cast<std::uint16_t>(delta) - 1u);
}

// F8 / F9: both are relative to the first byte AFTER the three-byte record.
constexpr std::int64_t tsqRelativeTarget(std::size_t recordOffset, std::int16_t rel) {
    return static_cast<std::int64_t>(recordOffset) + 3 + rel;
}

constexpr bool kTsqEaIsQ12BendMultiplier = true;   // EA -> +0xC, key_on freq = freq*bend>>12
constexpr bool kTsqE5RetriggersVoice = true;       // E5 -> +0xE absolute freq + voice retrigger

} // namespace rt::native::gameplay::script_ops
