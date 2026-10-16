#include "gameplay/tile_streamer.h"

namespace rt::native::gameplay {

int TileStreamer::clampRow(int r) {
    r %= kFieldTileRows;
    return r < 0 ? r + kFieldTileRows : r;
}

int TileStreamer::clampCol(int c) {
    const int lastStart = kFieldTileColumns - kFieldTileSlots;
    if (c < 0) return 0;
    return c > lastStart ? lastStart : c;
}

bool TileStreamer::tick() {
    ++elapsed_;
    if (elapsed_ % stepFrames_ != 0) return false;
    col_ += dir_;
    if (dir_ > 0 && col_ + kFieldTileSlots > kFieldTileColumns) {
        // Right edge: flip and drop to the next row (all four slots reload).
        dir_ = -1;
        col_ = kFieldTileColumns - kFieldTileSlots;
        row_ = clampRow(row_ + 1);
    } else if (dir_ < 0 && col_ < 0) {
        dir_ = 1;
        col_ = 0;
        row_ = clampRow(row_ + 1);
    }
    return true;
}

int TileStreamer::idInSlot(int slot) const {
    // Window columns are col_ .. col_+3; row * 16 is a multiple of 4, so
    // slot = id & 3 picks the column c with (c - col_) ≡ slot (mod 4).
    const int offset = (slot - col_) & (kFieldTileSlots - 1);
    return row_ * kFieldTileColumns + col_ + offset;
}

} // namespace rt::native::gameplay
