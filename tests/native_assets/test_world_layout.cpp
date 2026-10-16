// Authentic world-geography unit pins (docs/native-reference/world-geography.md).
// Disc-free unless noted; the LBA join uses only the user's own extract.
#include "../../src/native/gameplay/world_layout.h"

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <string>
#include <unordered_map>

using namespace rt::native::gameplay;

static int g_fail = 0;
#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #c); ++g_fail; } } while (0)

namespace {

const char* fldName(int id) {
    static char buf[8];
    const int A = (id >> 4) & 3;
    const int col = (id >> 1) & 7;
    const int b = id & 1;
    const int B = col >> 1;
    const int C = (b << 1) | (col & 1);
    std::snprintf(buf, sizeof(buf), "%d%d%d.BIN", A, B, C);
    return buf;
}

void testAll64RoundTripThroughFormula() {
    for (int id = 0; id < 64; ++id) {
        const TilePlacement t = tilePlacement(id);
        const std::int32_t cx = t.x0 + static_cast<std::int32_t>(800.0 * kFixedUnitsPerGameUnit);
        const std::int32_t cz = t.z0 + static_cast<std::int32_t>(800.0 * kFixedUnitsPerGameUnit);
        CHECK(gridCellId({cx, cz}) == id);
    }
}

void testPeachTownPlacementExact() {
    const TilePlacement peach = tilePlacement(11);
    CHECK(peach.x0 == 0x0b000000);
    CHECK(peach.z0 == 0x02000000);
    CHECK(peach.row == 1);
    CHECK(peach.halfColumn == 11);
    // id 11 is the scripted Peach tile -> FLD/023.BIN (asset-map.md join).
    CHECK(std::string(fldName(11)) == "023.BIN");
    CHECK(std::string(fldName(43)) == "223.BIN");
    CHECK(std::string(fldName(3)) == "003.BIN");   // swap2(3) == 3
    CHECK(std::string(fldName(2)) == "001.BIN");    // swap2(1) == 2
}

void testTileLocalCentre() {
    const TilePlacement t = tilePlacement(11);
    const TileLocal l = tileLocalOf({t.x0 + 0x01000000, t.z0 + 0x01000000});
    CHECK(l.id == 11);
    CHECK(std::fabs(l.localX - 800.0) < 0.001);
    CHECK(std::fabs(l.localZ - 800.0) < 0.001);
}

void testOddRowShear() {
    // Z bit 25 set -> X adds one half-column (0x01000000 = 800 game units)
    // before masking (FUN_0021ba70).
    TileLocal odd = tileLocalOf({0x00000000, 0x02000000});
    CHECK(std::fabs(odd.localX - 800.0) < 0.001);
    TileLocal even = tileLocalOf({0x00000000, 0x00000000});
    CHECK(even.localX == 0.0);
}

void testTorusWrapIsModuloSixteenMeg() {
    FixedPos w = wrapToras({0x12345678, static_cast<std::int32_t>(0x9abcdef0)});
    CHECK(w.x == 0x02345678);
    CHECK(w.z == 0x0abcdef0);
}

void testDiscLbaJoinForAllIds() {
    const char* env = std::getenv("RT_DISC");
    std::string disc = env ? env : (std::string(std::getenv("HOME") ? std::getenv("HOME") : "") +
                                    "/Library/Application Support/RoadTripRecomp/disc");
    std::ifstream map(disc + "/.lbn_map.tsv");
    std::ifstream elf(disc + "/SLUS_203.98", std::ios::binary);
    if (!map || !elf) { std::printf("skip disc join (no extract at %s)\n", disc.c_str()); return; }
    std::unordered_map<std::int64_t, std::string> byLba;
    std::string line;
    while (std::getline(map, line)) {
        auto t1 = line.find('\t');
        if (t1 == std::string::npos) continue;
        const std::int64_t lba = std::atoll(line.substr(t1 + 1).c_str());
        byLba[lba] = line.substr(0, t1);
    }
    // FLD descriptor table vaddr 0x29bb50, 12-byte {tag, sectorOffset, ...};
    // ELF PT_LOAD delta 0x1ff000. Derive the ISO data base from id 0
    // (000.BIN) and verify every other id resolves to its mapped filename.
    elf.seekg(0x29bb50 - 0x1ff000);
    std::int32_t desc[64 * 3];
    elf.read(reinterpret_cast<char*>(desc), sizeof(desc));
    if (!elf) { std::printf("skip disc join (ELF too small)\n"); return; }
    std::int64_t base = -1;
    for (const auto& kv : byLba) {
        if (kv.second == "FLD/000.BIN") { base = kv.first - desc[1]; break; }
    }
    CHECK(base >= 0);
    int matched = 0;
    if (base >= 0) {
        for (int id = 0; id < 64; ++id) {
            CHECK((desc[id * 3] & 0xffffff) == 0x333388);
            auto it = byLba.find(base + desc[id * 3 + 1]);
            if (it != byLba.end() && it->second == "FLD/" + std::string(fldName(id))) ++matched;
        }
    }
    CHECK(matched == 64);
}

} // namespace

int main() {
    testAll64RoundTripThroughFormula();
    testPeachTownPlacementExact();
    testTileLocalCentre();
    testOddRowShear();
    testTorusWrapIsModuloSixteenMeg();
    testDiscLbaJoinForAllIds();
    if (g_fail) { std::fprintf(stderr, "%d checks FAILED\n", g_fail); return 1; }
    std::printf("all world_layout tests passed\n");
    return 0;
}
