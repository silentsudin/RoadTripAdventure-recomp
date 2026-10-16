// Static RULES-table accessors. All data lives in rules_tables.h as constexpr
// (provenance comments there); this translation unit holds only the decoded
// accessor behaviour, each annotated with the consumer function it mirrors.

#include "gameplay/rules_tables.h"

namespace rt::native::gameplay {

// Mirrors the ladder reads of FUN_00236238 SumPlayerPoints /
// FUN_00236858 (World-Grand-Prix standings): no payout outside the table.
std::uint32_t racePayout(int tier, int place) {
    if (tier < 0 || tier >= kRacePayoutTiers || place < 0 || place >= kRacePayoutPlaces) {
        return 0;
    }
    return kRacePayoutLadder[tier][place];
}

// FUN_0023a140 (message opcode 0x16): rate = band*10 + 10 G per whole mile.
std::uint32_t mileageExchangeRate(std::uint8_t band) {
    return static_cast<std::uint32_t>(band) * 10u + 10u;
}

// FUN_0023cd58: linear scan of the 25 (type,id) pairs at 0x2A2B70; the
// matching entry number is the progress-flag bit that satisfies the gate.
int findUnlockGate(std::uint8_t type, std::uint8_t id) {
    for (int i = 0; i < kUnlockGateCount; ++i) {
        if (kUnlockGates[i].type == type && kUnlockGates[i].id == id) {
            return i;
        }
    }
    return -1;
}

// FUN_00251b60 (UpdateGolfMiniGame): yardage is only computed when the
// measured distance exceeds the fixed 8.5856 offset, then scaled by the
// exact metre->yard factor 0.9144 and truncated to a whole number.
std::uint32_t golfYardage(float metreDistance) {
    if (metreDistance <= kGolfYardMetreOffset) {
        return 0;
    }
    const float yards = (metreDistance - kGolfYardMetreOffset) / kGolfYardMetreDivisor;
    if (yards <= 0.f) {
        return 0;
    }
    return static_cast<std::uint32_t>(yards);
}

// FUN_00261de0 (payout screen): pay = bet x row multiplier, 0 outside 24 rows.
std::uint32_t slotPayout(std::uint32_t bet, int row) {
    if (row < 0 || row >= kSlotRowCount) {
        return 0;
    }
    return bet * kSlotRowMultiplier[row];
}

// FUN_00249458 CalculateSpeedometerValue: integer mph = raw / 1.609, clamped
// to 186 while driving (the clamp argument is always true for the HUD).
int speedometerMph(float rawSpeed) {
    int mph = static_cast<int>(rawSpeed / kSpeedoMphDivisor);
    if (mph > kSpeedometerCapMph) {
        mph = kSpeedometerCapMph;
    }
    if (mph < 0) {
        mph = 0;
    }
    return mph;
}

// car-physics.md: brake force = curve[ramp-1] * 10000 >> 5, ramp 1..32 after
// the 32-frame brake ramp fills.
std::int32_t brakeForce(int brakeIndex, int rampFrames) {
    if (brakeIndex < 0 || brakeIndex >= kBrakeCount) {
        return 0;
    }
    if (rampFrames < 1 || rampFrames > kBrakeRampFrames) {
        return 0;
    }
    const std::int32_t value = kBrakeSpecs[brakeIndex].curve[rampFrames - 1];
    return (value * 10000) >> 5;
}

// FUN_002180c0 LoadCarPerformanceSpecs: tire grips are copied straight into
// car+0x21C[contact&7] (six low classes are populated).
std::int16_t tireGrip(int tireIndex, int surfaceClass) {
    if (tireIndex < 0 || tireIndex >= kTireCount || surfaceClass < 0 ||
        surfaceClass >= 6) {
        return 0;
    }
    return static_cast<std::int16_t>(kTireSpecs[tireIndex].grip[surfaceClass]);
}

// Scripted day-cycle: FUN_0026ca48 sets the clock from kDayCycleStepClock[step]
// (step 0..9, starts the area lighting task).
std::uint32_t dayCycleStepClock(int step) {
    if (step < 0 || step >= kDayCycleStepCount) {
        return 0;
    }
    return kDayCycleStepClock[step];
}

// FUN_0026d1c0: waits for task tick == *(u16*)(0x2DCD50 + step*2); step 0's
// zero terminates immediately and the machine ends after step 9.
std::uint32_t dayCycleStepTicks(int step) {
    if (step < 0 || step >= kDayCycleStepCount) {
        return 0;
    }
    return kDayCycleStepTicks[step];
}

}  // namespace rt::native::gameplay
