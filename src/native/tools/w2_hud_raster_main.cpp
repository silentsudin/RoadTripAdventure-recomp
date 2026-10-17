// rt-w2-hud_raster: software rasterization of the hud_live drive-HUD draw list
// (dial + speed digits) into oracle shot space (1280x447, x = 2*uiX, y = uiY) for ROI
// diffs against build/scratch/oracle-golden (see tests/native_w2/hud_oracle_diff.py).
//
//   rt-w2-hud_raster --mph N [--rpm R] [--gear G] [--meter M] [--town] [--clock TICKS]
//                    [--disc DIR] [--out raster.ppm] [--host-speed S --divisor D]
// Renders handler 0 (1P gauge, ITEM/METER0.GSL) via hud_live::buildGaugeHud.
//
// Prints one line: HUD-RASTER mph=<n> cells=<k> fnv=<hash of the full shot raster>.

#include "gameplay/hud_live.h"

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>

namespace hl = rt::native::gameplay::hud_live;

namespace {

bool loadArt(const std::filesystem::path& path, hl::GaugeArt& out) {
    std::ifstream in(path, std::ios::binary);
    if (!in) return false;
    const std::vector<std::uint8_t> bytes((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    return hl::decodeGaugeArt(bytes, out);
}

}  // namespace

int main(int argc, char** argv) {
    std::filesystem::path disc;
    if (const char* home = std::getenv("HOME"))
        disc = std::filesystem::path(home) / "Library/Application Support/RoadTripRecomp/disc";
    std::string outPath;
    int mph = -1, rpm = 0, gear = 0, meter = 0;
    bool town = false;
    int clockTicks = 83250;
    float hostSpeed = -1.f, divisor = hl::kDefaultSpeedDivisor;
    for (int i = 1; i < argc; ++i) {
        const std::string a = argv[i];
        auto next = [&]() -> const char* { return i + 1 < argc ? argv[++i] : ""; };
        if (a == "--disc") disc = next();
        else if (a == "--out") outPath = next();
        else if (a == "--mph") mph = std::atoi(next());
        else if (a == "--rpm") rpm = std::atoi(next());
        else if (a == "--gear") gear = std::atoi(next());
        else if (a == "--meter") meter = std::atoi(next());
        else if (a == "--clock") clockTicks = std::atoi(next());
        else if (a == "--town") town = true;
        else if (a == "--host-speed") hostSpeed = static_cast<float>(std::atof(next()));
        else if (a == "--divisor") divisor = static_cast<float>(std::atof(next()));
        else { std::fprintf(stderr, "unknown arg %s\n", a.c_str()); return 2; }
    }
    if (mph < 0 && hostSpeed < 0.f) {
        std::fprintf(stderr, "usage: rt-w2-hud_raster --mph N | --host-speed S [--divisor D] [--rpm R] [--gear G] "
                             "[--meter M] [--town] [--clock T] [--disc DIR] [--out f.ppm]\n");
        return 2;
    }
    hl::GaugeArt art0;
    if (!loadArt(disc / "ITEM/METER0.GSL", art0)) {
        std::fprintf(stderr, "NO_DISC: cannot decode ITEM/METER0.GSL under %s\n", disc.c_str());
        return 2;
    }
    hl::GaugeInput in;
    in.speedDivisor = 1.f;
    in.speedRaw = mph >= 0 ? static_cast<float>(mph) + 0.25f : hostSpeed;
    if (mph < 0) in.speedDivisor = divisor;
    in.rpm = rpm; in.gear = gear; in.meterField = meter; in.townMode = town; in.clockTicks = clockTicks;
    const auto quads = hl::buildGaugeHud(in);
    const hl::GaugeArt* pages[3] = {nullptr, &art0, nullptr};
    hl::Canvas canvas;
    hl::rasterize(quads, pages, canvas);

    constexpr int kW = hl::kShotWidth, kH = hl::kShotHeight;
    const std::vector<std::uint8_t> shot = hl::toShotSpace(canvas);
    std::uint64_t h = 1469598103934665603ull;
    for (std::uint8_t b : shot) { h ^= b; h *= 1099511628211ull; }
    if (!outPath.empty()) {
        std::ofstream o(outPath, std::ios::binary);
        o << "P6\n" << kW << " " << kH << "\n255\n";
        o.write(reinterpret_cast<const char*>(shot.data()), static_cast<std::streamsize>(shot.size()));
    }
    std::printf("HUD-RASTER mph=%d cells=%zu fnv=%016llx\n", hl::hostMph(in.speedRaw, in.speedDivisor),
                hl::speedCells(hl::hostMph(in.speedRaw, in.speedDivisor), 0x59, 0x11).size(),
                static_cast<unsigned long long>(h));
    return 0;
}
