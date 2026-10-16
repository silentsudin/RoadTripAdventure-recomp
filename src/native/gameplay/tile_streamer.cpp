#include "gameplay/tile_streamer.h"

#include "gameplay/world_layout.h"

#include <algorithm>
#include <array>

namespace rt::native::gameplay {

namespace {

// Shortest signed delta between two points on the 0x10000000 torus, so
// tiles across the wrap seam stay near instead of measuring the long way
// around the world.
std::int64_t wrapTorusDelta(std::int64_t d) {
    d %= kTorasWrapUnits;
    if (d >= kTorasWrapUnits / 2) d -= kTorasWrapUnits;
    else if (d < -(kTorasWrapUnits / 2)) d += kTorasWrapUnits;
    return d;
}

// Squared fixed-unit distance from a wrapped position to a tile's centre
// (placement comes from the proven world_layout mapping).  Integer-only:
// no floating point anywhere in the demand ordering.
std::uint64_t torusDistanceSquared(FixedPos p, int id) {
    const TilePlacement t = tilePlacement(id);
    const std::int64_t dx = wrapTorusDelta(p.x - (t.x0 + kHalfTileFixedUnits));
    const std::int64_t dz = wrapTorusDelta(p.z - (t.z0 + kHalfTileFixedUnits));
    return static_cast<std::uint64_t>(dx * dx + dz * dz);
}

} // namespace

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

std::vector<TileStreamCommand> TileStreamer::advanceTo(std::int32_t globalX, std::int32_t globalZ) {
    // All geometry goes through world_layout: wrap onto the stored torus
    // (FUN_0021ba70 masks & 0x0fffffff), then FUN_0021c8d0 picks the cell.
    const FixedPos p = wrapToras({globalX, globalZ});
    const int id = gridCellId(p);
    const int demandRow = (id >> 4) & (kFieldTileRows - 1);
    // Demand window = the existing 4-consecutive-half-column window of this
    // id row; the current half-column sits one in from the left edge and the
    // start clamps at both torus edges.
    const int demandCol = clampCol((id & (kFieldTileColumns - 1)) - 1);

    int incoming[kFieldTileSlots];
    for (int slot = 0; slot < kFieldTileSlots; ++slot) {
        incoming[slot] = windowIdAt(demandRow, demandCol, slot);
    }

    std::vector<TileStreamCommand> commands;
    commands.reserve(3 * kFieldTileSlots);

    // Phase 1: evict outgoing residents, least-recently-used first.  Slot
    // choice is pinned by the hardware (slot = id & 3), so the outgoing tile
    // of a changed slot is simply whichever id held it.
    struct EvictCandidate {
        std::uint32_t stamp;
        int slot;
        int id;
    };
    std::array<EvictCandidate, kFieldTileSlots> evicts{};
    int evictCount = 0;
    for (int slot = 0; slot < kFieldTileSlots; ++slot) {
        const int resident = demandResident_[slot];
        if (resident >= 0 && resident != incoming[slot]) {
            evicts[static_cast<std::size_t>(evictCount)] = {demandStamp_[slot], slot, resident};
            ++evictCount;
        }
    }
    std::sort(evicts.begin(), evicts.begin() + evictCount,
              [](const EvictCandidate& a, const EvictCandidate& b) {
                  if (a.stamp != b.stamp) return a.stamp < b.stamp; // LRU first
                  return a.slot < b.slot;                           // deterministic tie-break
              });
    for (int i = 0; i < evictCount; ++i) {
        commands.push_back({TileStreamCommand::Action::Evict, evicts[static_cast<std::size_t>(i)].id,
                            evicts[static_cast<std::size_t>(i)].slot});
    }

    // Phases 2+3: keeps, then loads, each nearest-first from the car.
    struct SlotDemand {
        int slot;
        int id;
        std::uint64_t distance;
    };
    std::array<SlotDemand, kFieldTileSlots> order{};
    for (int slot = 0; slot < kFieldTileSlots; ++slot) {
        order[static_cast<std::size_t>(slot)] = {slot, incoming[slot], torusDistanceSquared(p, incoming[slot])};
    }
    std::sort(order.begin(), order.end(), [](const SlotDemand& a, const SlotDemand& b) {
        if (a.distance != b.distance) return a.distance < b.distance; // nearest first
        return a.id < b.id;                                          // deterministic tie-break
    });
    for (int pass = 0; pass < 2; ++pass) {
        const bool keepPass = pass == 0;
        for (const SlotDemand& demand : order) {
            const bool resident = demandResident_[demand.slot] == demand.id;
            if (resident != keepPass) continue;
            commands.push_back({keepPass ? TileStreamCommand::Action::Keep : TileStreamCommand::Action::Load,
                                demand.id, demand.slot});
            // Stamps follow emission order, so a later all-slot eviction
            // replays this call's keep/load recency as its LRU order.
            demandStamp_[demand.slot] = ++demandClock_;
        }
    }

    // After advanceTo the buffers hold exactly the demand window.
    for (int slot = 0; slot < kFieldTileSlots; ++slot) {
        demandResident_[slot] = incoming[slot];
    }
    demandedId_ = id;
    return commands;
}

} // namespace rt::native::gameplay
