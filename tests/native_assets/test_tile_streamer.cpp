// Field-tile streaming algebra tests: slot aliasing, window sweep, and the
// one-slot-per-column-swap invariant, plus the position-driven demand path
// (advanceTo): world_layout-selected cell, window rebuild, ordered
// Load/Keep/Evict commands, torus/shear boundaries, and a golden trace over
// 50 positions.  No game data.
#include "../../src/native/gameplay/tile_streamer.h"
#include "../../src/native/gameplay/world_layout.h"

#include <cstdio>
#include <set>
#include <string>
#include <vector>

using namespace rt::native::gameplay;

static int g_fail = 0;
#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #c); ++g_fail; } } while (0)

static int changedSlots(const TileStreamer& before, const TileStreamer& after) {
    int changed = 0;
    for (int slot = 0; slot < kFieldTileSlots; ++slot) {
        if (before.idInSlot(slot) != after.idInSlot(slot)) ++changed;
    }
    return changed;
}

static void testSlotAliasing() {
    // slot = id & 3 (the four 0x3A8000 streaming buffers).
    CHECK(TileStreamer::slotFor(0) == 0);
    CHECK(TileStreamer::slotFor(7) == 3);
    CHECK(TileStreamer::slotFor(16) == 0);
    CHECK(TileStreamer::slotFor(63) == 3);

    TileStreamer s{1, 0, 0};
    for (int slot = 0; slot < kFieldTileSlots; ++slot) {
        const int id = s.idInSlot(slot);
        CHECK(TileStreamer::slotFor(id) == slot);
        CHECK(id == slot);  // row 0, col window 0..3 -> ids 0..3
    }
}

static void testStepGating() {
    TileStreamer s{3, 2, 0};
    CHECK(!s.tick());
    CHECK(!s.tick());
    CHECK(s.tick());
    CHECK(!s.tick());
    CHECK(!s.tick());
    CHECK(s.tick());
    CHECK(s.colStart() == 2);  // two window moves, one column each
}

static void testSweepAndFlip() {
    TileStreamer s{1, 2, 0};
    // Columns 0..12 take 12 moves; direction still forward at the edge.
    for (int i = 0; i < 12; ++i) { CHECK(s.tick()); }
    CHECK(s.colStart() == 12);
    CHECK(s.direction() == 1);
    CHECK(s.row() == 2);
    // Next move flips at the edge and drops a row: same columns, new ids.
    const TileStreamer before{s};
    CHECK(s.tick());
    CHECK(s.colStart() == 12);
    CHECK(s.direction() == -1);
    CHECK(s.row() == 3);
    CHECK(changedSlots(before, s) == 4);
    // Back sweep to the left edge.
    for (int i = 0; i < 12; ++i) { CHECK(s.tick()); }
    CHECK(s.colStart() == 0);
    CHECK(s.direction() == -1);
    CHECK(s.row() == 3);
    // Left flip wraps rows 3 -> 0.
    CHECK(s.tick());
    CHECK(s.direction() == 1);
    CHECK(s.row() == 0);
    CHECK(changedSlots(before, s) == 4);
}

static void testSingleSlotSwapInvariant() {
    TileStreamer s{1, 0, 0};
    for (int step = 0; step < 400; ++step) {
        const TileStreamer before{s};
        CHECK(s.tick());
        const int changed = changedSlots(before, s);
        // Mid-row steps swap exactly one slot; row changes swap all four.
        CHECK(changed == 1 || changed == 4);
        CHECK(s.colStart() >= 0);
        CHECK(s.colStart() + kFieldTileSlots <= kFieldTileColumns);
        for (int slot = 0; slot < kFieldTileSlots; ++slot) {
            const int id = s.idInSlot(slot);
            CHECK(TileStreamer::slotFor(id) == slot);
            CHECK(id >= 0 && id < kFieldTileColumns * kFieldTileRows);
            CHECK(id >> 4 == s.row());
            const int col = id & 0xF;
            CHECK(col >= s.colStart() && col < s.colStart() + kFieldTileSlots);
        }
    }
}

static void testFullCoverage() {
    // A full serpentine cycle visits every (row, colStart) window: 4 rows x
    // 13 starts = 52.
    TileStreamer s{1, 0, 0};
    std::set<std::pair<int, int>> windows;
    windows.insert({s.row(), s.colStart()});
    for (int step = 0; step < 104; ++step) {  // two full cycles
        CHECK(s.tick());
        windows.insert({s.row(), s.colStart()});
    }
    CHECK(windows.size() == static_cast<std::size_t>(kFieldTileRows) *
                                (kFieldTileColumns - kFieldTileSlots + 1));
}

static void testClampedStarts() {
    TileStreamer a{1, -3, -5};   // row wraps, col clamps to 0
    CHECK(a.row() == 1);
    CHECK(a.colStart() == 0);
    TileStreamer b{1, 99, 99};   // row 99 % 4 == 3, col clamps to 12
    CHECK(b.row() == 3);
    CHECK(b.colStart() == 12);
    TileStreamer c{0, 0, 0};     // step clamps to 1
    CHECK(c.tick());
}

// ---- position-driven demand (advanceTo) ------------------------------------
//
// The demand RULE (window alignment, command order) is mirrored here on
// purpose; every piece of geometry (torus wrap, cell id, placement) comes
// from world_layout exactly as the streamer uses it.

struct TracePoint {
    std::int32_t x;
    std::int32_t z;
};

// Independent derivation of the demanded window: wrap through world_layout,
// cell id from gridCellId, then the 4-half-column window starting one
// half-column before the current one (clamped at the torus edges).
static std::array<int, kFieldTileSlots> expectedWindow(std::int32_t x, std::int32_t z) {
    const FixedPos p = wrapToras({x, z});
    const int id = gridCellId(p);
    const int row = (id >> 4) & (kFieldTileRows - 1);
    int colStart = (id & (kFieldTileColumns - 1)) - 1;
    if (colStart < 0) colStart = 0;
    if (colStart > kFieldTileColumns - kFieldTileSlots) colStart = kFieldTileColumns - kFieldTileSlots;
    std::array<int, kFieldTileSlots> window{};
    for (int slot = 0; slot < kFieldTileSlots; ++slot) {
        window[static_cast<std::size_t>(slot)] =
            row * kFieldTileColumns + colStart + ((slot - colStart) & (kFieldTileSlots - 1));
    }
    return window;
}

// Per-step demand invariants, checked on every position of every walk: the
// command vector must be exactly the per-slot residency transition (previous
// resident -> window id), phases ordered evict -> keep -> load, with
// slot = id & 3 throughout.
static void checkAdvanceInvariants(TileStreamer& s, TracePoint p) {
    const FixedPos wrapped = wrapToras({p.x, p.z});
    const int cell = gridCellId(wrapped);
    const std::array<int, kFieldTileSlots> window = expectedWindow(p.x, p.z);

    int previous[kFieldTileSlots];
    for (int slot = 0; slot < kFieldTileSlots; ++slot) previous[slot] = s.residentId(slot);

    const std::vector<TileStreamCommand> cmds = s.advanceTo(p.x, p.z);
    CHECK(s.lastDemandedId() == cell);  // demand id = world_layout gridCellId

    int count[3][kFieldTileSlots] = {};  // [Evict, Keep, Load][slot]
    int id[3][kFieldTileSlots] = {};
    int lastPhase = -1;
    for (const TileStreamCommand& c : cmds) {
        CHECK(c.id >= 0 && c.id < kFieldTileColumns * kFieldTileRows);
        CHECK(c.slot >= 0 && c.slot < kFieldTileSlots);
        CHECK(c.slot == TileStreamer::slotFor(c.id));  // hardware pinning
        const int phase = c.action == TileStreamCommand::Action::Evict   ? 0
                          : c.action == TileStreamCommand::Action::Keep  ? 1
                                                                        : 2;
        CHECK(phase >= lastPhase);  // evicts, then keeps, then loads
        lastPhase = phase;
        CHECK(count[phase][c.slot] == 0);  // one command per slot per action
        count[phase][c.slot] = 1;
        id[phase][c.slot] = c.id;
    }
    for (int slot = 0; slot < kFieldTileSlots; ++slot) {
        if (previous[slot] == -1) {
            // Cold slot: loads straight in, nothing to evict or keep.
            CHECK(count[0][slot] == 0 && count[1][slot] == 0 && count[2][slot] == 1);
            CHECK(id[2][slot] == window[slot]);
        } else if (previous[slot] == window[slot]) {
            // Resident tile still demanded: keep, no I/O.
            CHECK(count[0][slot] == 0 && count[1][slot] == 1 && count[2][slot] == 0);
            CHECK(id[1][slot] == window[slot]);
        } else {
            // Slot repinned: evict the old resident, then load the window id.
            CHECK(count[0][slot] == 1 && count[1][slot] == 0 && count[2][slot] == 1);
            CHECK(id[0][slot] == previous[slot] && id[2][slot] == window[slot]);
        }
        CHECK(s.residentId(slot) == window[slot]);  // residency = demand window
    }
    bool cellResident = false;
    for (int slot = 0; slot < kFieldTileSlots; ++slot) {
        if (s.residentId(slot) == cell) cellResident = true;
    }
    CHECK(cellResident);  // the car's own tile is always resident
}

static void testDemandInitialLoad() {
    TileStreamer s{1, 0, 0};
    CHECK(s.residentId(0) == -1);
    CHECK(s.lastDemandedId() == -1);

    const std::vector<TileStreamCommand> cmds = s.advanceTo(0x00800000, 0x01000000);
    CHECK(cmds.size() == 4);
    for (const TileStreamCommand& c : cmds) {
        CHECK(c.action == TileStreamCommand::Action::Load);  // cold buffers: loads only
    }
    // Nearest-first from the tile-0 centre (Z = 0x01000000 = 2 quanta):
    // tile 0 (d2 = 1), then the tied pair 1/2 (d2 = 25 each, id ascending),
    // then tile 3 (d2 = 65).
    CHECK(cmds[0].id == 0 && cmds[1].id == 1 && cmds[2].id == 2 && cmds[3].id == 3);
    CHECK(s.lastDemandedId() == 0);
    const std::array<int, kFieldTileSlots> window = expectedWindow(0x00800000, 0x01000000);
    for (int slot = 0; slot < kFieldTileSlots; ++slot) {
        CHECK(s.residentId(slot) == window[static_cast<std::size_t>(slot)]);
    }
}

static void testDemandRepeatIsAllKeeps() {
    TileStreamer s{1, 0, 0};
    s.advanceTo(0x04800000, 0x04800000);
    const std::vector<TileStreamCommand> again = s.advanceTo(0x04800000, 0x04800000);
    CHECK(again.size() == 4);
    for (const TileStreamCommand& c : again) {
        CHECK(c.action == TileStreamCommand::Action::Keep);  // same cell: no I/O
    }
    // Sub-tile motion inside the same cell also keeps everything.
    const std::vector<TileStreamCommand> drift = s.advanceTo(0x04400000, 0x04C00000);
    CHECK(drift.size() == 4);
    for (const TileStreamCommand& c : drift) {
        CHECK(c.action == TileStreamCommand::Action::Keep);
    }
}

static void testDemandSingleSlotSwap() {
    // One full tile +X on an even row: the window slides one half-column,
    // which aliases exactly one slot (incoming col+4 replaces outgoing col).
    TileStreamer s{1, 0, 0};
    s.advanceTo(0x00800000, 0x01000000);
    const std::vector<TileStreamCommand> cmds = s.advanceTo(0x02800000, 0x01000000);
    int loads = 0, keeps = 0, evicts = 0;
    for (const TileStreamCommand& c : cmds) {
        switch (c.action) {
            case TileStreamCommand::Action::Load: ++loads; break;
            case TileStreamCommand::Action::Keep: ++keeps; break;
            case TileStreamCommand::Action::Evict: ++evicts; break;
        }
    }
    CHECK(loads == 1 && evicts == 1 && keeps == 3);
    CHECK(cmds[0].action == TileStreamCommand::Action::Evict && cmds[0].id == 0 && cmds[0].slot == 0);
    CHECK(cmds.back().action == TileStreamCommand::Action::Load && cmds.back().id == 4 && cmds.back().slot == 0);
}

static void testDemandShearBoundarySingleSwap() {
    // Crossing the Z bit-25 boundary inside one A row (world row 0 -> 1) at
    // X = 0x04800000: the odd-row shear steps the half-column 4 -> 3, the
    // window slides one half-column, one slot swaps.
    TileStreamer s{1, 0, 0};
    s.advanceTo(0x04800000, 0x01FFFFFF);
    CHECK(s.lastDemandedId() == 4);
    const std::vector<TileStreamCommand> cmds = s.advanceTo(0x04800000, 0x02000000);
    CHECK(s.lastDemandedId() == 3);
    int loads = 0, keeps = 0, evicts = 0;
    int evicted = -1, loaded = -1;
    for (const TileStreamCommand& c : cmds) {
        switch (c.action) {
            case TileStreamCommand::Action::Load: ++loads; loaded = c.id; break;
            case TileStreamCommand::Action::Keep: ++keeps; break;
            case TileStreamCommand::Action::Evict: ++evicts; evicted = c.id; break;
        }
    }
    CHECK(loads == 1 && evicts == 1 && keeps == 3);
    CHECK(evicted == 6 && loaded == 2);  // window [3..6] -> [2..5]
}

static void testDemandARowSwapEvictsLRU() {
    // Crossing an A-row boundary swaps all four slots; the eviction order is
    // least-recently-used first, i.e. the previous call's load order.
    TileStreamer s{1, 0, 0};
    s.advanceTo(0x00800000, 0x01000000);  // load order (nearest first): 0, 1, 2, 3
    const std::vector<TileStreamCommand> cmds = s.advanceTo(0x04800000, 0x04800000);
    int loads = 0, keeps = 0, evicts = 0;
    std::vector<int> evictIds, loadIds;
    for (const TileStreamCommand& c : cmds) {
        switch (c.action) {
            case TileStreamCommand::Action::Load: ++loads; loadIds.push_back(c.id); break;
            case TileStreamCommand::Action::Keep: ++keeps; break;
            case TileStreamCommand::Action::Evict: ++evicts; evictIds.push_back(c.id); break;
        }
    }
    CHECK(loads == 4 && evicts == 4 && keeps == 0);
    CHECK(evictIds == std::vector<int>({0, 1, 2, 3}));  // LRU = first load evicted first
    // Nearest-first from (0x04800000, 0x04800000) over window ids 19..22:
    // 20 (d2 = 2), tied 19/22 (d2 = 26 each, id ascending), 21 (d2 = 34).
    CHECK(loadIds == std::vector<int>({20, 19, 22, 21}));
    // Repeating the cell now keeps in the same nearest-first order.
    const std::vector<TileStreamCommand> again = s.advanceTo(0x04800000, 0x04800000);
    std::vector<int> keepIds;
    for (const TileStreamCommand& c : again) keepIds.push_back(c.id);
    CHECK(keepIds == loadIds);
}

static void testDemandTorusDistanceWrap() {
    // Just past the X seam on the sheared row: tile 15's centre sits across
    // the 0x10000000 wrap, and torus-wrapped distance must rank it nearest
    // (unwrapped distance would rank it last).
    TileStreamer s{1, 0, 0};
    const std::vector<TileStreamCommand> cmds = s.advanceTo(0x00800000, 0x03000000);
    std::vector<int> loadIds;
    for (const TileStreamCommand& c : cmds) loadIds.push_back(c.id);
    CHECK(loadIds == std::vector<int>({15, 13, 14, 12}));
    CHECK(s.lastDemandedId() == 15);  // sheared wrap tile, extent [0x0F.., 0x11..)
}

// The 50-position golden walk: half-tile sweeps along even and sheared odd
// rows, the Z bit-25 shear boundary, the A-row boundary, X and Z torus wraps
// (including negative inputs), exact half-column boundaries, a teleport, and
// no-op revisits.
static std::vector<TracePoint> goldenWalk() {
    constexpr std::int32_t kHalf = 0x01000000;
    std::vector<TracePoint> pts;
    auto push = [&pts](std::int32_t x, std::int32_t z) { pts.push_back({x, z}); };

    // A: world row 0, +X half-tile sweep across both torus seam crossings
    // (k = 16 wraps 0x10800000 back onto 0x00800000).
    for (int k = 0; k <= 16; ++k) push(0x00800000 + k * kHalf, 0x01000000);
    // B: negative X input takes the FUN_0021c8d0 correction path.
    push(-kHalf, 0x01000000);
    // C: Z bit-25 shear boundary, then the A-row boundary at 0x04000000.
    push(0x04800000, 0x01800000);
    push(0x04800000, 0x01FFFFFF);
    push(0x04800000, 0x02000000);
    push(0x04800000, 0x02800000);
    push(0x04800000, 0x03000000);
    push(0x04800000, 0x03FFFFFF);
    push(0x04800000, 0x04000000);
    push(0x04800000, 0x04800000);
    // D: world row 1 (sheared odd half-columns), +X half-tile sweep.
    for (int k = 0; k <= 8; ++k) push(0x00800000 + k * kHalf, 0x03000000);
    // E: Z torus wrap around 0x10000000, exact top edge, negative Z.
    push(0x04800000, 0x0F800000);
    push(0x04800000, 0x0FFFFFFF);
    push(0x04800000, 0x10000000);
    push(0x04800000, -kHalf);
    // F: teleport into Peach Town's tile (id 11), then no-op revisits.
    push(0x0B800000, 0x03000000);
    push(0x0B800000, 0x03000000);
    push(0x0B400000, 0x02C00000);
    // G: exact X half-column boundaries on the sheared row (one half-column
    // off from the even-row boundaries).
    push(0x00FFFFFF, 0x03000000);
    push(0x01000000, 0x03000000);
    push(0x02FFFFFF, 0x03000000);
    push(0x03000000, 0x03000000);
    // H: -X half-tile steps back along world row 0.
    push(0x04800000, 0x01000000);
    push(0x03800000, 0x01000000);
    push(0x02800000, 0x01000000);
    push(0x01800000, 0x01000000);
    return pts;
}

// Runs the walk, rendering one canonical line per position:
//   "#NN pos=XXXXXXXX,XXXXXXXX id=II L<id>@<slot> K.. E.."
// (pos is the raw input in hex, so negative/wrap inputs are pinned too).
static std::string renderWalk(const std::vector<TracePoint>& pts) {
    TileStreamer s{1, 0, 0};
    std::string out;
    for (std::size_t i = 0; i < pts.size(); ++i) {
        const std::vector<TileStreamCommand> cmds = s.advanceTo(pts[i].x, pts[i].z);
        char head[64];
        std::snprintf(head, sizeof(head), "#%02u pos=%08x,%08x id=%02d",
                      static_cast<unsigned>(i),
                      static_cast<unsigned>(static_cast<std::uint32_t>(pts[i].x)),
                      static_cast<unsigned>(static_cast<std::uint32_t>(pts[i].z)),
                      s.lastDemandedId());
        out += head;
        for (const TileStreamCommand& c : cmds) {
            char cmd[16];
            std::snprintf(cmd, sizeof(cmd), " %c%d@%d",
                          c.action == TileStreamCommand::Action::Load ? 'L'
                              : c.action == TileStreamCommand::Action::Keep ? 'K' : 'E',
                          c.id, c.slot);
            out += cmd;
        }
        out += '\n';
    }
    return out;
}

// Golden command trace: same position sequence must always produce exactly
// this command stream (run twice on fresh streamers and against the pin).
// One line per position: raw input position (hex), demanded cell id, then the
// ordered commands L<id>@<slot> / K<id>@<slot> / E<id>@<slot>.
static const char* kGoldenTrace = R"(#00 pos=00800000,01000000 id=00 L0@0 L1@1 L2@2 L3@3
#01 pos=01800000,01000000 id=00 K0@0 K2@2 K1@1 K3@3
#02 pos=02800000,01000000 id=02 E0@0 K2@2 K1@1 K3@3 L4@0
#03 pos=03800000,01000000 id=02 K2@2 K4@0 K3@3 K1@1
#04 pos=04800000,01000000 id=04 E2@2 E1@1 K4@0 K3@3 L5@1 L6@2
#05 pos=05800000,01000000 id=04 K4@0 K6@2 K5@1 K3@3
#06 pos=06800000,01000000 id=06 E4@0 E3@3 K6@2 K5@1 L7@3 L8@0
#07 pos=07800000,01000000 id=06 K6@2 K8@0 K7@3 K5@1
#08 pos=08800000,01000000 id=08 E6@2 E5@1 K8@0 K7@3 L9@1 L10@2
#09 pos=09800000,01000000 id=08 K8@0 K10@2 K9@1 K7@3
#10 pos=0a800000,01000000 id=10 E8@0 E7@3 K10@2 K9@1 L11@3 L12@0
#11 pos=0b800000,01000000 id=10 K10@2 K12@0 K11@3 K9@1
#12 pos=0c800000,01000000 id=12 E10@2 E9@1 K12@0 K11@3 L13@1 L14@2
#13 pos=0d800000,01000000 id=12 K12@0 K14@2 K13@1 K11@3
#14 pos=0e800000,01000000 id=14 E11@3 K14@2 K12@0 K13@1 L15@3
#15 pos=0f800000,01000000 id=14 K14@2 K15@3 K12@0 K13@1
#16 pos=10800000,01000000 id=00 E14@2 E15@3 E12@0 E13@1 L0@0 L1@1 L2@2 L3@3
#17 pos=ff000000,01000000 id=14 E0@0 E1@1 E2@2 E3@3 L14@2 L12@0 L13@1 L15@3
#18 pos=04800000,01800000 id=04 E14@2 E12@0 E13@1 E15@3 L4@0 L3@3 L5@1 L6@2
#19 pos=04800000,01ffffff id=04 K4@0 K3@3 K5@1 K6@2
#20 pos=04800000,02000000 id=03 E6@2 K3@3 K4@0 K5@1 L2@2
#21 pos=04800000,02800000 id=03 K3@3 K4@0 K5@1 K2@2
#22 pos=04800000,03000000 id=03 K3@3 K5@1 K4@0 K2@2
#23 pos=04800000,03ffffff id=03 K3@3 K5@1 K4@0 K2@2
#24 pos=04800000,04000000 id=20 E3@3 E5@1 E4@0 E2@2 L20@0 L22@2 L19@3 L21@1
#25 pos=04800000,04800000 id=20 K20@0 K19@3 K22@2 K21@1
#26 pos=00800000,03000000 id=15 E20@0 E19@3 E22@2 E21@1 L15@3 L13@1 L14@2 L12@0
#27 pos=01800000,03000000 id=01 E15@3 E13@1 E14@2 E12@0 L1@1 L0@0 L2@2 L3@3
#28 pos=02800000,03000000 id=01 K1@1 K3@3 K2@2 K0@0
#29 pos=03800000,03000000 id=03 E1@1 E0@0 K3@3 K2@2 L4@0 L5@1
#30 pos=04800000,03000000 id=03 K3@3 K5@1 K4@0 K2@2
#31 pos=05800000,03000000 id=05 E3@3 E2@2 K5@1 K4@0 L6@2 L7@3
#32 pos=06800000,03000000 id=05 K5@1 K7@3 K6@2 K4@0
#33 pos=07800000,03000000 id=07 E5@1 E4@0 K7@3 K6@2 L8@0 L9@1
#34 pos=08800000,03000000 id=07 K7@3 K9@1 K8@0 K6@2
#35 pos=04800000,0f800000 id=51 E7@3 E9@1 E8@0 E6@2 L51@3 L53@1 L52@0 L50@2
#36 pos=04800000,0fffffff id=51 K51@3 K53@1 K52@0 K50@2
#37 pos=04800000,10000000 id=04 E51@3 E53@1 E52@0 E50@2 L4@0 L6@2 L3@3 L5@1
#38 pos=04800000,ff000000 id=51 E4@0 E6@2 E3@3 E5@1 L51@3 L53@1 L52@0 L50@2
#39 pos=0b800000,03000000 id=11 E51@3 E53@1 E52@0 E50@2 L11@3 L10@2 L12@0 L13@1
#40 pos=0b800000,03000000 id=11 K11@3 K10@2 K12@0 K13@1
#41 pos=0b400000,02c00000 id=11 K11@3 K10@2 K12@0 K13@1
#42 pos=00ffffff,03000000 id=15 E11@3 E10@2 K13@1 K12@0 L15@3 L14@2
#43 pos=01000000,03000000 id=01 E13@1 E12@0 E15@3 E14@2 L1@1 L0@0 L2@2 L3@3
#44 pos=02ffffff,03000000 id=01 K1@1 K3@3 K2@2 K0@0
#45 pos=03000000,03000000 id=03 E1@1 E0@0 K3@3 K2@2 L4@0 L5@1
#46 pos=04800000,01000000 id=04 E2@2 K4@0 K3@3 K5@1 L6@2
#47 pos=03800000,01000000 id=02 E5@1 E6@2 K4@0 K3@3 L2@2 L1@1
#48 pos=02800000,01000000 id=02 K2@2 K1@1 K3@3 K4@0
#49 pos=01800000,01000000 id=00 E4@0 K2@2 K1@1 K3@3 L0@0
)";

static void testGoldenTrace() {
    const std::vector<TracePoint> pts = goldenWalk();
    CHECK(pts.size() == 50);

    // Every step must satisfy the demand invariants (own walk instance).
    {
        TileStreamer s{1, 0, 0};
        for (const TracePoint& p : pts) checkAdvanceInvariants(s, p);
    }

    const std::string runA = renderWalk(pts);
    const std::string runB = renderWalk(pts);  // fresh streamer, same sequence
    CHECK(runA == runB);
    if (runA != kGoldenTrace) {
        // Diagnostic: print the actual trace so the golden pin can be
        // regenerated deliberately.
        std::fprintf(stderr, "---- actual trace begin ----\n%s---- actual trace end ----\n", runA.c_str());
    }
    CHECK(runA == kGoldenTrace);
}

int main() {
    testSlotAliasing();
    testStepGating();
    testSweepAndFlip();
    testSingleSlotSwapInvariant();
    testFullCoverage();
    testClampedStarts();
    testDemandInitialLoad();
    testDemandRepeatIsAllKeeps();
    testDemandSingleSlotSwap();
    testDemandShearBoundarySingleSwap();
    testDemandARowSwapEvictsLRU();
    testDemandTorusDistanceWrap();
    testGoldenTrace();
    if (g_fail != 0) {
        std::fprintf(stderr, "tile_streamer: %d checks failed\n", g_fail);
        return 1;
    }
    std::printf("tile_streamer: all checks passed\n");
    return 0;
}
