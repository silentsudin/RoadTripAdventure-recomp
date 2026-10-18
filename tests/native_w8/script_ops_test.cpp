// W4-08 script_ops contract test.
//
// Part 1 (always runs, no ROM): hand-authored byte streams.  Source addresses are cited at
// each stream:  SLUS_203.98 vaddrs for TownPrefix/PadPattern, SNDMOD.IRX vaddrs and
// SOUND/*.TSQ file offsets (as quoted in docs/native-reference/tsq-tvb-format.md) for TsqTrack.
// The 64-byte rumble-pattern fixture below is calibration data cited by vaddr 0x2EA3D0..0x2EA40F.
//
// Part 2 (ELF/IRX present only; otherwise prints SKIP and still exits 0): the committed
// fixtures and table addresses are checked against the read-only images.  No image bytes are
// written anywhere.
//
// Env: RT_ELF (SLUS_203.98), RT_IRX (SNDMOD.IRX); defaults build/rom/SLUS_203.98 and
// ~/Library/Application Support/RoadTripRecomp/disc/SNDMOD.IRX.

#include "gameplay/script_ops.h"

#include <array>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>

namespace so = rt::native::gameplay::script_ops;
using so::Dialect;
using so::WalkStatus;
using Bytes = std::vector<std::uint8_t>;

namespace {
int g_fail = 0;
int g_checks = 0;
#define CHECK(c)                                                                          \
    do {                                                                                  \
        ++g_checks;                                                                       \
        if (!(c)) {                                                                       \
            ++g_fail;                                                                     \
            std::printf("FAIL %s:%d  %s\n", __FILE__, __LINE__, #c);                      \
        }                                                                                 \
    } while (0)

struct Seen {
    std::vector<std::uint8_t> ops;
    std::vector<std::size_t> offsets;
    std::vector<std::uint8_t> widths;
};

so::WalkResult walkAll(Dialect d, const Bytes& b, Seen& seen) {
    return so::walkRecord(d, b, [&](const so::Record& r) {
        seen.ops.push_back(r.op);
        seen.offsets.push_back(r.offset);
        seen.widths.push_back(r.width);
        return true;
    });
}

// ---- 8 rumble pattern records, SLUS_203.98 0x2EA3D0 + 8n (pointer table 0x29AD60).
constexpr std::array<std::uint8_t, 64> kRumblePatterns = {
    0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // 0x2EA3D0 pattern 0
    0x80, 0x84, 0x02, 0x81, 0x06, 0xff, 0x00, 0x00,  // 0x2EA3D8 pattern 1
    0x80, 0x84, 0x04, 0xff, 0x00, 0x00, 0x00, 0x00,  // 0x2EA3E0 pattern 2
    0x80, 0x01, 0x81, 0x82, 0xff, 0x02, 0xff, 0x00,  // 0x2EA3E8 pattern 3
    0x82, 0xff, 0x01, 0x80, 0x01, 0xff, 0x00, 0x00,  // 0x2EA3F0 pattern 4
    0x80, 0x82, 0xff, 0x02, 0xff, 0x00, 0x00, 0x00,  // 0x2EA3F8 pattern 5
    0x84, 0x04, 0x80, 0x02, 0x81, 0x02, 0xff, 0x00,  // 0x2EA400 pattern 6
    0x82, 0x7f, 0x1e, 0xff, 0x00, 0x00, 0x00, 0x00,  // 0x2EA408 pattern 7
};
constexpr std::array<std::size_t, 8> kPatternRecords = {1, 6, 4, 6, 5, 4, 7, 3};

// ------------------------------------------------------------------ table integrity
void testTables() {
    for (Dialect d : {Dialect::TownPrefix, Dialect::PadPattern, Dialect::TsqTrack}) {
        auto t = so::opTable(d);
        CHECK(t.size() == 256);
        bool shape = true;
        for (std::size_t i = 0; i < 256; ++i) {
            shape = shape && t[i].op == i && t[i].name != nullptr && t[i].grounding_addr != 0;
        }
        CHECK(shape);
    }

    // TownPrefix widths restated independently from the skip arms (script-vm.md section 3).
    const std::array<std::pair<int, int>, 26> townWidths = {{
        {0x01, 3}, {0x02, 3}, {0x03, 4}, {0x04, 2}, {0x05, 3}, {0x06, 3}, {0x07, 2},
        {0x08, 3}, {0x0B, 3}, {0x0D, 3}, {0x0E, 7}, {0x0F, 2}, {0x10, 2}, {0x11, 3},
        {0x12, 1}, {0x13, 2}, {0x14, 3}, {0x15, 2}, {0x16, 2}, {0x17, 2}, {0x18, 2},
        {0x19, 3}, {0x1A, 3}, {0x1B, 2}, {0x1C, 3}, {0x1D, 4}}};
    int known = 0;
    for (std::size_t op = 0; op < 256; ++op) {
        if (so::lookup(Dialect::TownPrefix, static_cast<std::uint8_t>(op)).width) ++known;
    }
    CHECK(known == 26);
    for (auto [op, w] : townWidths) {
        CHECK(so::lookup(Dialect::TownPrefix, static_cast<std::uint8_t>(op)).width == w);
    }
    // gsl-vm.md 5.1 draft widths were wrong: op 03 = 4 (three operands), op 08 = 3 (two idx).
    CHECK(so::lookup(Dialect::TownPrefix, 0x03).width == 4);
    CHECK(so::lookup(Dialect::TownPrefix, 0x08).width == 3);
    for (int op : {0x00, 0x09, 0x0A, 0x0C, 0x1E, 0x1F}) {
        const auto& r = so::lookup(Dialect::TownPrefix, static_cast<std::uint8_t>(op));
        CHECK(r.width == 0 && r.grounding_addr == 0x0023B0E0);
    }

    // PadPattern: only 0x82 is wider than one byte; all 256 byte values have a width.
    for (std::size_t op = 0; op < 256; ++op) {
        const auto w = so::lookup(Dialect::PadPattern, static_cast<std::uint8_t>(op)).width;
        CHECK(w == (op == 0x82 ? 2 : 1));
    }

    // TsqTrack: 240 known, 16 unknown (EB..EF, F2..F7, FA..FE).
    int tsqKnown = 0, tsqUnknown = 0;
    for (std::size_t op = 0; op < 256; ++op) {
        (so::lookup(Dialect::TsqTrack, static_cast<std::uint8_t>(op)).width ? tsqKnown
                                                                             : tsqUnknown)++;
    }
    CHECK(tsqKnown == 240 && tsqUnknown == 16);
    for (int op : {0xEB, 0xEC, 0xED, 0xEE, 0xEF, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xFA, 0xFB,
                   0xFC, 0xFD, 0xFE}) {
        const auto& r = so::lookup(Dialect::TsqTrack, static_cast<std::uint8_t>(op));
        CHECK(r.width == 0 && r.grounding_addr == 0x5640);
    }
    CHECK(so::isTerminator(Dialect::TsqTrack, 0xFF));
    CHECK(so::isTerminator(Dialect::PadPattern, 0xFF));
    CHECK(!so::isTerminator(Dialect::TownPrefix, 0xFF));
    CHECK(!so::isTerminator(Dialect::TsqTrack, 0xF3));
}

// ------------------------------------------------------------------ PadPattern (0x20A660)
void testPad() {
    // Real patterns: 36 records in total (event-bridge.md), per-pattern counts, all end on FF.
    std::size_t total = 0;
    for (std::size_t p = 0; p < 8; ++p) {
        Bytes b(kRumblePatterns.begin() + 8 * p, kRumblePatterns.begin() + 8 * p + 8);
        Seen s;
        auto r = walkAll(Dialect::PadPattern, b, s);
        CHECK(r.status == WalkStatus::Terminated);
        CHECK(r.records == kPatternRecords[p]);
        total += r.records;
        CHECK(!s.ops.empty() && s.ops.back() == 0xFF);
    }
    CHECK(total == 36);

    // Pattern 1 (0x2EA3D8): SmallOn, LargeInitial(runtime strength), Hold 2, SmallOff, Hold 6, End.
    {
        Bytes b(kRumblePatterns.begin() + 8, kRumblePatterns.begin() + 16);
        std::vector<so::PadRecord> recs;
        auto r = so::walkRecord(Dialect::PadPattern, b, [&](const so::Record& rec) {
            recs.push_back(*so::decodePad(rec));
            return true;
        });
        CHECK(recs.size() == 6);
        using K = so::PadKind;
        CHECK(recs[0].kind == K::SmallOn && recs[1].kind == K::LargeInitial);
        CHECK(!recs[1].valueKnown);
        CHECK(recs[2].kind == K::Hold && recs[2].value == 2);
        CHECK(recs[3].kind == K::SmallOff);
        CHECK(recs[4].kind == K::Hold && recs[4].value == 6);
        CHECK(recs[5].kind == K::End);
        CHECK(r.consumed == 6 && r.remaining.size() == 2);  // two zero padding bytes untouched
    }
    // Pattern 7 (0x2EA408): LargeValue(127), Hold(30), End.
    {
        Bytes b(kRumblePatterns.begin() + 56, kRumblePatterns.begin() + 64);
        std::vector<so::PadRecord> recs;
        so::walkRecord(Dialect::PadPattern, b, [&](const so::Record& rec) {
            recs.push_back(*so::decodePad(rec));
            return true;
        });
        CHECK(recs.size() == 3);
        CHECK(recs[0].kind == so::PadKind::LargeValue && recs[0].value == 0x7F);
        CHECK(recs[1].kind == so::PadKind::Hold && recs[1].value == 30);
        CHECK(recs[2].kind == so::PadKind::End);
    }

    // 0x85..0x8B (and everything 0x85..0xFE): ONE-byte HOLD records, frames = byte value.
    // Default arm 0x20A7E0 stores the opcode byte into the hold counter; dispatch at
    // 0x20A740/0x20A748/0x20A750 only special-cases 83/84/FF.  Hand-authored stream.
    {
        Bytes b = {0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0xFE, 0x00, 0x7F};
        std::vector<so::PadRecord> recs;
        Seen s;
        auto r = so::walkRecord(Dialect::PadPattern, b, [&](const so::Record& rec) {
            s.widths.push_back(rec.width);
            recs.push_back(*so::decodePad(rec));
            return true;
        });
        CHECK(r.status == WalkStatus::Exhausted && r.records == b.size());
        for (std::size_t i = 0; i < b.size(); ++i) {
            CHECK(s.widths[i] == 1);
            CHECK(recs[i].kind == so::PadKind::Hold && recs[i].value == b[i]);
            CHECK(so::padIsHold(b[i]));
        }
        CHECK(so::padIsHoldHighRange(0x85) && so::padIsHoldHighRange(0x8B));
        CHECK(!so::padIsHoldHighRange(0x84) && !so::padIsHoldHighRange(0xFF));
        // Hold(0) (0x00) is not a terminator; only FF is.
        CHECK(!so::isTerminator(Dialect::PadPattern, 0x00));
        // 0x85..0x8B are NOT radio/cutscene triggers: the table names them "hold".
        for (int op = 0x85; op <= 0x8B; ++op) {
            CHECK(std::strcmp(so::lookup(Dialect::PadPattern, static_cast<std::uint8_t>(op)).name,
                              "hold") == 0);
            CHECK(so::lookup(Dialect::PadPattern, static_cast<std::uint8_t>(op)).grounding_addr ==
                  0x0020A7E0);
        }
    }
    // Truncated 82 (needs a literal byte): walker does not read past the buffer.
    {
        Bytes b = {0x80, 0x82};
        Seen s;
        auto r = walkAll(Dialect::PadPattern, b, s);
        CHECK(r.status == WalkStatus::Truncated);
        CHECK(r.records == 1 && r.consumed == 1 && r.remaining.size() == 1 && r.remaining[0] == 0x82);
    }
    // 83 = LargeOff, 84 = LargeInitial are one byte; 82 swallows its literal even when it
    // looks like a terminator (82 FF is LargeValue(255), not End).
    {
        Bytes b = {0x82, 0xFF, 0x83, 0x84, 0xFF, 0x99};
        Seen s;
        auto r = walkAll(Dialect::PadPattern, b, s);
        CHECK(r.status == WalkStatus::Terminated && r.records == 4);
        CHECK(s.ops == (std::vector<std::uint8_t>{0x82, 0x83, 0x84, 0xFF}));
        CHECK(r.remaining.size() == 1 && r.remaining[0] == 0x99);
    }
    // Callback stop.
    {
        Bytes b = {0x80, 0x03, 0x81, 0xFF};
        auto r = so::walkRecord(Dialect::PadPattern, b,
                                [](const so::Record& rec) { return rec.op != 0x03; });
        CHECK(r.status == WalkStatus::Stopped && r.records == 2 && r.consumed == 2);
    }
}

// ------------------------------------------------------------------ TownPrefix (0x23AD00)
void testTown() {
    // Hand-authored.  Case handlers 0x23AF00.. (0E: skip arm 0x23AF0C adds 6), 0x23AD90 (03:
    // skip arm 0x23AFD4 adds 3), 0x23AE20 (08), 0x23AD50 (01), 0x23AFF8 (0F).  Text byte 0x41 ends it.
    Bytes b = {0x0F, 0x01,                                  // set flag 1          (width 2)
               0x01, 0x02, 0x03,                            // if flag 2 -> idx 3   (width 3)
               0x03, 0x01, 0x02, 0x04,                      // slot,bit,idx         (width 4)
               0x08, 0x05, 0x06,                            // idxA, idxB           (width 3)
               0x0E, 1, 2, 3, 4, 5, 6,                      // six task idx         (width 7)
               0x12,                                        // done                 (width 1)
               0x41, 0x42};                                 // text
    Seen s;
    auto r = walkAll(Dialect::TownPrefix, b, s);
    CHECK(r.status == WalkStatus::TextStart);
    CHECK(r.records == 6 && r.consumed == 20);
    CHECK(s.ops == (std::vector<std::uint8_t>{0x0F, 0x01, 0x03, 0x08, 0x0E, 0x12}));
    CHECK(s.widths == (std::vector<std::uint8_t>{2, 3, 4, 3, 7, 1}));
    CHECK(s.offsets == (std::vector<std::size_t>{0, 2, 5, 9, 12, 19}));
    CHECK(r.remaining.size() == 2 && r.remaining[0] == 0x41);

    // Unknown ops stop; they are NOT skipped (the original's swallow rule is data dependent).
    for (int op : {0x00, 0x09, 0x0A, 0x0C, 0x1E, 0x1F}) {
        Bytes u = {0x0F, 0x01, static_cast<std::uint8_t>(op), 0x0F, 0x02};
        Seen su;
        auto ru = walkAll(Dialect::TownPrefix, u, su);
        CHECK(ru.status == WalkStatus::UnknownWidth);
        CHECK(ru.records == 1 && ru.consumed == 2);
        CHECK(ru.remaining.size() == 3 && ru.remaining[0] == op);
    }
    // Truncation never over-reads: op 03 needs 4 bytes.
    {
        Bytes t = {0x03, 0x01, 0x02};
        Seen st;
        auto rt = walkAll(Dialect::TownPrefix, t, st);
        CHECK(rt.status == WalkStatus::Truncated && rt.records == 0 && rt.remaining.size() == 3);
    }
    // 0x20 itself is text (sltiu < 0x20 test at 0x23B140).
    {
        Bytes t = {0x20};
        Seen st;
        CHECK(walkAll(Dialect::TownPrefix, t, st).status == WalkStatus::TextStart);
    }
}

// ------------------------------------------------------------------ TsqTrack (SNDMOD play 0x5294)
void testTsq() {
    using K = so::TsqKind;
    // SOUND/ACTION.TSQ:0x194..0x1A3 (tsq-tvb-format.md "Fully traced"); IRX handlers cited in table.
    {
        Bytes b = {0xE9, 0xF0, 0xF9, 0x0B, 0x00, 0xE1, 0x40, 0x00, 0xE0, 0xB4,
                   0xE2, 0x01, 0xA1, 0x64, 0xF0, 0xFF, 0xAA /* next phrase byte, not consumed */};
        std::vector<so::TsqRecord> recs;
        std::vector<std::size_t> offs;
        auto r = so::walkRecord(Dialect::TsqTrack, b, [&](const so::Record& rec) {
            recs.push_back(*so::decodeTsq(rec));
            offs.push_back(rec.offset);
            return true;
        });
        CHECK(r.status == WalkStatus::Terminated && r.records == 10 && r.consumed == 16);
        CHECK(r.remaining.size() == 1);
        CHECK(recs[0].kind == K::ReverbOff && recs[1].kind == K::KeyOff);
        // F9 fork: operands 0B 00 -> s16 +11; target = (record offset 2) + 3 + 11 = 16, i.e.
        // 0x194 + 16 = 0x1A4, the mirrored phrase (audio-runtime.md, ext_chan 0x45D4).
        CHECK(recs[2].kind == K::Fork && recs[2].s16 == 11 && offs[2] == 2);
        CHECK(so::tsqRelativeTarget(offs[2], recs[2].s16) == 16);
        CHECK(recs[3].kind == K::Pan && recs[3].b0 == 0x40 && recs[3].b1 == 0x00);
        CHECK(recs[4].kind == K::Volume && recs[4].b0 == 0xB4);
        CHECK(recs[5].kind == K::Program && recs[5].b0 == 0x01);
        CHECK(recs[6].kind == K::NoteOn && recs[6].b0 == 0x21);
        CHECK(recs[7].kind == K::Delta && recs[7].b0 == 100);
        CHECK(recs[8].kind == K::KeyOff && recs[9].kind == K::End);
    }
    // SOUND/ACTION.TSQ:0x29F..0x2A7: F8 FF F3 loops back to 0x29B (F8 at 0x2A5, next PC 0x2A8, -13).
    {
        Bytes b = {0xA5, 0x2D, 0xB1, 0x16, 0xF0, 0x35, 0xF8, 0xF3, 0xFF};
        std::vector<so::TsqRecord> recs;
        std::vector<std::size_t> offs;
        auto r = so::walkRecord(Dialect::TsqTrack, b, [&](const so::Record& rec) {
            recs.push_back(*so::decodeTsq(rec));
            offs.push_back(rec.offset);
            return true;
        });
        // F8 consumes F3 FF as its operands (they are data, NOT an F3 opcode and NOT an FF end).
        CHECK(r.status == WalkStatus::Exhausted && r.records == 7);
        CHECK(recs[6].kind == K::Jump && recs[6].s16 == -13 && offs[6] == 6);
        CHECK(0x29F + so::tsqRelativeTarget(offs[6], recs[6].s16) == 0x29B);
        CHECK(recs[0].b0 == 0x25 && recs[1].b0 == 0x2D && recs[2].b0 == 0x31);
    }
    // SOUND/BGM_02.TSQ:0x190..0x1A3 with EA (bend multiplier), notes, deltas.
    {
        Bytes b = {0xE1, 0x40, 0x40, 0xE0, 0x32, 0xE2, 0x12, 0xEA, 0xEE, 0x10, 0x96, 0x10,
                   0xF0, 0x27, 0xE0, 0x34, 0x9D, 0x0F, 0xF0, 0x1F};
        std::vector<so::TsqRecord> recs;
        auto r = so::walkRecord(Dialect::TsqTrack, b, [&](const so::Record& rec) {
            recs.push_back(*so::decodeTsq(rec));
            return true;
        });
        CHECK(r.status == WalkStatus::Exhausted && r.records == 13 && r.consumed == b.size());
        CHECK(recs[3].kind == K::PitchCent && recs[3].u16 == 0x10EE);  // EA: Q12 bend at +0xC
        CHECK(recs[4].kind == K::NoteOn && recs[4].b0 == 0x16);
        CHECK(recs[5].kind == K::Delta && recs[5].b0 == 0x10);
        CHECK(so::kTsqEaIsQ12BendMultiplier);
    }
    // E4 vs E5.  SOUND/BGM_02.TSQ:0x785 "E4 00 01 6F" (E4 value 0x0100, then delta 0x6F);
    // SOUND/BGM_03.TSQ:0x776 "E5 83 06" (absolute freq 0x0683 + retrigger).
    {
        Bytes b = {0xE4, 0x00, 0x01, 0x6F, 0xE5, 0x83, 0x06};
        std::vector<so::TsqRecord> recs;
        so::walkRecord(Dialect::TsqTrack, b, [&](const so::Record& rec) {
            recs.push_back(*so::decodeTsq(rec));
            return true;
        });
        CHECK(recs.size() == 3);
        CHECK(recs[0].kind == K::Pitch && recs[0].u16 == 0x0100);
        CHECK(recs[1].kind == K::Delta && recs[1].b0 == 0x6F);
        CHECK(recs[2].kind == K::KeyPitch && recs[2].u16 == 0x0683);
        CHECK(so::kTsqE5RetriggersVoice);
        CHECK(std::strcmp(so::lookup(Dialect::TsqTrack, 0xE5).name, "key_pitch") == 0);
        CHECK(std::strcmp(so::lookup(Dialect::TsqTrack, 0xEA).name, "pitch_cent") == 0);
    }
    // F9 with negative offset (backward fork) is a signed 16-bit field.
    {
        Bytes b = {0xF9, 0xF0, 0xFF};
        so::TsqRecord rec{};
        so::walkRecord(Dialect::TsqTrack, b, [&](const so::Record& r) {
            rec = *so::decodeTsq(r);
            return true;
        });
        CHECK(rec.kind == K::Fork && rec.s16 == -16 && rec.u16 == 0xFFF0);
        CHECK(so::tsqRelativeTarget(0, rec.s16) == -13);
    }
    // E3 tempo consumes two operand bytes (width 3).
    {
        Bytes b = {0xE3, 0x78, 0x00, 0x05};
        Seen s;
        auto r = walkAll(Dialect::TsqTrack, b, s);
        CHECK(r.records == 2 && s.widths[0] == 3 && s.ops[1] == 0x05);
    }
    // Unknown-width ops stop the walk and hand back the bytes, F3 included (default arm 0x5640
    // -> data_end 0x4940 stops the track on the real driver).  No invented semantics.
    for (int op : {0xF3, 0xEB, 0xF2, 0xFA, 0xFE}) {
        Bytes b = {0x10, 0xF0, static_cast<std::uint8_t>(op), 0x20, 0x30};
        Seen s;
        auto r = walkAll(Dialect::TsqTrack, b, s);
        CHECK(r.status == WalkStatus::UnknownWidth);
        CHECK(r.records == 2 && r.consumed == 2);
        CHECK(r.remaining.size() == 3 && r.remaining[0] == op);
    }
    // Truncation: E1 needs 3 bytes.
    {
        Bytes b = {0x01, 0xE1, 0x40};
        Seen s;
        auto r = walkAll(Dialect::TsqTrack, b, s);
        CHECK(r.status == WalkStatus::Truncated && r.records == 1 && r.remaining.size() == 2);
    }
    // Delta-0 stall: independent model of play() 0x52C4..0x52DC + step_t 0x4810.
    //   stored = (u16)(D - 1); every tick: cnt = (s16)(cnt - 1); event read when cnt == -1.
    auto ticksUntilRead = [](std::uint8_t d) {
        std::uint16_t cnt = so::tsqStoredCountdown(d);
        std::uint32_t ticks = 0;
        for (;;) {
            cnt = static_cast<std::uint16_t>(cnt - 1u);
            ++ticks;
            if (static_cast<std::int16_t>(cnt) == -1) return ticks;
        }
    };
    CHECK(so::tsqStoredCountdown(0) == 0xFFFF);
    CHECK(so::tsqStoredCountdown(1) == 0);
    CHECK(so::tsqStoredCountdown(0x7F) == 0x7E);
    for (int d = 0; d < 0x80; ++d) {
        const auto a = ticksUntilRead(static_cast<std::uint8_t>(d));
        CHECK(a == so::tsqTicksUntilNextEvent(static_cast<std::uint8_t>(d)));
    }
    CHECK(so::tsqTicksUntilNextEvent(0) == 65536);
    CHECK(so::tsqTicksUntilNextEvent(1) == 1);
}

// ------------------------------------------------------------------ image checks (optional)
struct Image {
    struct Seg { std::uint32_t va, off, fsz; };
    Bytes data;
    std::vector<Seg> segs;
    bool load(const std::filesystem::path& p) {
        std::ifstream f(p, std::ios::binary);
        if (!f) return false;
        data.assign(std::istreambuf_iterator<char>(f), std::istreambuf_iterator<char>());
        if (data.size() < 0x34 || std::memcmp(data.data(), "\x7f" "ELF", 4) != 0) return false;
        auto u32 = [&](std::size_t o) {
            return static_cast<std::uint32_t>(data[o] | data[o + 1] << 8 | data[o + 2] << 16 |
                                              static_cast<std::uint32_t>(data[o + 3]) << 24);
        };
        auto u16 = [&](std::size_t o) { return static_cast<std::uint32_t>(data[o] | data[o + 1] << 8); };
        const std::uint32_t phoff = u32(0x1c);
        const std::uint32_t phentsize = u16(0x2a), phnum = u16(0x2c);
        for (std::uint32_t i = 0; i < phnum; ++i) {
            const std::size_t o = phoff + static_cast<std::size_t>(i) * phentsize;
            if (o + 32 > data.size()) return false;
            if (u32(o) == 1) segs.push_back({u32(o + 8), u32(o + 4), u32(o + 16)});
        }
        return !segs.empty();
    }
    const std::uint8_t* at(std::uint32_t va, std::uint32_t n) const {
        for (const auto& s : segs) {
            if (va >= s.va && va - s.va + n <= s.fsz && s.off + (va - s.va) + n <= data.size()) {
                return data.data() + s.off + (va - s.va);
            }
        }
        return nullptr;
    }
    bool has(std::uint32_t va) const { return at(va, 4) != nullptr; }
    bool word(std::uint32_t va, std::uint32_t& out) const {
        const std::uint8_t* p = at(va, 4);
        if (!p) return false;
        out = static_cast<std::uint32_t>(p[0] | p[1] << 8 | p[2] << 16 |
                                         static_cast<std::uint32_t>(p[3]) << 24);
        return true;
    }
};

std::filesystem::path envOr(const char* name, const std::filesystem::path& fallback) {
    const char* v = std::getenv(name);
    return (v && *v) ? std::filesystem::path(v) : fallback;
}

void testElf() {
    Image elf;
    const char* home = std::getenv("HOME");
    std::filesystem::path p = envOr("RT_ELF", "build/rom/SLUS_203.98");
    if (!std::filesystem::exists(p) && home) {
        p = std::filesystem::path(home) / "Library/Application Support/RoadTripRecomp/disc/SLUS_203.98";
    }
    if (!std::filesystem::exists(p) || !elf.load(p)) {
        std::printf("SKIP: SLUS_203.98 not available (%s); ELF cross-checks not run\n", p.string().c_str());
        return;
    }
    std::printf("ELF cross-checks against %s\n", p.string().c_str());

    // Every SLUS grounding address must be inside a file-backed PT_LOAD.
    for (Dialect d : {Dialect::TownPrefix, Dialect::PadPattern}) {
        for (const auto& row : so::opTable(d)) CHECK(elf.has(row.grounding_addr));
    }
    // Town: jump table 0x2EC750 (1-based op -> word op-1) equals the case vaddrs in the table.
    for (std::uint32_t op = 1; op <= 0x1D; ++op) {
        std::uint32_t w = 0;
        CHECK(elf.word(0x2EC750 + 4 * (op - 1), w));
        CHECK(w == so::lookup(Dialect::TownPrefix, static_cast<std::uint8_t>(op)).grounding_addr);
    }
    // Dispatch bound: sltiu v0,v1,0x1D at 0x23AD2C; text test sltiu v0,v0,0x20 at 0x23B140.
    {
        std::uint32_t w = 0;
        CHECK(elf.word(0x23AD2C, w) && w == 0x2c62001d);
        CHECK(elf.word(0x23B140, w) && w == 0x2c420020);
    }
    // Rumble pattern bytes at 0x2EA3D0..0x2EA40F equal the committed fixture.
    {
        const std::uint8_t* p64 = elf.at(0x2EA3D0, 64);
        CHECK(p64 && std::memcmp(p64, kRumblePatterns.data(), 64) == 0);
    }
    // Pad interpreter instruction words (default arm and dispatch constants).
    struct W { std::uint32_t va, word; };
    const W padWords[] = {
        {0x20A710, 0x1072001b},  // beq v1,s2(0x82) -> 0x20A780
        {0x20A718, 0x28620083},  // slti v0,v1,0x83
        {0x20A724, 0x5071000e},  // beql v1,s1(0x80) -> 0x20A760
        {0x20A72C, 0x50700010},  // beql v1,s0(0x81) -> 0x20A770
        {0x20A740, 0x1079001b},  // beq v1,t9(0x84) -> 0x20A7B0
        {0x20A744, 0x28620084},  // slti v0,v1,0x84   (0x83 -> large off 0x20A7A0)
        {0x20A750, 0x5078001d},  // beql v1,t8(0xFF) -> 0x20A7C8
        {0x20A7E0, 0x90430000},  // default arm: lbu v1,(v0)  (hold literal)
        {0x20A7E4, 0xa1230000},  // sb v1,(t1)  (hold counter)
    };
    for (const auto& w : padWords) {
        std::uint32_t got = 0;
        CHECK(elf.word(w.va, got) && got == w.word);
    }
    // Town skip arms that fix the widths of 03/1D, 08 and 0E.  s0 = opcode+1 on handler entry
    // (0x23AD34), so a delay-slot `addiu s0,s0,N` on the not-taken path means width N+1.
    const W townWords[] = {
        {0x23AFD4, 0x50400059},  // beql v0,zero,0x23B13C   (ops 03 and 1D, shared tail)
        {0x23AFD8, 0x26100003},  // addiu s0,s0,3           -> width 4
        {0x23AE48, 0x544000bc},  // bnel v0,zero,0x23B13C   (op 08, Fcb8 arm)
        {0x23AE4C, 0x26100002},  // addiu s0,s0,2           -> width 3
        {0x23AF08, 0x5040008c},  // beql v0,zero,0x23B13C   (op 0E, flag 0x50 clear)
        {0x23AF0C, 0x26100006},  // addiu s0,s0,6           -> width 7
    };
    for (const auto& w : townWords) {
        std::uint32_t got = 0;
        CHECK(elf.word(w.va, got) && got == w.word);
    }
}

void testIrx() {
    Image irx;
    const char* home = std::getenv("HOME");
    std::filesystem::path p = envOr("RT_IRX", "");
    if (p.empty() && home) {
        p = std::filesystem::path(home) / "Library/Application Support/RoadTripRecomp/disc/SNDMOD.IRX";
    }
    if (p.empty() || !std::filesystem::exists(p) || !irx.load(p)) {
        std::printf("SKIP: SNDMOD.IRX not available; IRX cross-checks not run\n");
        return;
    }
    std::printf("IRX cross-checks against %s\n", p.string().c_str());
    for (const auto& row : so::opTable(Dialect::TsqTrack)) CHECK(irx.has(row.grounding_addr));

    // play() dispatch: compare-immediate `addiu v1,zero,OP` words and the jal handler words.
    struct Arm { std::uint32_t cmpVa; std::uint8_t op; std::uint32_t jalVa; std::uint32_t handler; };
    const Arm arms[] = {
        {0x53D8, 0xE0, 0x53EC, 0x4F74}, {0x5400, 0xE1, 0x5414, 0x4DAC},
        {0x5428, 0xE2, 0x5438, 0x4D34}, {0x544C, 0xE3, 0x545C, 0x5110},
        {0x5470, 0xE4, 0x5484, 0x5184}, {0x5498, 0xE5, 0x54AC, 0x4A40},
        {0x54C0, 0xE6, 0x54EC, 0x446C}, {0x5500, 0xE7, 0x552C, 0x4518},
        {0x5588, 0xEA, 0x5598, 0x48BC}, {0x55AC, 0xF0, 0x55C0, 0x4CE4},
        {0x55D4, 0xF1, 0x55E8, 0x49B4}, {0x55FC, 0xF8, 0x560C, 0x482C},
        {0x5620, 0xF9, 0x5630, 0x45D4},
    };
    for (const auto& a : arms) {
        std::uint32_t w = 0;
        CHECK(irx.word(a.cmpVa, w) && w == (0x24030000u | a.op));
        CHECK(irx.word(a.jalVa, w) && w == (0x0c000000u | (a.handler >> 2)));
        CHECK(so::lookup(Dialect::TsqTrack, a.op).grounding_addr == a.handler);
    }
    std::uint32_t w = 0;
    CHECK(irx.word(0x539C, w) && w == 0x2c4300e0);      // sltiu v1,v0,0xE0 : note-on range
    CHECK(irx.word(0x53C4, w) && w == 0x0c0012eb);      // jal key_on 0x4BAC
    CHECK(irx.word(0x537C, w) && w == 0x0c0011fc);      // jal step_t 0x47F0 (delta)
    CHECK(irx.word(0x5648, w) && w == 0x0c001250);      // default arm: jal data_end 0x4940
    CHECK(irx.word(0x4810, w) && w == 0x2464ffff);      // step_t: addiu a0,v1,-1  (stored D-1)
    CHECK(irx.word(0x52C4, w) && w == 0x2483ffff);      // play: cnt-1
    CHECK(irx.word(0x52D8, w) && w == 0x2403ffff);      // compare against -1
    CHECK(irx.word(0x4928, w) && w == 0xa443000c);      // pitch_cent: sh v1,0xC(v0)  (EA -> +0xC)
    CHECK(irx.word(0x4AB4, w) && w == 0xa443000e);      // key_pitch:  sh v1,0xE(v0)  (E5 -> +0xE)
    CHECK(irx.word(0x4B8C, w) && w == 0x24030002);      // key_pitch retrigger: v1 = 2 ...
    CHECK(irx.word(0x4B90, w) && w == 0xa0430002);      // ... sb v1,2(v0)
    CHECK(irx.word(0x4630, w) && w == 0xac430000);      // ext_chan operand 2 pointer store
    CHECK(irx.word(0x47CC, w) && w == 0xac440010);      // ext_chan: new pc = pc + s16
    CHECK(irx.word(0x47D8, w) && w == 0xa4400014);      // ext_chan: child countdown = 0
}

} // namespace

int main() {
    testTables();
    testPad();
    testTown();
    testTsq();
    testElf();
    testIrx();
    std::printf("script_ops_test: %d checks, %d failures\n", g_checks, g_fail);
    return g_fail == 0 ? 0 : 1;
}
