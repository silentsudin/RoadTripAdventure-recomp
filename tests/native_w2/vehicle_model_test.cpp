#include "gameplay/vehicle_model.h"

#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace {

constexpr std::uint32_t kElfBase = 0x1ff000;

std::uint64_t fnv1a(const std::vector<std::uint8_t>& bytes) {
    std::uint64_t h = 0xcbf29ce484222325ULL;
    for (const std::uint8_t b : bytes) h = (h ^ b) * 0x100000001b3ULL;
    return h;
}

bool check(bool condition) { return condition; }

std::vector<std::uint8_t> elfBytes(const std::filesystem::path& path) {
    std::ifstream in(path, std::ios::binary);
    return {std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>()};
}

std::uint32_t le32(const std::vector<std::uint8_t>& data, std::uint32_t vaddr) {
    const std::size_t off = static_cast<std::size_t>(vaddr - kElfBase);
    return std::uint32_t(data[off]) | (std::uint32_t(data[off + 1]) << 8) |
           (std::uint32_t(data[off + 2]) << 16) | (std::uint32_t(data[off + 3]) << 24);
}

std::uint64_t rangeHash(const std::vector<std::uint8_t>& data, std::uint32_t begin, std::uint32_t end) {
    const auto first = data.begin() + static_cast<std::ptrdiff_t>(begin - kElfBase);
    const auto last = data.begin() + static_cast<std::ptrdiff_t>(end - kElfBase);
    return fnv1a({first, last});
}

std::uint64_t replayHash() {
    using namespace rt::native::gameplay;
    VehicleKinematics replay{};
    replay.velocity = {15382, 0, 0}; // trunc((15382/32768*216)/1.609) == 63.
    std::vector<std::uint8_t> bytes;
    bytes.reserve(731 * 16);
    for (int i = 0; i < 731; ++i) {
        tickKinematics(replay);
        for (const std::int32_t word : {replay.position.x, replay.position.y,
                                        replay.position.z, replay.velocity.y}) {
            for (int shift = 0; shift < 32; shift += 8)
                bytes.push_back(static_cast<std::uint8_t>(word >> shift));
        }
    }
    return fnv1a(bytes);
}

struct TraceRow {
    double x = 0, y = 0, z = 0;
    rt::native::gameplay::F15Vector velocity{};
    int sampledDialMph = -1;
};

std::vector<TraceRow> loadTrace() {
    const std::filesystem::path path = std::filesystem::path(__FILE__).parent_path() / "data/vehicle_trace.csv";
    std::ifstream in(path);
    std::string line;
    std::getline(in, line); // header
    std::vector<TraceRow> rows;
    while (std::getline(in, line)) {
        std::stringstream fields(line);
        std::string value;
        std::vector<std::string> column;
        while (std::getline(fields, value, ',')) column.push_back(value);
        if (column.size() < 9) return {};
        rows.push_back({std::stod(column[2]), std::stod(column[3]), std::stod(column[4]),
                        {std::stoi(column[5]), std::stoi(column[6]), std::stoi(column[7])},
                        (column[8].empty() || column[8] == "\r") ? -1 : std::stoi(column[8])});
    }
    return rows;
}

}  // namespace

int main(int argc, char** argv) {
    std::filesystem::path elf = std::filesystem::path(std::getenv("HOME") ? std::getenv("HOME") : "") /
        "Library/Application Support/RoadTripRecomp/disc/SLUS_203.98";
    for (int i = 1; i + 1 < argc; ++i) if (std::string(argv[i]) == "--elf") elf = argv[++i];
    if (!std::filesystem::exists(elf)) {
        std::cerr << "NO_ELF\n";
        return 2;
    }

    const auto image = elfBytes(elf);
    bool ok = image.size() > 0x133f48;
    // Pinned from the read-only ELF; vaddr - 0x1ff000 is the file offset.
    ok &= check(le32(image, 0x00332f44) == 0x3fcdf3b6U);
    ok &= check(rangeHash(image, 0x0021d358, 0x0021d387) == 0x078c393c7af6d8d1ULL);
    ok &= check(rangeHash(image, 0x00249458, 0x002494e7) == 0x0c0a7123de6d7f6aULL);
    ok &= check(rangeHash(image, 0x0021ba70, 0x0021c69b) == 0xc92f40e0033aa59aULL);

    using namespace rt::native::gameplay;
    const F15Vector v{0, 0, 0};
    ok &= check(rawSpeedFromF15(v) == 0.0);
    ok &= check(speedometerMph(F15Vector{32768, 0, 0}) == 134);
    ok &= check(speedometerMph(F15Vector{100000, 0, 0}) == 186);

    VehicleKinematics a{};
    a.position = {0, 0, 0};
    a.velocity = {2500, -89, -5000};
    a.airborne = true;
    VehicleKinematics b = a;
    tickKinematics(a);
    tickKinematics(b);
    ok &= check(a.position.x == 1600 && a.position.y == -113 && a.position.z == -3200);
    ok &= check(a.velocity.y == -178 && a.position == b.position && a.velocity == b.velocity);

    SteeringState steering{};
    steerTick(steering, 100, 3000, 10000); // target=(100+3000)/3, capped to 512.
    ok &= check(steering.command == 8 && steering.yaw == 8);
    for (int i = 0; i < 80; ++i) steerTick(steering, 100, 3000, 10000);
    // The yaw integration leaves the +/-0xfff command-update window after 32 ticks.
    ok &= check(steering.command == 256);

    const auto trace = loadTrace();
    ok &= check(trace.size() == 731);
    if (trace.size() == 731) {
        int exactMphCells = 0;
        int sampledDialCells = 0;
        int exactSampledDialCells = 0;
        for (const TraceRow& row : trace) {
            const int independentlyRounded = static_cast<int>(rawSpeedFromF15(row.velocity) / kMphDivisor);
            exactMphCells += speedometerMph(row.velocity, false) == independentlyRounded;
            if (row.sampledDialMph >= 0) {
                ++sampledDialCells;
                exactSampledDialCells += speedometerMph(row.velocity) == row.sampledDialMph;
            }
        }
        const double chord = std::hypot(trace.back().x - trace.front().x, trace.back().z - trace.front().z);
        ok &= check(exactMphCells == static_cast<int>(trace.size()));
        ok &= check(sampledDialCells == 24 && exactSampledDialCells == sampledDialCells);
        ok &= check(speedometerMph(trace.back().velocity) >= 62 && speedometerMph(trace.back().velocity) <= 64);
        ok &= check(std::abs(chord - 158.4568) / 158.4568 <= 0.02);
    }

    const F15Vector raceEndVelocity{15382, 0, 0}; // isolated scalar vector yields the documented 63 mph.
    const auto firstReplay = replayHash();
    const auto secondReplay = replayHash();
    ok &= check(speedometerMph(raceEndVelocity) == 63 && firstReplay == secondReplay);

    std::cout << "SCORE: W2-03=" << (ok ? "PASS" : "FAIL") << ' ' << (ok ? "PASS" : "FAIL") << '\n';
    return ok ? 0 : 1;
}
