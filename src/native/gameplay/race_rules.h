// W2-05: pure race scoring rules grounded in SLUS-203.98.
#pragma once
#include "gameplay/save.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <span>
#include <vector>

namespace rt::native::gameplay::race_rules {
inline constexpr std::size_t kPayoutTiers = 5, kPayoutPlaces = 6;
inline constexpr std::size_t kRaceRecords = 26, kLicenceRecords = 24;
inline constexpr std::size_t kPlayerBands = 3, kBandEntries = 26;
inline constexpr std::uint8_t kNotRaced = 0xff;
struct RaceRecord { std::uint8_t course, tier; };
struct LapSplit { std::uint32_t elapsed = 0; std::array<std::uint32_t, 3> splits{}; std::uint8_t count = 0; bool newBest = false; };
struct LapState { std::uint32_t lapTimer = 0; std::uint8_t lap = 0; std::array<std::uint32_t, 3> recent{}; std::uint32_t best = kBestTimeNone; };
struct Standing { std::uint8_t driver = 0; std::uint16_t points = 0; bool operator==(const Standing&) const = default; };

extern const std::array<std::array<std::uint32_t, kPayoutPlaces>, kPayoutTiers> kPayoutLadder;
extern const std::array<RaceRecord, kRaceRecords> kRecords;

[[nodiscard]] int recordSlotForRaceId(int raceId); // guest remap: 26/27 -> 24/25
[[nodiscard]] std::uint32_t payout(std::uint8_t tier, std::uint8_t zeroBasedPlace);
[[nodiscard]] LapSplit recordLap(LapState& state); // RecordLapSplitTimes 0x22D390 core bookkeeping
[[nodiscard]] bool eligibleForLicence(std::uint8_t currentLicence, std::span<const std::uint8_t> resultSlots);
[[nodiscard]] bool awardLicenceIfEligible(SaveData& save, std::span<const std::uint8_t> resultSlots);
void writeResultAndTimes(SaveData& save, std::uint8_t playerBand, int raceId, std::uint8_t place,
                         std::uint32_t lapFrames, std::uint32_t totalFrames);
[[nodiscard]] std::array<Standing, 8> rankWgp(const std::array<std::array<std::uint8_t, 8>, 7>& rounds,
                                               std::size_t completedRounds);
[[nodiscard]] std::uint32_t wgpPurse(std::uint8_t tier, const std::array<Standing, 8>& standings,
                                      std::uint8_t playerDriver = 0);
} // namespace rt::native::gameplay::race_rules
