#include "gameplay/script_ops.h"

#include <array>

namespace rt::native::gameplay::script_ops {

namespace {

using Table = std::array<OpRow, kOpCount>;

constexpr Table blank(const char* name, std::uint32_t addr) {
    Table t{};
    for (std::size_t i = 0; i < kOpCount; ++i) {
        t[i] = OpRow{static_cast<std::uint8_t>(i), 0, name, addr};
    }
    return t;
}

constexpr void set(Table& t, std::uint8_t op, std::uint8_t width, const char* name,
                   std::uint32_t addr) {
    t[op] = OpRow{op, width, name, addr};
}

constexpr void setRange(Table& t, unsigned lo, unsigned hi, std::uint8_t width,
                        const char* name, std::uint32_t addr) {
    for (unsigned op = lo; op <= hi; ++op) {
        t[op] = OpRow{static_cast<std::uint8_t>(op), width, name, addr};
    }
}

// ---- TownPrefix: SLUS_203.98, jump table 0x2EC750, handler (case) vaddrs.
// Widths are the not-taken path: operand bytes + opcode.  The `s0` register is opcode+1
// on entry (0x23AD34) and every skip arm re-enters the loop at 0x23B13C.
constexpr Table makeTown() {
    Table t = blank("unknown_precontrol", 0x0023B0E0); // report + swallow-by-rule, no fixed width
    setRange(t, 0x20, 0xFF, 0, "text_start", 0x0023B140); // sltiu <0x20 loop test
    set(t, 0x01, 3, "if_flag_set_branch", 0x0023AD50);
    set(t, 0x02, 3, "if_flag_clear_branch", 0x0023AD70);
    set(t, 0x03, 4, "if_slot_count_branch", 0x0023AD90);       // skip arm 0x23AFD4: s0 += 3
    set(t, 0x04, 2, "if_stamp_flag_branch", 0x0023ADA8);
    set(t, 0x05, 3, "if_flag_fc68_branch", 0x0023ADC8);
    set(t, 0x06, 3, "if_state_bit_branch", 0x0023ADE8);
    set(t, 0x07, 2, "if_slot_presence_branch", 0x0023AE08);
    set(t, 0x08, 3, "slot_pair_three_way", 0x0023AE20);        // Fcb8 arm 0x23AE4C: s0 += 2
    set(t, 0x0B, 3, "if_president_result_branch", 0x0023AEC0);
    set(t, 0x0D, 3, "if_flag_fca8_clear_branch", 0x0023AEE0);
    set(t, 0x0E, 7, "task_indexed_jump", 0x0023AF00);          // skip arm 0x23AF0C: s0 += 6
    set(t, 0x0F, 2, "set_flag_fc98", 0x0023AFF8);
    set(t, 0x10, 2, "clear_flag_fc98", 0x0023B010);
    set(t, 0x11, 3, "clear_flagged_slot_bit", 0x0023B028);
    set(t, 0x12, 1, "parent_done_result10", 0x0023B048);
    set(t, 0x13, 2, "record_task_result", 0x0023B060);
    set(t, 0x14, 3, "warp_town_shop", 0x0023B078);
    set(t, 0x15, 2, "set_state_bit_fc88", 0x0023B0B0);
    set(t, 0x16, 2, "if_progress_for_pair_branch", 0x0023AE58);
    set(t, 0x17, 2, "set_progress_flag_queue", 0x0023B0C8);
    set(t, 0x18, 2, "if_word14_out_of_range_branch", 0x0023AE78);
    set(t, 0x19, 3, "if_progress_flag_branch", 0x0023AEA8);
    set(t, 0x1A, 3, "if_bits_left_branch", 0x0023AF38);
    set(t, 0x1B, 2, "license_gate", 0x0023AF60);
    set(t, 0x1C, 3, "if_town_equals_branch", 0x0023AFA0);
    set(t, 0x1D, 4, "if_object_attr_branch", 0x0023AFC0);
    return t;
}

// ---- PadPattern: SLUS_203.98 per-frame interpreter inside 0x20A660.
// Dispatch compares: 0x20A710 (82), 0x20A718/0x20A724 (80), 0x20A72C (81), 0x20A740 (84),
// 0x20A748 (83), 0x20A750 (FF); everything else -> default arm 0x20A7E0 (hold literal).
constexpr Table makePad() {
    Table t = blank("hold", 0x0020A7E0);
    for (std::size_t i = 0; i < kOpCount; ++i) t[i].width = 1;
    set(t, 0x80, 1, "small_on", 0x0020A760);
    set(t, 0x81, 1, "small_off", 0x0020A770);
    set(t, 0x82, 2, "large_value_literal", 0x0020A780);
    set(t, 0x83, 1, "large_off", 0x0020A7A0);
    set(t, 0x84, 1, "large_stored_strength", 0x0020A7B0);
    set(t, 0xFF, 1, "end", 0x0020A7C8);
    return t;
}

// ---- TsqTrack: SNDMOD.IRX `play` 0x5294 dispatch (0x5368..0x5640); addresses are the
// handler entry (or the inline arm for E8/E9).  Every byte not matched falls to the
// default arm 0x5640 -> data_end 0x4940 (stops the track).
constexpr Table makeTsq() {
    Table t = blank("unknown_data_end", 0x00005640);
    setRange(t, 0x00, 0x7F, 1, "delta", 0x000047F0);           // step_t
    setRange(t, 0x80, 0xDF, 1, "note_on", 0x00004BAC);         // key_on
    set(t, 0xE0, 2, "volume", 0x00004F74);
    set(t, 0xE1, 3, "panpot", 0x00004DAC);
    set(t, 0xE2, 2, "tone_chg", 0x00004D34);
    set(t, 0xE3, 3, "tempo", 0x00005110);
    set(t, 0xE4, 3, "pitch", 0x00005184);
    set(t, 0xE5, 3, "key_pitch", 0x00004A40);
    set(t, 0xE6, 2, "set_rev_mode", 0x0000446C);
    set(t, 0xE7, 2, "set_rev_depth", 0x00004518);
    set(t, 0xE8, 1, "reverb_on", 0x0000554C);
    set(t, 0xE9, 1, "reverb_off", 0x00005570);
    set(t, 0xEA, 3, "pitch_cent", 0x000048BC);
    set(t, 0xF0, 1, "key_off", 0x00004CE4);
    set(t, 0xF1, 2, "pri_chg", 0x000049B4);
    set(t, 0xF8, 3, "data_jump", 0x0000482C);
    set(t, 0xF9, 3, "ext_chan", 0x000045D4);
    set(t, 0xFF, 1, "data_end", 0x00004940);
    return t;
}

constexpr Table kTown = makeTown();
constexpr Table kPad = makePad();
constexpr Table kTsq = makeTsq();

const Table& tableFor(Dialect d) {
    switch (d) {
        case Dialect::TownPrefix: return kTown;
        case Dialect::PadPattern: return kPad;
        case Dialect::TsqTrack: return kTsq;
    }
    return kTown;
}

std::int16_t s16le(std::uint8_t lo, std::uint8_t hi) {
    return static_cast<std::int16_t>(static_cast<std::uint16_t>(lo | (hi << 8)));
}

} // namespace

std::span<const OpRow, kOpCount> opTable(Dialect d) {
    return std::span<const OpRow, kOpCount>(tableFor(d).data(), kOpCount);
}

const OpRow& lookup(Dialect d, std::uint8_t op) { return tableFor(d)[op]; }

const char* dialectName(Dialect d) {
    switch (d) {
        case Dialect::TownPrefix: return "TownPrefix";
        case Dialect::PadPattern: return "PadPattern";
        case Dialect::TsqTrack: return "TsqTrack";
    }
    return "?";
}

const char* groundingImage(Dialect d) {
    return d == Dialect::TsqTrack ? "SNDMOD.IRX" : "SLUS_203.98";
}

bool isTerminator(Dialect d, std::uint8_t op) {
    return op == 0xFF && (d == Dialect::PadPattern || d == Dialect::TsqTrack);
}

WalkResult walkRecord(Dialect d, std::span<const std::uint8_t> stream, const RecordFn& onRecord) {
    WalkResult out;
    const Table& table = tableFor(d);
    std::size_t pos = 0;
    auto finish = [&](WalkStatus s) {
        out.status = s;
        out.consumed = pos;
        out.remaining = stream.subspan(pos);
        return out;
    };
    while (pos < stream.size()) {
        const std::uint8_t op = stream[pos];
        const OpRow& row = table[op];
        if (d == Dialect::TownPrefix && op >= kTownTextStart) return finish(WalkStatus::TextStart);
        if (row.width == 0) return finish(WalkStatus::UnknownWidth);
        if (stream.size() - pos < row.width) return finish(WalkStatus::Truncated);
        const Record rec{d, &row, op, row.width, pos, stream.subspan(pos, row.width)};
        pos += row.width;
        ++out.records;
        const bool keepGoing = !onRecord || onRecord(rec);
        if (isTerminator(d, op)) return finish(WalkStatus::Terminated);
        if (!keepGoing) return finish(WalkStatus::Stopped);
    }
    return finish(WalkStatus::Exhausted);
}

// ------------------------------------------------------------------ PadPattern
PadKind padKind(std::uint8_t op) {
    switch (op) {
        case 0x80: return PadKind::SmallOn;
        case 0x81: return PadKind::SmallOff;
        case 0x82: return PadKind::LargeValue;
        case 0x83: return PadKind::LargeOff;
        case 0x84: return PadKind::LargeInitial;
        case 0xFF: return PadKind::End;
        default: return PadKind::Hold;
    }
}

bool padIsHold(std::uint8_t op) { return padKind(op) == PadKind::Hold; }

bool padIsHoldHighRange(std::uint8_t op) { return op >= 0x85 && op <= 0xFE; }

std::optional<PadRecord> decodePad(const Record& r) {
    if (r.dialect != Dialect::PadPattern || r.bytes.empty()) return std::nullopt;
    PadRecord p;
    p.kind = padKind(r.op);
    switch (p.kind) {
        case PadKind::Hold: p.value = r.op; break;
        case PadKind::LargeValue:
            if (r.bytes.size() < 2) return std::nullopt;
            p.value = r.bytes[1];
            break;
        case PadKind::LargeInitial: p.value = 0; p.valueKnown = false; break;
        default: p.value = 0; break;
    }
    return p;
}

// ------------------------------------------------------------------ TsqTrack
std::optional<TsqRecord> decodeTsq(const Record& r) {
    if (r.dialect != Dialect::TsqTrack || r.bytes.size() != r.width || r.width == 0) {
        return std::nullopt;
    }
    TsqRecord t;
    const auto& b = r.bytes;
    if (r.op <= 0x7F) {
        t.kind = TsqKind::Delta;
        t.b0 = r.op;
        return t;
    }
    if (r.op <= 0xDF) {
        t.kind = TsqKind::NoteOn;
        t.b0 = static_cast<std::uint8_t>(r.op & 0x7F);
        return t;
    }
    auto u16 = [&]() {
        t.b0 = b[1];
        t.b1 = b[2];
        t.u16 = static_cast<std::uint16_t>(b[1] | (b[2] << 8));
        t.s16 = s16le(b[1], b[2]);
    };
    switch (r.op) {
        case 0xE0: t.kind = TsqKind::Volume; t.b0 = b[1]; break;
        case 0xE1: t.kind = TsqKind::Pan; t.b0 = b[1]; t.b1 = b[2]; break;
        case 0xE2: t.kind = TsqKind::Program; t.b0 = b[1]; break;
        case 0xE3: t.kind = TsqKind::Tempo; u16(); break;
        case 0xE4: t.kind = TsqKind::Pitch; u16(); break;
        case 0xE5: t.kind = TsqKind::KeyPitch; u16(); break;
        case 0xE6: t.kind = TsqKind::RevMode; t.b0 = b[1]; break;
        case 0xE7: t.kind = TsqKind::RevDepth; t.b0 = b[1]; break;
        case 0xE8: t.kind = TsqKind::ReverbOn; break;
        case 0xE9: t.kind = TsqKind::ReverbOff; break;
        case 0xEA: t.kind = TsqKind::PitchCent; u16(); break;
        case 0xF0: t.kind = TsqKind::KeyOff; break;
        case 0xF1: t.kind = TsqKind::Priority; t.b0 = b[1]; break;
        case 0xF8: t.kind = TsqKind::Jump; u16(); break;
        case 0xF9: t.kind = TsqKind::Fork; u16(); break;
        case 0xFF: t.kind = TsqKind::End; break;
        default: return std::nullopt;
    }
    return t;
}

} // namespace rt::native::gameplay::script_ops
