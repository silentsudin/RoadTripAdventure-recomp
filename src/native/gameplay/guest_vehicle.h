#pragma once
//
// guest_vehicle.h — translation of the guest's longitudinal car model:
//   gs_controls_car_update @ 0x0021A310 (gear-policy dispatch, steering
//     self-centre, speed word, brake ramp, throttle word),
//   AdvanceVehicleMotion @ 0x00218EE0 (the motion/RPM integrator),
//   gear policy A @ 0x0021CE48, gear policy B @ 0x0021CF80.
//
// Structural translation of build/decomp/fn/00218ee0.c and 0021a310.c plus
// the MIPS listings in build/scratch/w9/gear_policy_disasm.txt (policies are
// leaf functions; decoded with delay-slot semantics). Guest int division
// truncates toward zero like C++ `/`.
//
// Not ported here (lateral slice): UpdateCarTireSound, the tire-slip
// accumulator law at 0x0021A310 lines 157-253, TransformVecByF15Matrix,
// UpdateSteeringDamping. Guest trap(7) zero-div guards become eeDiv -> 0.
//
// Deviations from the guest record (documented):
//   - brakeTable: the guest holds a guest-RAM pointer at +0x200; the host
//     struct carries a host `const uint8_t*` at 0x208 instead (asset load
//     fills it). The +0x200 word itself is never read.
//   - the DAT_003356d4 debug sprintf path is dead in the native app (0).

#include <cstddef>
#include <cstdint>

namespace rt::native::gameplay {

// Guest car block fields 0x000..0x244 (struct base = guest car object start),
// offsets pinned by static_assert.
struct GuestCarBlock {
    uint8_t gap_0[0x19C];
    uint32_t tireIdx;        // 0x19C surface/tire index; & 7 selects tireGrip
    uint8_t gap_1a0[0x1B8 - 0x1A0];
    int32_t tireSlipAccum;   // 0x1B8 lateral law state (not ported, untouched)
    int32_t wheelArc;        // 0x1BC (rpm*ratio rounded) >> 7, rewritten each frame
    uint8_t gap_1c0[0x1CC - 0x1C0];
    int16_t gripOut;         // 0x1CC turn-resistance scalar (lateral law feeds it)
    int16_t steerPos;        // 0x1CE self-centring steering position (+-0x20)
    uint16_t rpm;            // 0x1D0 low half read as s16 by the integrator
    uint16_t rpmUpper;       // 0x1D2 separate s16 (lateral law; untouched here)
    int16_t lat1D4;          // 0x1D4 lateral law state (not ported)
    int16_t lat1D6;          // 0x1D6 lateral law state (not ported)
    int32_t speedWord;       // 0x1D8 |motion| of this frame's motion input
    int32_t damp1DC;         // 0x1DC damping word (oracle: 0x1000)
    int32_t damp1E0;         // 0x1E0 throttle gate: damp1E0|damp1E4 == 0 -> no throttle
    int32_t damp1E4;         // 0x1E4 damping word
    uint8_t gap_1e8[0x1F8 - 0x1E8];
    uint32_t updateFlags;    // 0x1F8 bit1 set on every integrator call
    uint16_t gap_1fc;        // 0x1FC
    uint8_t brakeRamp;       // 0x1FE brake ramp step 0..0x20
    uint8_t gear;            // 0x1FF gear index, 0 = reverse
    uint8_t gap_200[8];      // 0x200 guest's u32 table pointer, replaced (see above)
    const uint8_t* brakeTable; // host stand-in for the byte table at guest +0x200
    uint32_t gap_210;        // 0x210
    int32_t accel;           // 0x214 engine accel word (oracle race car: 1800)
    int32_t response;        // 0x218 response divisor (oracle race car: 25)
    int16_t tireGrip[8];     // 0x21C grip per tire slot (loaded at 0x21C, guest reads via +0x210+0xc)
    int16_t gearRow[8];      // 0x22C final-drive ratio per gear (oracle: -95,116,162,227,318,446,0,0)
    int32_t shiftAcc;        // 0x23C throttle accumulation (u32 word)
    int16_t shiftDec;        // 0x240 per-frame accumulator decrement
    int16_t gripRate;        // 0x242 steering grip rate scalar
    // 0x244 end of the covered span
};
static_assert(offsetof(GuestCarBlock, tireIdx) == 0x19C, "guest layout 0x19C");
static_assert(offsetof(GuestCarBlock, tireSlipAccum) == 0x1B8, "guest layout 0x1B8");
static_assert(offsetof(GuestCarBlock, wheelArc) == 0x1BC, "guest layout 0x1BC");
static_assert(offsetof(GuestCarBlock, gripOut) == 0x1CC, "guest layout 0x1CC");
static_assert(offsetof(GuestCarBlock, steerPos) == 0x1CE, "guest layout 0x1CE");
static_assert(offsetof(GuestCarBlock, rpm) == 0x1D0, "guest layout 0x1D0");
static_assert(offsetof(GuestCarBlock, rpmUpper) == 0x1D2, "guest layout 0x1D2");
static_assert(offsetof(GuestCarBlock, speedWord) == 0x1D8, "guest layout 0x1D8");
static_assert(offsetof(GuestCarBlock, damp1E0) == 0x1E0, "guest layout 0x1E0");
static_assert(offsetof(GuestCarBlock, damp1E4) == 0x1E4, "guest layout 0x1E4");
static_assert(offsetof(GuestCarBlock, updateFlags) == 0x1F8, "guest layout 0x1F8");
static_assert(offsetof(GuestCarBlock, brakeRamp) == 0x1FE, "guest layout 0x1FE");
static_assert(offsetof(GuestCarBlock, gear) == 0x1FF, "guest layout 0x1FF");
static_assert(offsetof(GuestCarBlock, accel) == 0x214, "guest layout 0x214");
static_assert(offsetof(GuestCarBlock, response) == 0x218, "guest layout 0x218");
static_assert(offsetof(GuestCarBlock, tireGrip) == 0x21C, "guest layout 0x21C");
static_assert(offsetof(GuestCarBlock, gearRow) == 0x22C, "guest layout 0x22C");
static_assert(offsetof(GuestCarBlock, shiftAcc) == 0x23C, "guest layout 0x23C");
static_assert(offsetof(GuestCarBlock, shiftDec) == 0x240, "guest layout 0x240");
static_assert(offsetof(GuestCarBlock, gripRate) == 0x242, "guest layout 0x242");

// Gear policy fn ptr: gs_game_scene3d_handler slot for the gear argument
// tuple (a0 gear, a1 motion, a2 pad, a3 &gearRow[0]); the caller's 5th
// argument (rpm) is unused by both policies.
using GearPolicy = int (*)(int gear, int motion, uint32_t pad, const int16_t* row);

// Policy A @ 0x0021CE48 (race/town handler; oracle-confirmed by the upshift
// at gear 1). Policy B @ 0x0021CF80: same shape, thresholds 5001/3000/4000.
int gearPolicyA(int gear, int motion, uint32_t pad, const int16_t* row);
int gearPolicyB(int gear, int motion, uint32_t pad, const int16_t* row);

// AdvanceVehicleMotion @ 0x00218EE0, longitudinal-relevant half of the out
// vector: outMotion is out[2] (motion' = motion + dMotion). The guest's
// out[0] lateral term stays in the caller (not ported). pad bits: 1 gas,
// 2 brake, 4 reverse mode, 0x2000/0x8000 steer. Returns the slip flag
// (iStack_9c), which UpdateCarTireSound would consume.
int advanceVehicleMotion(GuestCarBlock& car, int motion, int lat, int throttle,
                         int brakeDelta, uint32_t pad, bool active, int* outMotion);

// gs_controls_car_update @ 0x0021A310 longitudinal slice: runs the gear
// policy, the steering self-centre and speed word, the brake ramp, the
// throttle word, then the integrator. latPos is the caller's *param_3
// (lateral component of the motion vector; 0 in a straight line).
// outMotion = the integrator's out[2]. Returns the slip flag.
int gsCarUpdateMotion(GuestCarBlock& car, GearPolicy policy, uint32_t pad,
                      int motion, int latPos, bool drivable, int* outMotion);

}  // namespace rt::native::gameplay
