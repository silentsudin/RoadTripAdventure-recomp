// Hardware-style field-tile streaming algebra: the PS2 loads FLD tiles into
// four 0x3A8000-byte buffers at fieldTileData + slot * 0x3A8000 with
// slot = id & 3 (docs/native-reference/field-topology-status.md).  With the
// id grid col = id & 0xF, row = id >> 4, a resident set is a 4-column window
// of one row, and advancing the window one column swaps exactly one slot
// (incoming column col+4 aliases outgoing column col).  This models that
// window/slot algebra for the host's runtime streaming.
//
// Two drivers share the slot algebra:
//  * tick() walks a scripted serpentine window (a placeholder until the
//    authentic camera-to-tile selection lands with the nav tables,
//    docs/native-reference/nav-tables.md).
//  * advanceTo() is the position-driven demand path (the PS2 behaviour):
//    the car's global fixed position selects the tile through world_layout
//    (wrapToras + gridCellId, FUN_0021ba70/FUN_0021c8d0), and the streamer
//    emits the ordered Load/Keep/Evict commands that make the four buffers
//    hold the 4-half-column window around that cell.  The two drivers keep
//    separate state; advanceTo() never touches the scripted window and
//    vice versa.
#pragma once

#include <cstdint>
#include <vector>

namespace rt::native::gameplay {

inline constexpr int kFieldTileColumns = 16;
inline constexpr int kFieldTileRows = 4;
inline constexpr int kFieldTileSlots = 4;

// One buffer transition emitted by advanceTo().  A slot must be evicted
// before the incoming tile loads into it (evict commands precede their
// load in the vector); a resident tile the demand window still needs keeps.
struct TileStreamCommand {
    enum class Action { Load, Keep, Evict };
    Action action;
    int id;   // streaming tile id 0..63
    int slot; // hardware buffer index 0..3 == id & 3
};

class TileStreamer {
public:
    TileStreamer(int stepFrames, int startRow, int startCol)
        : stepFrames_(stepFrames < 1 ? 1 : stepFrames), row_(clampRow(startRow)), col_(clampCol(startCol)) {}

    // Advances one host frame; returns true on the frames the window moved,
    // when the caller should reload any slot whose resident tile changed.
    bool tick();

    int row() const { return row_; }
    int colStart() const { return col_; }
    int direction() const { return dir_; }

    // Hardware buffer index of a tile id.
    static int slotFor(int id) { return id & (kFieldTileSlots - 1); }

    // Resident tile id in `slot` (0 <= slot < kFieldTileSlots) for the
    // current scripted window.
    int idInSlot(int slot) const { return windowIdAt(row_, col_, slot); }

    // ---- position-driven demand (FUN_002201c0-style) ----
    // Recomputes demand from the car's global fixed position (world_layout
    // fixed units; any int32 is accepted, positions wrap onto the stored
    // 0x10000000 torus first).  The demanded cell id is world_layout's
    // gridCellId of the wrapped position; the demanded set is the existing
    // 4-consecutive-half-column window of that id row with the current
    // half-column one in from the left edge.  Returns the full transition
    // as an ordered command vector, evicts first (least-recently-used
    // resident first), then keeps and loads each nearest-first by torus
    // distance from the car to the tile centre (ties by ascending id).
    // Calling it again with the same cell emits four Keeps and no I/O.
    std::vector<TileStreamCommand> advanceTo(std::int32_t globalX, std::int32_t globalZ);

    // Tile id actually loaded in `slot` (0 <= slot < kFieldTileSlots) by
    // the demand path after the last advanceTo(); -1 before the first call.
    int residentId(int slot) const { return demandResident_[slot]; }

    // Demanded cell id from the last advanceTo() (gridCellId of the wrapped
    // position); -1 before the first call.
    int lastDemandedId() const { return demandedId_; }

private:
    static int clampRow(int r);
    static int clampCol(int c);

    // Window algebra shared by both drivers: the id of `slot` when the
    // resident window is the 4 consecutive columns starting at colStart on
    // `row`.  slot = id & 3 picks the unique column of the window with that
    // residue (proven one-slot-per-column-swap invariant).
    static int windowIdAt(int row, int colStart, int slot) {
        return row * kFieldTileColumns + colStart + ((slot - colStart) & (kFieldTileSlots - 1));
    }

    // Scripted serpentine state (tick() driver).
    int stepFrames_;
    int row_;
    int col_;
    int dir_ = 1;
    int elapsed_ = 0;

    // Position-driven demand state (advanceTo() driver).
    int demandResident_[kFieldTileSlots] = {-1, -1, -1, -1};
    std::uint32_t demandStamp_[kFieldTileSlots] = {};
    std::uint32_t demandClock_ = 0;
    int demandedId_ = -1;
};

} // namespace rt::native::gameplay
