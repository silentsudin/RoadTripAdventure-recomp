// Hardware-style field-tile streaming algebra: the PS2 loads FLD tiles into
// four 0x3A8000-byte buffers at fieldTileData + slot * 0x3A8000 with
// slot = id & 3 (docs/native-reference/field-topology-status.md).  With the
// id grid col = id & 0xF, row = id >> 4, a resident set is a 4-column window
// of one row, and advancing the window one column swaps exactly one slot
// (incoming column col+4 aliases outgoing column col).  This models that
// window/slot algebra for the host's runtime streaming.  Which world path the
// window follows is scripted here (a serpentine sweep) because the authentic
// camera-to-tile selection lives in the still-open nav tables
// (docs/native-reference/nav-tables.md when it lands).
#pragma once

namespace rt::native::gameplay {

inline constexpr int kFieldTileColumns = 16;
inline constexpr int kFieldTileRows = 4;
inline constexpr int kFieldTileSlots = 4;

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
    // current window.
    int idInSlot(int slot) const;

private:
    static int clampRow(int r);
    static int clampCol(int c);

    int stepFrames_;
    int row_;
    int col_;
    int dir_ = 1;
    int elapsed_ = 0;
};

} // namespace rt::native::gameplay
