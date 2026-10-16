// Field-tile streaming algebra tests: slot aliasing, window sweep, and the
// one-slot-per-column-swap invariant.  No game data.
#include "../../src/native/gameplay/tile_streamer.h"

#include <cstdio>
#include <set>

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

int main() {
    testSlotAliasing();
    testStepGating();
    testSweepAndFlip();
    testSingleSlotSwapInvariant();
    testFullCoverage();
    testClampedStarts();
    if (g_fail != 0) {
        std::fprintf(stderr, "tile_streamer: %d checks failed\n", g_fail);
        return 1;
    }
    std::printf("tile_streamer: all checks passed\n");
    return 0;
}
