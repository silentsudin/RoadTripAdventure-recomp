// Issue #24 "Fix steering bias" pin: UpdateSteeringDamping 0x21a088.
// Retail's descending arm tests closeness as `iVar3 - cmd < 9`, which is already
// true while the command sits above its negative target, so a left command snaps
// to lock in one tick while a right one steps +8. rtguest::gSteerBiasFix mirrors
// the test (`cmd - iVar3 < 9`) so both sides step. Also pins the arg signedness:
// as uint32, `param_5 < 0` was dead and every left command took the positive ramp.
#include "guest/guest_shim.h"

#include <cstdint>
#include <cstdio>

namespace guest {
uint32_t UpdateSteeringDamping(uint32_t param_1, int32_t param_2, int32_t param_3, int32_t param_4,
                               int32_t param_5, ...);
}

namespace {

int failures = 0;
void check(bool ok, const char* what) {
    if (!ok) { std::printf("FAIL: %s\n", what); ++failures; }
}

constexpr uint32_t kCar = 0x00100000;

void reset(int16_t command) {
    *reinterpret_cast<int16_t*>(rtguest::G(kCar + 0x1d2)) = command;  // steering command
    *reinterpret_cast<int16_t*>(rtguest::G(kCar + 0x1d6)) = 0;        // yaw offset (in range)
    *reinterpret_cast<int16_t*>(rtguest::G(kCar + 0x1cc)) = 0;
    *reinterpret_cast<int16_t*>(rtguest::G(kCar + 0x1d4)) = 0;
    *reinterpret_cast<uint32_t*>(rtguest::G(kCar + 0x1b8)) = 1;       // speed nonzero
    *reinterpret_cast<uint32_t*>(rtguest::G(kCar + 0x1bc)) = 0;
    *reinterpret_cast<uint32_t*>(rtguest::G(kCar + 0x1f8)) = 0;
}

int16_t command() { return *reinterpret_cast<int16_t*>(rtguest::G(kCar + 0x1d2)); }

// param_5 = -300 (left), param_3 = 100 -> target (param_5 - param_3)/3 = -133.
// param_4 = 6000 keeps the tail from rewriting +0x1d2 (it does at < 5000).
int16_t oneTick() {
    reset(0);
    guest::UpdateSteeringDamping(kCar, 0, 100, 6000, -300);
    return command();
}

}  // namespace

int main() {
    rtguest::gSteerBiasFix = false;
    check(oneTick() == -133, "retail snaps the left command to its target in one tick");
    rtguest::gSteerBiasFix = true;
    check(oneTick() == -8, "fixed steps the left command by 8 like the right arm");

    rtguest::gSteerBiasFix = false;
    reset(0);
    guest::UpdateSteeringDamping(kCar, 0, 100, 6000, 300);  // right arm, same magnitude
    check(command() == 8, "the right command steps 8 (unchanged by either mode)");

    if (failures) { std::printf("steering_bias: %d failures\n", failures); return 1; }
    std::printf("steering_bias: ok (retail snaps left, fixed ramps, right unchanged)\n");
    return 0;
}
