// ROM-derived race HUD state locations.  These are probes/constants only;
// callers still own the native simulation state and its update ordering.
#pragma once

#include <cstdint>

namespace rt::native::gameplay::race_state_probe {

inline constexpr std::uint32_t kCarStride = 0x270u;

// RankCarsByProgress store: 0x0022d8a0 `sb t3,0x247(a2)`.
inline constexpr std::uint32_t kRacePlaceOffset = 0x247u;
inline constexpr std::uint32_t kRacePlaceRankWriteVa = 0x0022d8a0u;

// raceHudTask: 0x0022e100/08 increments, RecordLapSplitTimes: 0x0022d4c0 resets.
inline constexpr std::uint32_t kCurrentLapFramesOffset = 0x260u;
inline constexpr std::uint32_t kLapFramesIncrementLoadVa = 0x0022e100u;
inline constexpr std::uint32_t kLapFramesIncrementStoreVa = 0x0022e108u;
inline constexpr std::uint32_t kLapFramesResetVa = 0x0022d4c0u;
// DrawRecordTableRows shared tail: 0x0022daa4 `lw t0,0x260(s2)`.
inline constexpr std::uint32_t kLapFramesHudReadVa = 0x0022daa4u;

// GetVehicleGear: 0x002495bc `lb v0,0x1ff(a0)`; per-frame controls write at 0x0021a38c.
inline constexpr std::uint32_t kGearOffset = 0x1ffu;
inline constexpr std::uint32_t kGearHudReadVa = 0x002495bcu;
inline constexpr std::uint32_t kGearFrameWriteVa = 0x0021a38cu;

}  // namespace rt::native::gameplay::race_state_probe
