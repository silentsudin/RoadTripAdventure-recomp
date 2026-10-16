#pragma once

// Static RULES tables for Road Trip (SLUS-20398), authored from the shipped ELF.
// Every constant below cites its ELF virtual address and a confidence mark:
//   CONFIRMED — read from the ELF bytes AND cross-checked against the
//               instruction stream of a consumer function (decompile/
//               xref), or against an already-PROVEN anchor in
//               docs/native-reference/*.
//   INFERRED  — the bytes are read exactly, but the human-facing semantic is
//               pattern-matched from the consumer code and not proven end to
//               end.
// Table provenance ledger and verification commands:
// docs/native-reference/rules-tables.md. This file contains no game bytes,
// only decoded constants.

#include <cstddef>
#include <cstdint>

namespace rt::native::gameplay {

// ===========================================================================
// 1. Purse / race economy
// ===========================================================================

// New-game purse seed -- CONFIRMED: `li v1,0x3e8` at 0x00229ffc stores 1000 to
// progress+0x654 (`sw v1,0x654(v0)` at 0x0022a004, v0 = 0x0177F760 base);
// money field itself PROVEN in save-layout.md.
inline constexpr std::uint32_t kMoneySeedNewGame = 1000;

// Purse ceiling -- CONFIRMED: FUN_0023dc58 (the single money mutator) clamps
// money to 999999 (0xF423F) after every transaction; the same literal appears
// in the shop/slot display paths (0x261638, 0x261de0, 0x268e40).
inline constexpr std::int32_t kMoneyCap = 999999;

// Money field offset inside the 0x3448 progress record (PROVEN, save-layout.md).
inline constexpr std::uint32_t kMoneySaveOffset = 0x654;

// SFX played by FUN_0023dc58 for any non-zero transaction -- CONFIRMED.
inline constexpr std::uint32_t kMoneySpendSfxId = 0x305;

// Ram budget of the bank-teller dialog -- CONFIRMED: initialized to 100000 by
// FUN_00227c68 (0x01782a6c) and compared by the deposit/withdraw confirm
// (FUN_0023a460: $amount > budget -> refuse). The teller's amount stepper
// starts at 1000 with a power-of-ten digit step.
inline constexpr std::uint32_t kBankBudgetInit = 100000;
inline constexpr std::uint32_t kBankStepperStart = 1000;

// Race payout ladder @0x0029FCB8, u32[5][6] -- CONFIRMED bytes (verified via
// phdr mapping, fileOffset = vaddr - 0x1ff000) and CONFIRMED consumers:
//  - FUN_00236238 SumPlayerPoints (licence purse) indexes
//    [race-record byte+3][position-DAT], positions at 0x0178700a/e/12 < 6.
//  - FUN_00236858 (World-Grand-Prix standings screen) sets the purse from the
//    same ladder for the player's rank among 8, credits it once at the end of
//    round 7 via FUN_0023dc58(-purse).
// Row = "tier" from the race record (0x002BDDD0, stride 0x10, byte 3):
// licence tiers 0/1/2 (records 0..23), tier 4 on the two WGP records. Which
// game mode owns tier 3 is INFERRED (it matches the WGP-scale purses).
// Column = zero-based finishing place (0 = 1st); no payout past 6th.
inline constexpr std::uint32_t kRacePayoutLadder[5][6] = {
    {800, 500, 400, 300, 200, 100},      // tier 0 (B-licence races)
    {1500, 1200, 1000, 800, 600, 500},    // tier 1 (A-licence races)
    {2500, 2000, 1600, 1200, 1000, 800},  // tier 2 (Super-A licence races)
    {80000, 60000, 40000, 30000, 20000, 10000},  // tier 3 (WGP-scale, INFERRED owner)
    {10000, 7500, 5000, 3000, 2000, 1000},       // tier 4 (WGP records 24/25)
};
inline constexpr int kRacePayoutTiers = 5;
inline constexpr int kRacePayoutPlaces = 6;

// Zero-based finishing place -> purse in G; 0 when out of the ladder.
[[nodiscard]] std::uint32_t racePayout(int tier, int place);

// Licence-test racer count: the licence scan of FUN_00237508 iterates 0x18
// races and requires a stored place-1 <= 5 in each to award the next licence
// -- CONFIRMED (top-6 finish required in every licence-tier race).
inline constexpr int kLicenceRaceCount = 24;
inline constexpr std::uint8_t kLicencePlaceStoredMax = 5;  // (place-1) <= 5
// Race-result sentinels -- PROVEN save-layout.md (0xFF never raced / int max).
inline constexpr std::uint8_t kRacePlaceUnraced = 0xFF;
inline constexpr std::uint32_t kBestTimeNone = 0x7FFFFFFF;
// Licence levels -- PROVEN save-layout.md (0x651), order CONFIRMED by the
// award strings "got a B-License"/"got a A-License" (FUN_002364b0).
inline constexpr std::uint8_t kLicenceNone = 0;
inline constexpr std::uint8_t kLicenceB = 1;
inline constexpr std::uint8_t kLicenceA = 2;
inline constexpr std::uint8_t kLicenceSuperA = 3;

// Best-time band layout -- CONFIRMED by FUN_00237c48 UpdateRaceRecords:
// 26 result slots per player, three player bands (player + 2 WGP teammates),
// i.e. 78 u32 rows for both tables (matches save-layout 0x1040/0x1178).
inline constexpr int kRaceResultSlotsPerPlayer = 26;
inline constexpr int kRaceResultPlayerBands = 3;

// Shop economy -- CONFIRMED: category prices dispatch through PTR table
// 0x002F1280 (15 getters) with the category selector table at 0x002DC8A8
// (category byte -> getter index 1..0xE); getter 0 (0x00243698 GetConst500)
// is the flat 500 price. Selling an owned item refunds half its price
// (FUN_00268e40: FUN_0023dc58(-price/2)).
inline constexpr std::uint32_t kShopFlatPriceCategory0 = 500;
inline constexpr int kSellPriceDivisor = 2;

// ===========================================================================
// 2. Job fares / distance income
// ===========================================================================

// Exhaustive static census: the only writers to the purse are
// FUN_0023dc58 (13 call sites) and the boot/default initializers; there is
// NO taxi-style per-customer fare faucet in the shipped code. The one
// distance-derived income is the mileage-exchange dialog spawned by message
// opcode 0x16 (FUN_0023a140): u32 counters at progress+0x65C[band] are
// divided by 1609 (metres per mile) and paid out at (band*10+10) G per mile
// -- all parts CONFIRMED from the decompile + disassembly (div by 0x649 at
// 0x0023a18c/0x0023a190, rate multiply in FUN_0023a1a4 region, credit at
// 0x0023a220).
inline constexpr std::uint32_t kMetresPerMile = 1609;  // 0x649
inline constexpr std::uint32_t kMileageExchangeSaveOffset = 0x65C;
inline constexpr int kMileageExchangeBands = 10;  // rate(0..9), INFERRED bound

// Exchange rate in G per whole mile for record band byte 0..9.
[[nodiscard]] std::uint32_t mileageExchangeRate(std::uint8_t band);

// ===========================================================================
// 3. Progression unlocks
// ===========================================================================

// Story-gate pair table @0x002A2B70, 25 x {u8 type, u8 id}. FUN_0023cd58
// (CheckUnlockCondition) scans it linearly: a (type,id) match reports
// progress-flag bit <index> of DAT_0177FDD0 as satisfied; a miss falls
// through to the special cases below on in keep -- CONFIRMED bytes and
// CONFIRMED consumer semantics, chapter/type meaning INFERRED.
struct UnlockGate {
    std::uint8_t type;
    std::uint8_t id;
};
inline constexpr UnlockGate kUnlockGates[25] = {
    {1, 29}, {6, 34}, {1, 30}, {7, 33}, {0, 0},  {0, 0},  {1, 31}, {0, 0},
    {7, 35}, {0, 0},  {6, 35}, {3, 39}, {3, 38}, {3, 37}, {2, 35}, {1, 32},
    {4, 25}, {3, 35}, {2, 36}, {4, 26}, {7, 36}, {1, 33}, {2, 37}, {3, 36},
    {6, 36},
};
inline constexpr int kUnlockGateCount = 25;

// Progress-flag bit index for a (type,id) gate, or -1 when not gated.
[[nodiscard]] int findUnlockGate(std::uint8_t type, std::uint8_t id);

// Time-of-day thresholds hard-coded in FUN_0023cd58's special cases
// -- CONFIRMED literals, INFERRED event names. All compare param_3
// (this pass reads it as the time-of-day tick counter, cf. section 6):
//   gate (6,10)  : 45000 < t - 0x1a5e0     => t >= 153001
//                  (153000 = 17:00, start of the sunset band in
//                  sound-and-daynight.md -- independent cross-check)
//   gate (13,0)  : 0x1c906 < t - 0xafc9     => t >= 161998
//                  (162000 = 18:00, dusk band end)
//   gate (2,29)  : 0x32898 < t              => t >= 206233 (22:55)
inline constexpr std::uint32_t kUnlockClockAfterSunset = 153001;
inline constexpr std::uint32_t kUnlockClockAfterDusk = 161998;
inline constexpr std::uint32_t kUnlockClockLate = 206233;

// Collectible bookkeeping -- CONFIRMED / PROVEN:
//  - Quick-Pic photos (gs_photos_take_function 0x0023C5C0): 100 slots, photo n
//    at bit n-1 (save 0x508); fully-collected test checks 64+36 sentinel bits
//    and then awards stamp id 0x60 -- CONFIRMED.
//  - Stamps: 100 bits (save 0x518, PROVEN); SetProgressFlagAndQueue
//    (FUN_0023CB68) awards stamp 0x33 when the progress bitmap fills.
//  - Mailbox capacity 128 (save 0x628, PROVEN).
inline constexpr int kPhotoCount = 100;
inline constexpr std::uint8_t kPhotoStampAllAward = 0x60;
inline constexpr int kStampCount = 100;
inline constexpr std::uint8_t kProgressStampAllAward = 0x33;
inline constexpr int kMailboxCapacity = 128;

// ===========================================================================
// 4. Mini-game scoring
// ===========================================================================

// Golf mini-game course table @0x002AC0C0, 9 records x 0x40:
//   +0x00 f32[4] point A, +0x10 f32[4] point B, +0x22 s16 par.
// FUN_00251b60 (UpdateGolfMiniGame) passes pars and yardages to
// FUN_00252608 InitGolfCourseTable -- CONFIRMED. Yardage =
// (|A-B| - kGolfYardMetreOffset) / kGolfYardMetreDivisor (constants @
// 0x0033302c = 8.5856 and 0x00333030 = 0.9144 = metres-per-yard) -- CONFIRMED.
// 0.9144 is the exact m->yard factor.
inline constexpr int kGolfHoles = 9;
inline constexpr std::uint8_t kGolfPar[9] = {3, 3, 5, 3, 5, 3, 5, 4, 5};
inline constexpr float kGolfYardMetreOffset = 8.585599899291992f;
inline constexpr float kGolfYardMetreDivisor = 0.9143999814987183f;

// Whole yards for a measured metre distance between cup points (0 when the
// offset has not been exceeded), matching FUN_00251b60's computation.
[[nodiscard]] std::uint32_t golfYardage(float metreDistance);

// Slot-machine rules -- CONFIRMED arithmetic:
//   FUN_00261638 (bet stepper): bet clamped to min(purse, 10000), starts at
//   1000, digits step by powers of ten; the bet is debited when the spin is
//   confirmed. FUN_00261de0 (payout screen): with the win flag (task+0x8028)
//   set, pay = bet x rowMult[result row], credited through FUN_0023dc58(-pay)
//   and capped at 999999. Result rows @0x002D9980, stride 0x30, 24 rows;
//   the first u32 of each row is the multiplier; the rest of the record is
//   the reel strip/menu data used by the surrounding state machine. Which
//   reel combination selects which row is INFERRED.
inline constexpr int kSlotRowCount = 24;
inline constexpr std::uint32_t kSlotBetMax = 10000;
inline constexpr std::uint32_t kSlotBetStart = 1000;
inline constexpr std::uint32_t kSlotRowMultiplier[24] = {
    18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18,
    18, 18, 18, 18, 18, 18, 3,  3,  3,  2,  2,  12,
};

// Payout for a bet on result row 0..23 (0 when row is out of range).
[[nodiscard]] std::uint32_t slotPayout(std::uint32_t bet, int row);

// ===========================================================================
// 5. Driving physics constants
// ===========================================================================

// Speed algebra -- CONFIRMED:
//   FUN_0021d358 GetSpeedFromF15Vector: length of the f15 velocity vector
//   x 216.0  (metres/frame -> km/h at 60 Hz).
//   FUN_00249458 CalculateSpeedometerValue: mph = raw / 1.6089999676
//   (kSpeedoMphDivisor @0x00332F44), integer-truncated and clamped to 186
//   (0xBA) while driving; the tach path rescales by max/186.
//   (216/1.609 = 134.16: the HUD speedometer reads mph; 186 mph * 1.609 =
//   299.27 km/h equivalent internal ceiling.)
inline constexpr float kSpeedRawToKmh = 216.0f;
inline constexpr float kSpeedoMphDivisor = 1.6089999675750732f;
inline constexpr std::int32_t kSpeedometerCapMph = 186;

// Integer mph shown on the speedometer for a raw display-speed value,
// clamped to kSpeedometerCapMph.
[[nodiscard]] int speedometerMph(float rawSpeed);

// Hard per-frame constants inside FUN_0021ba70 UpdateVehiclePhysics
// -- CONFIRMED by instruction search over the whole ELF:
//   0x0021bd20 `addiu v0,v0,-0x59`  fall-speed gravity while airborne
//   0x0021becc `addiu a1,v0,0x59`   wading push (water force)
//   0x0021be68/0x0021be80 `ori ?,zero,0xcf69`  nitro force cap (2 sites)
inline constexpr std::int32_t kGravityFallPerFrame = -0x59;
inline constexpr std::int32_t kWaterForcePerFrame = 0x59;
inline constexpr std::int32_t kNitroCap = 0xCF69;

// Equipped-part spec tables loaded by FUN_002180c0 LoadCarPerformanceSpecs.
// Record shapes CONFIRMED from the decompile; labels read from the string
// pool to name the rows. The car-mode part bytes map: [+1] tires, [+2]
// gearbox, [+3] chassis, [+4] engine, [+5] steering, [+6] brakes, [+0xd]
// horn.

// Tire/grip table @0x002ECA70, stride 0x1C, 13 records:
//   +0 label ptr, +8 price u32, +0xC grip s16[6] (per ground surface class,
//   order per car-physics.md "grip class": asphalt/dirt/sand/grass/snow-ice/
//   smooth), +0x1A bonus u16: nonzero adds +5 to the engine weight stat.
inline constexpr int kTireCount = 13;
struct TireSpec {
    const char* name;
    std::uint32_t price;
    std::uint16_t grip[6];
    std::uint16_t weightBonus;
};
inline constexpr TireSpec kTireSpecs[13] = {
    {"Normal", 200, {196, 168, 128, 168, 96, 64}, 0},
    {"Sports", 1000, {240, 196, 154, 160, 96, 64}, 0},
    {"Semi-Racing", 2000, {333, 168, 154, 140, 80, 64}, 0},
    {"Racing", 5000, {512, 102, 102, 100, 51, 64}, 0},
    {"HG Racing", 10000, {512, 160, 102, 128, 51, 64}, 0},
    {"Wet", 2000, {240, 196, 196, 160, 128, 64}, 0},
    {"HG Wet", 3000, {240, 196, 240, 160, 128, 64}, 0},
    {"Off-Road", 500, {196, 196, 160, 160, 128, 64}, 0},
    {"HG Off-Road", 3000, {240, 240, 160, 180, 168, 128}, 0},
    {"Studless", 1000, {196, 196, 160, 168, 168, 196}, 0},
    {"HG Studless", 3000, {240, 196, 160, 168, 196, 196}, 0},
    {"Big", 5000, {240, 240, 230, 210, 196, 96}, 1},
    {"Devil", 200000, {65280, 65280, 65280, 65280, 65280, 65280}, 0},
};

// Engine table @0x002ED328, stride 0x1C, 6 records: +0 label ptr, +8 price
// u32, +0xC u16[7] copied verbatim to car+0x22C (engine/nitro stat block).
// Param meanings are INFERRED (first word constant 0xFFA1).
inline constexpr int kEngineCount = 6;
struct EngineSpec {
    const char* name;
    std::uint32_t price;
    std::uint16_t params[7];
};
inline constexpr EngineSpec kEngineSpecs[6] = {
    {"Normal", 200, {65441, 116, 162, 227, 318, 446, 0}},
    {"Sports", 1000, {65441, 116, 182, 291, 408, 490, 0}},
    {"Power", 2000, {65441, 128, 220, 276, 387, 464, 557}},
    {"Speed", 4000, {65441, 128, 260, 327, 458, 550, 660}},
    {"Wide", 7000, {65441, 144, 300, 414, 539, 647, 711}},
    {"Hyper", 10000, {65441, 156, 350, 446, 550, 625, 750}},
};

// Gearbox table @0x002ECED0, stride 0x18, 8 records: +0 label ptr, +8 price
// u32, +0xC u32 -> car+0x214 (speed-class field), +0x10 u16 -> car+0x240
// (gear count).
inline constexpr int kGearboxCount = 8;
struct GearboxSpec {
    const char* name;
    std::uint32_t price;
    std::uint32_t speedClass;
    std::uint16_t gears;
};
inline constexpr GearboxSpec kGearboxSpecs[8] = {
    {"Normal", 200, 1500, 3},  {"Panther", 500, 1800, 4},
    {"Blue MAX", 1000, 2200, 5}, {"Blue MAX V2", 1500, 2600, 6},
    {"MAD", 2000, 2900, 7},    {"MAD V2", 4000, 3300, 8},
    {"Long MAD", 8000, 3600, 6}, {"Black MAX", 12000, 3900, 8},
};

// Chassis table @0x002ED1F8, stride 0x10, 5 records: +0 label ptr, +8 price
// u32, +0xC s32 weight stat -> car+0x218 (engine-force/drag divisor, per
// car-physics.md; smaller = lighter/faster).
inline constexpr int kChassisCount = 5;
struct ChassisSpec {
    const char* name;
    std::uint32_t price;
    std::int32_t weightStat;
};
inline constexpr ChassisSpec kChassisSpecs[5] = {
    {"Normal", 200, 25}, {"Light", 500, 22}, {"Feather", 1000, 20},
    {"Phantom", 2000, 18}, {"Hyper", 4000, 15},
};

// Steering table @0x002ED4E8, stride 0x10, 4 records: +8 price u32,
// +0xC s16 multiplier -> car+0x242 (steering-angle multiplier).
inline constexpr int kSteeringCount = 4;
struct SteeringSpec {
    const char* name;
    std::uint32_t price;
    std::int16_t multiplier;
};
inline constexpr SteeringSpec kSteeringSpecs[4] = {
    {"Normal", 200, 64}, {"Quick", 500, 96},
    {"X2 Quick", 1000, 128}, {"X3 Quick", 2000, 160},
};

// Brake curve table @0x002ED5DC, stride 0x2C, 4 records: 32 x u8 force ramp
// (ramp slot 0..31 == brake frames held 1..32), price u32 at +0x28
// {1000, 1500, 2000}; the 4th record's tail words are overwritten by the
// following inline string pool, so its price is unpinned (0 here).
// Force applied per frame (per car-physics.md): curve[ramp-1] * 10000 >> 5.
inline constexpr int kBrakeCount = 4;
inline constexpr int kBrakeRampFrames = 32;
struct BrakeSpec {
    std::uint32_t price;
    std::uint8_t curve[32];
};
inline constexpr BrakeSpec kBrakeSpecs[4] = {
    {1000, {1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 4, 4, 4, 5, 5, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32}},
    {1500, {4, 7, 10, 12, 14, 16, 17, 19, 20, 21, 22, 23, 24, 25, 25, 26, 26, 27, 28, 28, 28, 29, 29, 30, 30, 30, 31, 31, 31, 32, 32, 32}},
    {2000, {0, 1, 1, 1, 1, 2, 2, 2, 3, 3, 4, 4, 4, 5, 6, 6, 7, 8, 8, 9, 10, 11, 12, 13, 15, 16, 18, 20, 22, 25, 28, 32}},
    {0, {1, 1, 1, 1, 2, 2, 3, 3, 4, 5, 6, 7, 8, 10, 13, 16, 20, 22, 24, 26, 27, 28, 29, 29, 30, 30, 31, 31, 31, 32, 32, 32}},
};

// Brake force for rampFrames in 1..32 (0 elsewhere), the *10000>>5 scaling
// included, exactly as consumed by gs_controls_car_update.
[[nodiscard]] std::int32_t brakeForce(int brakeIndex, int rampFrames);

// Grip of tireIndex (0..12) on surfaceClass (0..5); 0 when out of range.
[[nodiscard]] std::int16_t tireGrip(int tireIndex, int surfaceClass);

// ===========================================================================
// 6. Day / night cycle
// ===========================================================================

// Free clock -- PROVEN (sound-and-daynight.md): world+0x14, wraps at 216000
// ticks (0x34BC0) => exactly 9000 ticks per game hour at 60 Hz.
inline constexpr std::uint32_t kClockTicksPerDay = 216000;
inline constexpr std::int32_t kClockTicksPerHour = 9000;

// Scripted day-cycle lighting steps (funfair area intro):
//   start-clock table @0x002DCD28, u32[10] -- CONFIRMED bytes and consumer
//   (FUN_0026ca48 ApplyTimeOfDayLighting sets the clock from it; xref
//   0x0026caf8).
//   step-duration table @0x002DCD50, u16[10] -- CONFIRMED bytes and consumer
//   (FUN_0026d1c0 AdvanceDayCycleStep waits on *(u16*)(0x2DCD50+step*2) and
//   spawns the fade-in 64 ticks early; ends after step 9).
// Hour column derived (CONFIRMED arithmetic): 216000 ticks / 24 h.
inline constexpr std::uint32_t kDayCycleStepClock[10] = {
    72000, 81000, 108000, 117000, 126000, 135000, 162000, 163000, 171000, 189000,
};
inline constexpr std::uint16_t kDayCycleStepTicks[10] = {
    0, 1260, 2520, 3780, 5040, 6300, 7560, 8820, 10080, 12033,
};
inline constexpr int kDayCycleStepCount = 10;
inline constexpr std::uint32_t kDayCycleFadeTicks = 64;

// Clock tick (vblanks since midnight) at which each scripted step begins.
[[nodiscard]] std::uint32_t dayCycleStepClock(int step);

// Duration in vblank ticks of each scripted step (step 1..9; 0 terminates).
[[nodiscard]] std::uint32_t dayCycleStepTicks(int step);

}  // namespace rt::native::gameplay
