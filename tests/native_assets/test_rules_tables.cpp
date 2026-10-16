// Synthetic contract test for the authored RULES tables (rules_tables.{h,cpp}).
// Pins table geometry, sentinel values and accessor behaviour — no game data,
// no ELF access. Plain counters, no assert(); prints "<n> checks passed" and
// exits non-zero on any failure.
#include "../../src/native/gameplay/rules_tables.h"

#include <cstdio>
#include <cstdint>

using namespace rt::native::gameplay;

static int g_checks = 0;
static int g_fail = 0;
#define CHECK(c)                                                              \
    do {                                                                      \
        ++g_checks;                                                            \
        if (!(c)) {                                                            \
            std::fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #c);  \
            ++g_fail;                                                          \
        }                                                                     \
    } while (0)

static void testMoneyPrimitives() {
    // Seed 1000 @0x654 and cap 999999 (FUN_0023dc58 clamp), both CONFIRMED.
    CHECK(kMoneySeedNewGame == 1000);
    CHECK(kMoneyCap == 999999);
    CHECK(kMoneySaveOffset == 0x654);
    CHECK(kMoneySpendSfxId == 0x305);
    CHECK(kBankBudgetInit == 100000);  // FUN_00227c68 init of 0x1782a6c
    CHECK(kBankStepperStart == 1000);  // FUN_0023a460 digit stepper init
    CHECK(kShopFlatPriceCategory0 == 500);  // GetConst500 0x00243698
    CHECK(kSellPriceDivisor == 2);           // refund halves the price
}

static void testRacePayoutLadder() {
    // Geometry: 5 tiers x 6 places, 4-byte entries (native u32).
    CHECK(sizeof(kRacePayoutLadder) == 5u * 6u * sizeof(std::uint32_t));
    CHECK(kRacePayoutTiers == 5);
    CHECK(kRacePayoutPlaces == 6);

    // First/last cells of the CONFIRMED ladder bytes @0x29FCB8.
    CHECK(kRacePayoutLadder[0][0] == 800);
    CHECK(kRacePayoutLadder[0][5] == 100);
    CHECK(kRacePayoutLadder[1][0] == 1500);
    CHECK(kRacePayoutLadder[2][5] == 800);
    CHECK(kRacePayoutLadder[3][0] == 80000);
    CHECK(kRacePayoutLadder[3][5] == 10000);
    CHECK(kRacePayoutLadder[4][0] == 10000);
    CHECK(kRacePayoutLadder[4][5] == 1000);

    // Ladder is strictly non-increasing per tier (payout by place).
    for (int t = 0; t < kRacePayoutTiers; ++t) {
        for (int p = 1; p < kRacePayoutPlaces; ++p) {
            CHECK(kRacePayoutLadder[t][p] <= kRacePayoutLadder[t][p - 1]);
        }
    }

    // Accessor behaviour.
    CHECK(racePayout(0, 0) == 800);
    CHECK(racePayout(2, 5) == 800);
    CHECK(racePayout(4, 0) == 10000);
    CHECK(racePayout(3, 5) == 10000);
    CHECK(racePayout(5, 0) == 0);   // tier out of range
    CHECK(racePayout(-1, 0) == 0);
    CHECK(racePayout(0, 6) == 0);   // 7th place pays nothing
    CHECK(racePayout(0, -1) == 0);

    // Licence records: 24 races, stored place-1 must be <= 5.
    CHECK(kLicenceRaceCount == 24);
    CHECK(kLicencePlaceStoredMax == 5);
    CHECK(kRacePlaceUnraced == 0xFF);
    CHECK(kBestTimeNone == 0x7FFFFFFF);

    // Licence enum order (save 0x651) and result band geometry.
    CHECK(kLicenceNone == 0 && kLicenceB == 1 && kLicenceA == 2 &&
          kLicenceSuperA == 3);
    CHECK(kRaceResultSlotsPerPlayer == 26);      // FUN_00237c48
    CHECK(kRaceResultPlayerBands == 3);         // 26*3 = 78 u32 rows
    CHECK(kRaceResultSlotsPerPlayer * kRaceResultPlayerBands * 4 == 78 * 4);
}

static void testMileageExchange() {
    // FUN_0023a140: metres->miles divisor 1609 (0x649), counters at 0x65C.
    CHECK(kMetresPerMile == 1609);
    CHECK(kMileageExchangeSaveOffset == 0x65C);
    CHECK(kMileageExchangeBands == 10);
    // rate ladder band*10+10.
    CHECK(mileageExchangeRate(0) == 10);
    CHECK(mileageExchangeRate(1) == 20);
    CHECK(mileageExchangeRate(2) == 30);
    CHECK(mileageExchangeRate(9) == 100);
}

static void testUnlockGates() {
    // Geometry: 25 entries of 2 bytes (stride of the original table 2).
    CHECK(kUnlockGateCount == 25);
    CHECK(sizeof(kUnlockGates) == 25u * 2u);

    // Sentinel rows from the CONFIRMED byte dump @0x2A2B70.
    CHECK(kUnlockGates[0].type == 1 && kUnlockGates[0].id == 29);
    CHECK(kUnlockGates[3].type == 7 && kUnlockGates[3].id == 33);
    CHECK(kUnlockGates[15].type == 1 && kUnlockGates[15].id == 32);
    CHECK(kUnlockGates[24].type == 6 && kUnlockGates[24].id == 36);

    // Every type in the observed domain and every id within 25..39.
    for (int i = 0; i < kUnlockGateCount; ++i) {
        CHECK(kUnlockGates[i].type <= 7);
        CHECK(kUnlockGates[i].id == 0 || (kUnlockGates[i].id >= 25 &&
                                          kUnlockGates[i].id <= 39));
    }

    // lookup returns the table index (the progress-flag bit) or -1.
    CHECK(findUnlockGate(1, 29) == 0);
    CHECK(findUnlockGate(7, 36) == 20);
    CHECK(findUnlockGate(6, 34) == 1);
    CHECK(findUnlockGate(9, 99) == -1);
    CHECK(findUnlockGate(0, 0) == 4);  // zero filler rows still match

    // Clock thresholds from FUN_0023cd58 special cases (CONFIRMED literals).
    CHECK(kUnlockClockAfterSunset == 153001);  // 17:00 sunset band
    CHECK(kUnlockClockAfterDusk == 161998);     // 18:00 dusk end
    CHECK(kUnlockClockLate == 206233);          // 22:55
    CHECK(kClockTicksPerHour > 0);
}

static void testCollectibleGates() {
    CHECK(kPhotoCount == 100);        // 64+36 sentinel bits in FUN_0023c5c0
    CHECK(kPhotoStampAllAward == 0x60);
    CHECK(kStampCount == 100);        // save-layout 0x518 bitmap
    CHECK(kProgressStampAllAward == 0x33);  // FUN_0023cb68
    CHECK(kMailboxCapacity == 128);   // save-layout 0x628
}

static void testGolfScoring() {
    CHECK(kGolfHoles == 9);
    // Par list read from the course records @0x2AC0C0 (+0x22 s16).
    CHECK(kGolfPar[0] == 3);
    CHECK(kGolfPar[2] == 5);
    CHECK(kGolfPar[7] == 4);
    CHECK(kGolfPar[8] == 5);
    // Course par total 36.
    int total = 0;
    for (int h = 0; h < kGolfHoles; ++h) {
        total += kGolfPar[h];
    }
    CHECK(total == 36);

    // Yardage constants: offset 8.5856, exact m->yard factor 0.9144.
    CHECK(kGolfYardMetreOffset == 8.585599899291992f);
    CHECK(kGolfYardMetreDivisor == 0.9143999814987183f);
    CHECK(golfYardage(8.5856f) == 0);          // no yard before the offset
    CHECK(golfYardage(0.0f) == 0);
    CHECK(golfYardage(8.5856f + 0.9144f) == 1);
    CHECK(golfYardage(1000.0f) == 1084);       // (1000-8.5856)/0.9144 = 1084.27
    CHECK(golfYardage(100.0f) == 99);          // (100-8.5856)/0.9144 = 99.97
}

static void testSlotRules() {
    CHECK(kSlotRowCount == 24);
    CHECK(sizeof(kSlotRowMultiplier) == 24u * sizeof(std::uint32_t));
    // Row multipliers @0x2D9980 (+0x30 stride), first word per record.
    CHECK(kSlotRowMultiplier[0] == 18);
    CHECK(kSlotRowMultiplier[17] == 18);
    CHECK(kSlotRowMultiplier[18] == 3);
    CHECK(kSlotRowMultiplier[20] == 3);
    CHECK(kSlotRowMultiplier[21] == 2);
    CHECK(kSlotRowMultiplier[22] == 2);
    CHECK(kSlotRowMultiplier[23] == 12);
    CHECK(kSlotBetMax == 10000);
    CHECK(kSlotBetStart == 1000);
    // Payout = bet x multiplier; out-of-range pays nothing.
    CHECK(slotPayout(100, 0) == 1800);
    CHECK(slotPayout(100, 18) == 300);
    CHECK(slotPayout(100, 23) == 1200);
    CHECK(slotPayout(10000, 0) == 180000);  // maximal bet x 18
    CHECK(slotPayout(100, 24) == 0);
    CHECK(slotPayout(100, -1) == 0);
}

static void testDrivingPhysics() {
    // Speed pipeline: f15 length x 216 = raw (km/h units), /1.609 = mph,
    // clamped 186 on the HUD (0x00249458, CONFIRMED).
    CHECK(kSpeedRawToKmh == 216.0f);
    CHECK(kSpeedoMphDivisor == 1.6089999675750732f);
    CHECK(kSpeedometerCapMph == 186);
    CHECK(speedometerMph(216.0f) == 134);     // 1 m/frame = 216 km/h raw
    CHECK(speedometerMph(216.0f * 1.6089999675750732f) == 186);
    CHECK(speedometerMph(1.0e9f) == 186);      // clamped
    CHECK(speedometerMph(0.0f) == 0);

    // Per-frame constants found by whole-ELF instruction search.
    CHECK(kGravityFallPerFrame == -0x59);
    CHECK(kWaterForcePerFrame == 0x59);
    CHECK(kNitroCap == 0xCF69);

    // Tire table: 13 records with the Devil row maxed (0xFF00 grips).
    CHECK(kTireCount == 13);
    CHECK(sizeof(kTireSpecs) == 13u * sizeof(TireSpec));
    CHECK(kTireSpecs[0].name != nullptr);
    CHECK(kTireSpecs[0].price == 200);
    CHECK(kTireSpecs[12].price == 200000);
    CHECK(kTireSpecs[11].weightBonus == 1);
    CHECK(kTireSpecs[12].grip[0] == 65280);
    CHECK(kTireSpecs[3].grip[0] == 512);
    CHECK(tireGrip(0, 0) == 196);
    CHECK(tireGrip(3, 2) == 102);
    CHECK(tireGrip(13, 0) == 0);   // out of range
    CHECK(tireGrip(0, 6) == 0);
    CHECK(tireGrip(0, -1) == 0);

    // Engine profiles: 6 x 7 params, ladder prices 200..10000.
    CHECK(kEngineCount == 6);
    CHECK(kEngineSpecs[0].price == 200);
    CHECK(kEngineSpecs[5].price == 10000);
    CHECK(kEngineSpecs[0].params[0] == 65441);
    CHECK(kEngineSpecs[5].params[6] == 750);

    // Gearboxes: 8 records, gear counts 3..8, price ladder 200..12000.
    CHECK(kGearboxCount == 8);
    CHECK(kGearboxSpecs[0].gears == 3);
    CHECK(kGearboxSpecs[5].gears == 8);
    CHECK(kGearboxSpecs[7].price == 12000);
    CHECK(kGearboxSpecs[0].speedClass == 1500);
    CHECK(kGearboxSpecs[7].speedClass == 3900);

    // Chassis (weight stat -> car+0x218) and steering (mult -> car+0x242).
    CHECK(kChassisCount == 5);
    CHECK(kChassisSpecs[0].weightStat == 25);
    CHECK(kChassisSpecs[4].weightStat == 15);
    CHECK(kChassisSpecs[4].price == 4000);
    CHECK(kSteeringCount == 4);
    CHECK(kSteeringSpecs[0].multiplier == 64);
    CHECK(kSteeringSpecs[3].multiplier == 160);
    CHECK(kSteeringSpecs[3].price == 2000);

    // Brake curves: 4 records x 32 ramp values; force = curve[ramp-1]*10000>>5.
    CHECK(kBrakeCount == 4);
    CHECK(kBrakeRampFrames == 32);
    CHECK(sizeof(kBrakeSpecs) == 4u * sizeof(BrakeSpec));
    CHECK(kBrakeSpecs[0].price == 1000);
    CHECK(kBrakeSpecs[1].price == 1500);
    CHECK(kBrakeSpecs[2].price == 2000);
    CHECK(kBrakeSpecs[0].curve[0] == 1);
    CHECK(kBrakeSpecs[0].curve[31] == 32);
    CHECK(kBrakeSpecs[1].curve[20] == 28);
    CHECK(brakeForce(0, 1) == (1 * 10000) >> 5);      // 312
    CHECK(brakeForce(0, 32) == (32 * 10000) >> 5);    // 10000
    CHECK(brakeForce(1, 20) == (28 * 10000) >> 5);    // 8750
    CHECK(brakeForce(0, 0) == 0);    // ramp starts at 1
    CHECK(brakeForce(0, 33) == 0);
    CHECK(brakeForce(4, 1) == 0);    // out of range
}

static void testDayNightCycle() {
    // Free clock: 216000 vblank ticks per day -> 9000 per hour (PROVEN wrap).
    CHECK(kClockTicksPerDay == 216000);
    CHECK(kClockTicksPerHour == 216000 / 24);

    // Scripted steps: 10 start clocks + 10 durations.
    CHECK(kDayCycleStepCount == 10);
    CHECK(sizeof(kDayCycleStepClock) == 10u * sizeof(std::uint32_t));
    CHECK(sizeof(kDayCycleStepTicks) == 10u * sizeof(std::uint16_t));

    // Confirmed step-clock bytes @0x2DCD28 and their hour conversion.
    CHECK(kDayCycleStepClock[0] == 72000);
    CHECK(kDayCycleStepClock[1] == 81000);
    CHECK(kDayCycleStepClock[2] == 108000);
    CHECK(kDayCycleStepClock[9] == 189000);
    for (int s = 0; s < kDayCycleStepCount; ++s) {
        CHECK(dayCycleStepClock(s) < kClockTicksPerDay);
    }
    CHECK(dayCycleStepClock(0) / kClockTicksPerHour == 8);     // 08:00
    CHECK(dayCycleStepClock(2) / kClockTicksPerHour == 12);    // 12:00 noon
    CHECK(dayCycleStepClock(9) / kClockTicksPerHour == 21);    // 21:00

    // Confirmed step-duration bytes @0x2DCD50 (u16): step 1 lasts 1260
    // ticks (21 s at 60 Hz); fade spawns 64 ticks early.
    CHECK(kDayCycleStepTicks[0] == 0);
    CHECK(kDayCycleStepTicks[1] == 1260);
    CHECK(kDayCycleStepTicks[9] == 12033);
    CHECK(kDayCycleFadeTicks == 64);
    CHECK(dayCycleStepTicks(3) == 3780);
    CHECK(dayCycleStepTicks(10) == 0);   // out of range
    CHECK(dayCycleStepClock(-1) == 0);
}

int main() {
    testMoneyPrimitives();
    testRacePayoutLadder();
    testMileageExchange();
    testUnlockGates();
    testCollectibleGates();
    testGolfScoring();
    testSlotRules();
    testDrivingPhysics();
    testDayNightCycle();

    if (g_fail == 0) {
        std::printf("%d checks passed\n", g_checks);
        return 0;
    }
    std::printf("%d of %d checks FAILED\n", g_fail, g_checks);
    return 1;
}
