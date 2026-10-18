#pragma once

#include <array>
#include <algorithm>
#include <cstdint>

namespace rt::native::gameplay::gauge_rpm {

// LoadCarPerformanceSpecs @ 0x2180cc loads this ROM table (0x2ed334, stride
// 0x1c) into car+0x21c..+0x228. AdvanceVehicleMotion @ 0x218f94 selects
// car+0x22c + 2*car[0x1ff], so index 1 is first forward gear; index 0 is R.
inline constexpr std::array<std::array<std::int16_t, 7>, 6> kGearRatioTable{{
    {{-95, 116, 162, 227, 318, 446, 0}},
    {{-95, 116, 182, 291, 408, 490, 0}},
    {{-95, 128, 220, 276, 387, 464, 557}},
    {{-95, 128, 260, 327, 458, 550, 660}},
    {{-95, 144, 300, 414, 539, 647, 711}},
    {{-95, 156, 350, 446, 550, 625, 750}},
}};

// AdvanceVehicleMotion @ 0x218f80 materializes this fixed-point scale as 0x280
// before dividing by the selected ratio. Its final car+0x1bc write uses
// (car[0x1d0] * ratio) >> 7 at 0x2193e4..0x2193fc.
inline constexpr std::int32_t kDrivelineFraction = 128;
// AdvanceVehicleMotion @ 0x2193c4 writes 0x2710 after testing car+0x1d0.
inline constexpr std::int32_t kRedlineRpm = 10000;

// The two gear selectors at 0x21ce48 and 0x21cf80 compare the same Q7
// driveline conversion against these literal thresholds. Automatic downshift
// also requires the next-lower gear's projected value below the second limit.
inline constexpr std::array<std::int32_t, 2> kAutoUpshiftRpm{{8201, 5001}};
inline constexpr std::array<std::int32_t, 2> kAutoDownshiftCurrentRpm{{5000, 3000}};
inline constexpr std::array<std::int32_t, 2> kAutoDownshiftProjectedRpm{{8000, 4000}};

// The ROM's gear-coupled mechanical conversion, expressed in the useful
// inverse direction for a native physics state. `longitudinalMotion` is the
// separate longitudinal/coupling state, not the HUD speed word (MPH is a
// different display conversion). The live EE routine is an integrator around
// this coupling; use its previous RPM/traction/throttle dynamics before this
// final clamp if frame-exact transient behaviour is required.
inline constexpr std::int32_t rpmFromMotionState(std::int64_t longitudinalMotion,
                                                  std::int16_t gearRatio) {
    if (gearRatio <= 0 || longitudinalMotion <= 0) return 0;
    const auto rpm = (longitudinalMotion * kDrivelineFraction) / gearRatio;
    return static_cast<std::int32_t>(std::clamp<std::int64_t>(rpm, 0, kRedlineRpm));
}

}  // namespace rt::native::gameplay::gauge_rpm
