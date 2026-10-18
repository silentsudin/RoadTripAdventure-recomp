// guest_vehicle.cpp — structural translation of the guest longitudinal car
// model. Sources: build/decomp/fn/00218ee0.c (AdvanceVehicleMotion),
// build/decomp/fn/0021a310.c (gs_controls_car_update, longitudinal slice),
// build/scratch/w9/gear_policy_disasm.txt (policies, delay-slot-exact decode
// of 0x21CE48 / 0x21CF80). Variable comments name the pseudocode slot.

#include "gameplay/guest_vehicle.h"

#include <cmath>

namespace rt::native::gameplay {

// Guest signed division (LO quotient). Divide-by-zero runs the trap(7) guard
// (break 0,7); never observed on driven paths, and the pseudocode gates it.
// shortcut: returns 0 on x/0, upgrade if a caller ever reaches the trap.
static int eeDiv(int a, int b) {
    if (b == 0) return 0;
    if (b == -1) return (int)-(unsigned)a;  // EE quotient overflow wraps
    return a / b;
}

// Guest sll v,7 (wraps as unsigned).
static int shl7(int v) { return (int)((unsigned)v << 7); }

// Guest "(if neg +0x7f) then >>7": round toward zero for negatives.
static int round7(int v) {
    if (v < 0) v += 0x7f;
    return v >> 7;  // sra: arithmetic shift
}

// Gear policy A @ 0x0021CE48. Leaf; a0 gear, a1 motion, a2 pad, a3 &gearRow.
int gearPolicyA(int gear, int motion, uint32_t pad, const int16_t* row) {
    if (pad & 4) return 0;                        // reverse mode: gear pinned 0
    int t0 = gear;
    if (t0 == 0) t0 = ((pad & 5) == 1) ? 1 : 0;   // neutral + gas -> first
    const bool gas = pad & 1, brake = pad & 2;
    if (!gas || t0 == 0) {                        // 0x21CE78/CE80 -> 0x21CF38
        if (!brake) return t0;
        if (t0 < 2) return t0;
        return eeDiv(shl7(motion), row[t0 - 1]) < 8000 ? t0 - 1 : t0;
    }
    if (eeDiv(shl7(motion), row[t0]) >= 8201) {   // 0x21CEA0 upshift window
        if (brake) {
            if (t0 < 3) return t0;                // 0x21CEB8 delay-slot guard
            // Coast eval at 0x21CEE8 with proj >= 8201: the 5000 test always
            // holds, so the downshift path is unreachable. Ported anyway.
            if (eeDiv(shl7(motion), row[t0]) >= 5000) return t0;
            if (eeDiv(shl7(motion), row[t0 - 1]) < 8000) return t0 - 1;
            return t0;
        }
        // bgtzl at 0x21CECC jumps over its addiu, but the EE runs branch-likely
        // delay slots unconditionally -> +1 either way. Oracle confirms:
        // shifts 1->2->3->4 all have a positive next row.
        if (t0 < 6) return t0 + 1;
        return t0;
    }
    if (t0 < 3) return t0;                        // 0x21CEE0 guard (CEAC delay slti)
    if (eeDiv(shl7(motion), row[t0]) >= 5000) return t0;
    if (eeDiv(shl7(motion), row[t0 - 1]) < 8000) return t0 - 1;
    return t0;
}

// Gear policy B @ 0x0021CF80: same shape, thresholds 5001 / 3000 / 4000
// (brake downshift still 8000). Not the race policy; kept for the handler slot.
int gearPolicyB(int gear, int motion, uint32_t pad, const int16_t* row) {
    if (pad & 4) return 0;
    int t0 = gear;
    if (t0 == 0) t0 = ((pad & 5) == 1) ? 1 : 0;
    const bool gas = pad & 1, brake = pad & 2;
    if (!gas || t0 == 0) {
        if (!brake) return t0;
        if (t0 < 2) return t0;
        return eeDiv(shl7(motion), row[t0 - 1]) < 8000 ? t0 - 1 : t0;
    }
    if (eeDiv(shl7(motion), row[t0]) >= 5001) {
        if (brake) {
            if (t0 < 3) return t0;
            if (eeDiv(shl7(motion), row[t0]) >= 3000) return t0;
            if (eeDiv(shl7(motion), row[t0 - 1]) < 4000) return t0 - 1;
            return t0;
        }
        if (t0 < 6) return t0 + 1;                // EE branch-likely: +1 either way
        return t0;
    }
    if (t0 < 3) return t0;                        // 0x21D018 guard (CFE4 delay slti)
    if (eeDiv(shl7(motion), row[t0]) >= 3000) return t0;
    if (eeDiv(shl7(motion), row[t0 - 1]) < 4000) return t0 - 1;
    return t0;
}

// AdvanceVehicleMotion @ 0x00218EE0. Longitudinal out term only (out[2]);
// the lateral out[0] (00218ee0.c:240) belongs to the unported lateral slice.
int advanceVehicleMotion(GuestCarBlock& car, int motion, int lat, int throttle,
                         int brakeIn, uint32_t pad, bool active, int* outMotion) {
    int slip = 0;                                  // iVar9 (:38)
    short target = 0;                              // sVar6 (:39)
    const short rpm = (short)car.rpm;              // sVar4 (:40)
    if ((short)car.rpm < 0x251d) {                 // :41-54
        if (pad & 1) {
            int acc = car.shiftAcc - (int)car.shiftDec;
            car.shiftAcc = acc;
            if (acc < 0) { car.shiftAcc = 0; target = 3000; }
            else target = 9000;
        }
    }
    const short ratio = car.gearRow[(signed char)car.gear];  // sVar3 (:60, char index)
    int th = throttle;
    if (!active) th = 0;                           // iVar5 (:61-64)
    int inv12 = eeDiv(0x280, (int)ratio);          // iVar12 (:65, trap hoisted)
    const int resp = car.response;                 // iVar8 (:70/74)
    const int slipLim = active ? throttle * resp : 0;  // iVar10 (:69-76, raw param_5)
    if (inv12 < 0) inv12 = -inv12;                 // :77-79
    const int r13 = th != 0 ? resp : 0;            // iVar13 (:80-83)
    if (inv12 == 0) inv12 = 1;                     // :84-86
    int engine;                                    // iVar5 (:87-104)
    if (rpm < target) engine = eeDiv(shl7(car.accel), (int)ratio);
    else if (target < rpm) engine = eeDiv(-0x3e800, (int)ratio);
    else engine = 0;
    const int ratioI = (int)ratio;                 // iVar11 (:105)
    int slipTerm = 0;                              // iVar14 (:106)
    if (slipLim != 0) {                            // :109-115
        slip = motion - round7((int)rpm * ratioI);
        slipTerm = slip * r13;
    }
    const int acc98 = eeDiv(engine + slipTerm, resp);  // iStack_98 (:117)
    int adj = (round7((int)rpm * ratioI) + acc98) * r13;  // iVar15 (:118-125)
    int brake = brakeIn;                           // param_6 (:126-137)
    if (adj < 0) {
        if (-brake < adj) brake = -adj;            // LAB_002190d4 (branch into else)
    } else {
        brake = -(adj < brake ? adj : brake);
    }
    engine += brake;                               // iVar5 (:138)
    int dMotion = eeDiv(engine, resp);             // iVar15 (:139, kept if band path)
    const int excess = engine - slipTerm;          // iVar14 (:143)
    int dc = -lat * r13;                           // iStack_dc (:144-145)
    int slipFlag = 0;                              // iStack_9c (:146-170)
    if (slipLim < 0) {
        // LAB_00219210: flag stays 0
    } else if (slipLim == 0) {
        dc = 0;
    } else {
        const float f = std::sqrt((float)excess * (float)excess + (float)dc * (float)dc);
        if (f > (float)slipLim) {
            const int d2 = slipLim << 1;           // iVar8 (:157)
            if (dc < 0) {
                if (d2 < -dc) dc = -slipLim * 2;   // :158-161
            } else {
                const int dc0 = dc;
                dc = d2;
                if (dc0 <= d2) dc = dc0;           // :163-167
            }
            slipFlag = (int)(f / (float)slipLim);  // :169
        }
    }
    int v8 = 0;                                    // iVar8 (:171)
    int rpmAcc = acc98;                            // iVar13 (:172)
    if (throttle < 0) {                            // :173-181 (param_5 raw)
        if (-1 < (int)rpm + eeDiv(shl7(acc98), ratioI)) {
            v8 = 0;
            goto lab_31c;                          // skips the /= inv12 at :234
        }
        dMotion = -motion;
    } else {
        const int negLim = -slipLim;               // iVar7 (:184)
        if (slipLim < excess) {                    // :185-203
            v8 = engine - slipLim;
            rpmAcc = eeDiv(slipLim, resp);
            if (slip < 0) {
                if (slip <= rpmAcc + eeDiv(v8, inv12)) {
                    dMotion = eeDiv(slipLim, resp);
                    goto lab_308;
                }
                v8 = 0;
                rpmAcc = slip;
            }
            dMotion = eeDiv(slipLim, resp);
        } else if (negLim <= excess) {             // :206-208 (|excess| <= slipLim)
            v8 = 0;
            goto lab_31c;                          // dMotion keeps engine/resp from :139
        } else {                                   // :210-227 (mirror)
            v8 = engine + slipLim;
            rpmAcc = eeDiv(negLim, resp);
            if (slip > 0) {
                if (rpmAcc + eeDiv(v8, inv12) <= slip) {
                    dMotion = eeDiv(negLim, resp);
                    goto lab_308;
                }
                v8 = 0;
                rpmAcc = slip;
            }
            dMotion = eeDiv(negLim, resp);
        }
lab_308:
        ;                                          // :229-232 trap guard, no-op via eeDiv
    }
    v8 = eeDiv(v8, inv12);                         // :234
lab_31c:
    *outMotion = motion + dMotion;                 // param_1[2] (:239)
    // guest :240 out[0] = lat + dc/resp stays with the lateral caller
    car.updateFlags |= 2;                          // :242
    const int rpmNew = (int)car.rpm + eeDiv((rpmAcc + v8) * 0x80, ratioI);  // :252 (u16 rpm)
    car.rpm = (uint16_t)(short)rpmNew;             // :253
    if ((short)rpmNew > 10000) car.rpm = 10000;    // :254 (s16-extended compare)
    if ((short)car.rpm < 0) car.rpm = 0;           // :257
    if (!active) {
        car.wheelArc = 0;                          // :260-265
        slipFlag = 0;
        *outMotion = 0;                            // piStack_b0[2]
        // guest zeroes out[0] as well (:263); lateral, not ported
    } else {
        car.wheelArc = round7((int)(short)car.rpm * ratioI);  // :267-271 (assignment)
    }
    return slipFlag;                               // :279 (DAT_003356d4 debug skipped)
}

// gs_controls_car_update @ 0x0021A310 longitudinal slice (:31-155).
int gsCarUpdateMotion(GuestCarBlock& car, GearPolicy policy, uint32_t pad,
                      int motion, int latPos, bool drivable, int* outMotion) {
    // :31-36 gate: full word @+0x1E0 (damp1E0|damp1E4 cover 0x1E0..0x1E7)
    const int gate = (car.damp1E0 != 0 || car.damp1E4 != 0) ? 0x59 : 0;
    car.gear = (uint8_t)policy((int)car.gear, motion, pad, car.gearRow);  // :38-41
    int am = motion < 0 ? -motion : motion;        // iVar15 (:42-45)
    if (am < car.speedWord) car.speedWord = am;    // :46-48
    long sp = (long)(short)car.steerPos;           // lVar9 (:49)
    int i7 = (int)(short)car.steerPos;             // iVar7 (:50)
    int i8;                                        // iVar8 (gripRate carrier)
    if ((pad & 0x8000) == 0) {                     // :51
        if ((pad & 0x2000) == 0) {                 // :52 straight line
            if (am != 0) {
                if (sp < 4) {
                    if (sp < 1) {
                        if (sp < -3) sp = (long)(i7 + 4);
                        else if (sp < 0) sp = 0;
                    } else {
                        sp = 0;
                    }
                } else {
                    sp = (long)(i7 - 4);
                }
                car.speedWord = am;                // :70
            }
        } else {                                   // :73-84 right pressed
            if (sp < 0) { car.speedWord = am; sp = -(long)i7; goto lab_450; }
            i7 = i7 + 1;
            if (i7 < 0x21) { i8 = car.gripRate; goto lab_454; }
            sp = 0x20;
        }
lab_450:
        i7 = (int)sp;
        i8 = car.gripRate;
    } else {                                       // :89-102 left pressed
        if (sp > 0) { car.speedWord = am; sp = -(long)i7; goto lab_450; }
        i7 = i7 - 1;
        if (i7 < -0x20) { sp = -0x20; goto lab_450; }
        i8 = car.gripRate;
    }
lab_454:
    car.steerPos = (short)i7;                      // :104
    i8 = i8 * i7;
    if (i8 < 0) i8 += 0x1f;
    i8 >>= 5;
    short grip = 0;                                // sVar4 (:110)
    if (i8 != 0) {                                 // :111-129
        int t = eeDiv(car.speedWord * 0x28be, i8); // :115
        // :116 flips on iVar15, which the pseudocode proved >= 0: dead as
        // written. gripOut only feeds the unported lateral law.
        if (0 < i8 && t < 0x18000) t = 0x18000;    // :119-121
        if (i8 < 0 && -0x18000 < t) t = -0x18000;  // :122-124
        grip = (short)eeDiv(0x1b4c14b6, t);        // :128 (t==0 trap -> 0)
    }
    car.gripOut = grip;                            // :130 (guest :131-133 *param_5
                                                   // steering-angle out stays unported)
    int brakeDelta;
    if ((pad & 2) == 0) {                          // :134-137
        car.brakeRamp = 0;
        brakeDelta = 0;
    } else {
        if (car.brakeRamp < 0x20) ++car.brakeRamp; // :139-145
        // guest +0x200 table pointer -> host brakeTable (header note).
        // shortcut: delta 0 when no table, native load always fills one.
        brakeDelta = car.brakeTable
            ? (int)((unsigned)car.brakeTable[car.brakeRamp - 1] * 10000u) >> 5
            : 0;
    }
    int thWord = gate * (int)car.tireGrip[car.tireIdx & 7];  // :148-149
    if (thWord < 0) thWord += 0xff;                // :150-152
    return advanceVehicleMotion(car, motion, latPos, thWord >> 8, brakeDelta,
                                pad, drivable, outMotion);  // :153-155
}

}  // namespace rt::native::gameplay
