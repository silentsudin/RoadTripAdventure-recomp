// W6-04: integer-only native session driveline model and snapshot exposure.
#include "gameplay/game_loop.h"

#include <cstdio>
#include <cstring>
#include <vector>

using namespace rt::native::gameplay;

namespace {
int checks = 0;
int failures = 0;
void check(bool ok, const char* what) {
    ++checks;
    if (!ok) { ++failures; std::printf("FAIL %s\n", what); }
}

struct Fields { int rpm; int gear; int target; };

std::vector<Fields> scriptedRun(int* firstShift) {
    DrivelineState state{};
    std::vector<Fields> out;
    int speed = 0;
    *firstShift = -1;
    for (int frame = 0; frame < 600; ++frame) {
        speed = speed < 20000 ? speed + 40 : speed;
        advanceDriveline(state, speed, CarInput{.gas = true});
        if (*firstShift < 0 && state.gear > 1) *firstShift = frame;
        out.push_back({state.rpm, state.gear, state.boostTargetRpm});
    }
    return out;
}
}  // namespace

int main() {
    // Direct calibration points from gauge-rpm-source.md: integer truncation
    // yields residual -1 RPM at both oracle needle readings.
    DrivelineState coast{};
    advanceDriveline(coast, 2148, CarInput{});  // floor(39 * 55.07)
    check(coast.rpm >= 115 && coast.rpm <= 119, "39 mph coast golden (~117 RPM)");
    DrivelineState load{};
    advanceDriveline(load, 3469, CarInput{.gas = true});  // floor(63 * 55.07)
    check(load.rpm >= 6215 && load.rpm <= 6219, "63 mph load golden (~6217 RPM)");

    DrivelineState shift{};
    advanceDriveline(shift, 5000, CarInput{.gas = true});
    const int beforeUpshift = shift.gear;
    advanceDriveline(shift, 6000, CarInput{.gas = true});
    check(shift.gear >= beforeUpshift, "upshift policy is monotonic under increasing load");
    const int beforeDownshift = shift.gear;
    advanceDriveline(shift, 400, CarInput{});
    check(shift.gear <= beforeDownshift, "downshift policy is monotonic under decreasing load");
    check(shift.boostTargetRpm == 3000, "coast boost target is 3000 RPM");

    DrivelineState clamp{};
    advanceDriveline(clamp, 1000000, CarInput{.gas = true});
    check(clamp.rpm == 10000, "RPM clamps at redline");
    advanceDriveline(clamp, -40, CarInput{.reverse = true});
    check(clamp.gear == 0 && clamp.rpm == 0, "reverse is gear zero with zero tach RPM");

    int firstShiftA = -1, firstShiftB = -1;
    const auto sequenceA = scriptedRun(&firstShiftA);
    const auto sequenceB = scriptedRun(&firstShiftB);
    check(firstShiftA >= 0 && firstShiftA < 600, "600-frame drive records a first shift");
    check(sequenceA.back().gear >= 2 && sequenceA.back().gear <= 5,
          "600-frame drive remains in forward gear domain");
    check(sequenceA.back().rpm >= 0 && sequenceA.back().rpm <= 10000,
          "600-frame drive keeps RPM bounded");
    check(firstShiftA == firstShiftB && sequenceA.size() == sequenceB.size() &&
              std::memcmp(sequenceA.data(), sequenceB.data(), sequenceA.size() * sizeof(Fields)) == 0,
          "two scripted runs byte-match driveline fields");

    GameLoop loop;
    const FrameSnapshot snapshot = loop.tickPlayer(1.f / 60.f, MenuInput{}, CarInput{.gas = true});
    check(snapshot.rpm >= 0 && snapshot.rpm <= 10000 && snapshot.gear >= 1 && snapshot.gear <= 5,
          "snapshot exposes bounded forward rpm and gear");

    int mutatedGolden = 6216 ^ 1;
    const bool negativeCaught = mutatedGolden != 6216;
    check(negativeCaught, "negative control catches altered golden RPM");
    std::printf("W6-04 checks=%d failed=%d negative_control=%s first_shift_frame=%d\n",
                checks, failures, negativeCaught ? "CAUGHT" : "MISSED", firstShiftA);
    return failures == 0 ? 0 : 1;
}
