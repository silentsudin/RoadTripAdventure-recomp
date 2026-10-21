// guest:0x00249fa0 transliterated from Ghidra pseudocode, hand-fixed (build/scratch/w13-orphan/REPORT.md)
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t FUN_00249fa0(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, uint32_t param_5, uint32_t param_6, ...) {
  rtguest::FrameGuard rt_fg(288);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x90) = (uint32_t)(int32_t)param_2;
    *(uint32_t*)rtguest::G(rt_sv+0xe8) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0xec) = 0x00000000u;
  }
  const uint32_t rt_sp = rtguest::frameTop();
    uint32_t fStack_98 = rt_sp + (0x120 - 0x98);  // sp+0x88
    uint32_t iStack_90 = rt_sp + (0x120 - 0x90);  // sp+0x90
    uint32_t uStack_100 = rt_sp + (0x120 - 0x100);  // sp+0x20
    uint32_t uStack_104 = rt_sp + (0x120 - 0x104);  // sp+0x1c
    uint32_t uStack_108 = rt_sp + (0x120 - 0x108);  // sp+0x18
    uint32_t uStack_10c = rt_sp + (0x120 - 0x10c);  // sp+0x14
    uint32_t uStack_110 = rt_sp + (0x120 - 0x110);  // sp+0x10
    uint32_t uStack_114 = rt_sp + (0x120 - 0x114);  // sp+0xc
    uint32_t uStack_a4 = rt_sp + (0x120 - 0xa4);  // sp+0x7c
    uint32_t uStack_a8 = rt_sp + (0x120 - 0xa8);  // sp+0x78
    uint32_t uStack_ac = rt_sp + (0x120 - 0xac);  // sp+0x74
    uint32_t uStack_b0 = rt_sp + (0x120 - 0xb0);  // sp+0x70
    uint32_t uStack_b4 = rt_sp + (0x120 - 0xb4);  // sp+0x6c
    uint32_t uStack_b8 = rt_sp + (0x120 - 0xb8);  // sp+0x68
    uint32_t uStack_bc = rt_sp + (0x120 - 0xbc);  // sp+0x64
    uint32_t uStack_c0 = rt_sp + (0x120 - 0xc0);  // sp+0x60
    uint32_t uStack_c4 = rt_sp + (0x120 - 0xc4);  // sp+0x5c
    uint32_t uStack_c8 = rt_sp + (0x120 - 0xc8);  // sp+0x58
    uint32_t uStack_cc = rt_sp + (0x120 - 0xcc);  // sp+0x54
    uint32_t uStack_d0 = rt_sp + (0x120 - 0xd0);  // sp+0x50
    uint32_t uStack_d4 = rt_sp + (0x120 - 0xd4);  // sp+0x4c
    uint32_t uStack_d8 = rt_sp + (0x120 - 0xd8);  // sp+0x48
    uint32_t uStack_dc = rt_sp + (0x120 - 0xdc);  // sp+0x44
    uint32_t uStack_e0 = rt_sp + (0x120 - 0xe0);  // sp+0x40
    uint32_t uStack_e4 = rt_sp + (0x120 - 0xe4);  // sp+0x3c
    uint32_t uStack_e8 = rt_sp + (0x120 - 0xe8);  // sp+0x38
    uint32_t uStack_ec = rt_sp + (0x120 - 0xec);  // sp+0x34
    uint32_t uStack_f0 = rt_sp + (0x120 - 0xf0);  // sp+0x30
    uint32_t uStack_f4 = rt_sp + (0x120 - 0xf4);  // sp+0x2c
    uint32_t uStack_f8 = rt_sp + (0x120 - 0xf8);  // sp+0x28
    uint32_t uStack_fc = rt_sp + (0x120 - 0xfc);  // sp+0x24
  undefined4 uVar1;
  undefined4 uVar2;
  uint uVar3;
  long lVar4;
  int iVar5;
  int iVar6;
uint32_t puVar7;
  float fVar8;
  undefined4 uVar9;
  undefined4 uVar10;
    uint32_t aiStack_120 = rt_sp + (0x120 - 0x120);  // sp+0x0
    uint32_t auStack_a0 = rt_sp + (0x120 - 0xa0);  // sp+0x80
  
  iVar5 = (param_4 + param_5) * 8;
  puVar7 = 0x177ac50 + param_5 * 0x270;  // &gs_controls_car_object (undefined8[]) + 0x4e elements = 0x270 bytes
  uVar1 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x2a9038)));
  uVar2 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x2a903c)));
  (*(int32_t*)rtguest::G((uint32_t)iStack_90)) = param_2;
  RT_CALLRA(0x24a030, guest::uiArenaInit(param_3,0x1000));
  RT_CALLRA(0x24a040, guest::uiArenaBeginBatch(param_3,0,0));
  RT_CALLRA(0x24a058, guest::uiArenaSetTexture(param_3,0x2a9050,0,0));
  RT_CALLRA(0x24a064, guest::uiArenaDataTag(param_3,1));
  (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(aiStack_120)), 0, 0x20));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_110)) = 0x8c;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_10c)) = 0x8d;
  RT_CALLRA(0x24a0a0, guest::uiArenaSprites(param_3,aiStack_120,1,uVar1,uVar2,1));
  if (param_6 != 0) {
    RT_CALLRA(0x24a0b0, guest::uiArenaDataTag(param_3,1));
    lVar4 = (int32_t)RT_CALLRA(0x24a0c0, guest::GetVehicleTransmissionMode(puVar7,0,0));
    (*(int32_t*)rtguest::G((uint32_t)aiStack_120 + (uint32_t)(0) * 4)) = 0x56;
    (*(int32_t*)rtguest::G((uint32_t)aiStack_120 + (uint32_t)(1) * 4)) = 0x2f;
    if (lVar4 == 0) {
      (*(int32_t*)rtguest::G((uint32_t)aiStack_120 + (uint32_t)(2) * 4)) = 0x90;
    }
    else {
      (*(int32_t*)rtguest::G((uint32_t)aiStack_120 + (uint32_t)(2) * 4)) = 0xb0;
      if (lVar4 != 1) {
        (*(int32_t*)rtguest::G((uint32_t)aiStack_120 + (uint32_t)(2) * 4)) = 0xa0;
      }
    }
    (*(uint32_t*)rtguest::G((uint32_t)uStack_114)) = 0x20;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_10c)) = 0x10;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_110)) = 0x10;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_108)) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_104)) = 0;
    RT_CALLRA(0x24a130, guest::uiArenaSprites(param_3,aiStack_120,1,uVar1,uVar2,1));
  }
  iVar5 = RT_CALLRA(0x24a144, guest::GetVehicleGear(puVar7,0,0));
  RT_CALLRA(0x24a154, guest::uiArenaDataTag(param_3,1));
  (*(int32_t*)rtguest::G((uint32_t)aiStack_120 + (uint32_t)(2) * 4)) = iVar5 * 0xd + 0x90;
  (*(int32_t*)rtguest::G((uint32_t)aiStack_120 + (uint32_t)(0) * 4)) = 0x5d;
  (*(int32_t*)rtguest::G((uint32_t)aiStack_120 + (uint32_t)(1) * 4)) = 0x5e;
  iVar6 = 2;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_110)) = 0xd;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_10c)) = 0xf;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_114)) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_108)) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_104)) = 0;
  RT_CALLRA(0x24a1c0, guest::uiArenaSprites(param_3,aiStack_120,1,uVar1,uVar2,1));
  uVar3 = RT_CALLRA(0x24a1c8, guest::CalculateSpeedometerDigits(puVar7));
  iVar5 = 0x6f;
  do {
    iVar6 = iVar6 + -1;
    RT_CALLRA(0x24a1e8, guest::uiArenaSetTexture(param_3,0x2a9050,0,0));
    RT_CALLRA(0x24a1f4, guest::uiArenaDataTag(param_3,1));
    (*(int32_t*)rtguest::G((uint32_t)aiStack_120 + (uint32_t)(2) * 4)) = (uVar3 & 0xf) * 9 + 0x90;
    uVar3 = (int)uVar3 >> 4;
    (*(int32_t*)rtguest::G((uint32_t)aiStack_120 + (uint32_t)(1) * 4)) = 0x4a;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_114)) = 0x10;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_110)) = 9;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_10c)) = 0xc;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_108)) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_104)) = 0;
    (*(int32_t*)rtguest::G((uint32_t)aiStack_120 + (uint32_t)(0) * 4)) = iVar5;
    RT_CALLRA(0x24a250, guest::uiArenaSprites(param_3,aiStack_120,1,uVar1,uVar2,1));
    iVar5 = iVar5 + -9;
  } while (-1 < iVar6);
  iVar5 = (int)param_3;
  if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)iStack_90)) + 0x28))) & 0x8000) == 0) {
    iVar6 = RT_CALLRA(0x24a28c, guest::CalculateTachometerValue(puVar7,0,0));
    fVar8 = (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x1000))) + (rtguest::fdiv((float)iVar6, (*(float*)rtguest::G(0x332f58)))) * 0.125f;
    (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x1000))) = fVar8;
  }
  else {
    fVar8 = (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x1000)));
  }
  RT_CALLRA(0x24a2ec, guest::uiArenaSetTexture(param_3,0x2a9050,0,0));
  uVar9 = 0xc0400000;
  uVar10 = 0x41700000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_100)) = 0x46;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_f8)) = 0x90;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_f4)) = 0x3d;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_f0)) = 0x12;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_ec)) = 6;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_e0)) = 0xc0400000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_dc)) = 0xc0400000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_d4)) = 0x3f800000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_d0)) = 0x41700000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_cc)) = 0xc0400000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_c4)) = 0x3f800000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_c0)) = 0xc0400000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_bc)) = 0x40400000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_b4)) = 0x3f800000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_b0)) = 0x41700000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_ac)) = 0x40400000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_a4)) = 0x3f800000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_fc)) = 0x28;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_e8)) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_e4)) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_d8)) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_c8)) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_b8)) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_a8)) = 0;
  (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(auStack_a0)), 0, 0x10));
  (*(float*)rtguest::G((uint32_t)fStack_98)) = fVar8 + -0.5f;
  RT_CALLRA(0x24a3b4, guest::uiArenaRotQuads(param_3,uStack_100,1,uVar1,uVar2,1));
  iVar5 = RT_CALLRA(0x24a3c4, guest::CalculateBoostGaugeValue(puVar7,0,1));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_fc)) = 0x46;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_f8)) = 0x90;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_f4)) = 0x3d;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_f0)) = 0x12;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_ec)) = 6;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_d4)) = 0x3f800000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_c4)) = 0x3f800000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_bc)) = 0x40400000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_b4)) = 0x3f800000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_ac)) = 0x40400000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_a4)) = 0x3f800000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_100)) = 0x27;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_e8)) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_e4)) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_d8)) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_c8)) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_b8)) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_a8)) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_e0)) = uVar9;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_dc)) = uVar9;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_d0)) = uVar10;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_cc)) = uVar9;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_c0)) = uVar9;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_b0)) = uVar10;
  (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(auStack_a0)), 0, 0x10));
  (*(float*)rtguest::G((uint32_t)fStack_98)) = 0.75f - (float)iVar5 * 3.8146973e-06f;
  RT_CALLRA(0x24a484, guest::uiArenaRotQuads(param_3,uStack_100,1,uVar1,uVar2,1));
  iVar5 = RT_CALLRA(0x24a48c, guest::GetVehicleRaceTime((*(int32_t*)rtguest::G((uint32_t)iStack_90))));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_100)) = 0x46;
  fVar8 = rtguest::fdiv((float)iVar5, (*(float*)rtguest::G(0x332f5c)));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_f4)) = 0x3d;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_f0)) = 0x12;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_ec)) = 6;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_f8)) = 0x90;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_d4)) = 0x3f800000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_c4)) = 0x3f800000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_bc)) = 0x40400000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_b4)) = 0x3f800000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_ac)) = 0x40400000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_a4)) = 0x3f800000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_fc)) = 0x65;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_e8)) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_e4)) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_d8)) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_c8)) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_b8)) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_a8)) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_e0)) = uVar9;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_dc)) = uVar9;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_d0)) = uVar10;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_cc)) = uVar9;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_c0)) = uVar9;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_b0)) = uVar10;
  (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(auStack_a0)), 0, 0x10));
  (*(float*)rtguest::G((uint32_t)fStack_98)) = fVar8 + fVar8 + -0.5f;
  RT_CALLRA(0x24a554, guest::uiArenaRotQuads(param_3,uStack_100,1,uVar1,uVar2,1));
  iVar5 = RT_CALLRA(0x24a564, guest::CalculateSpeedometerValue(puVar7,0,0));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_fc)) = 0x46;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_f8)) = 0x90;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_ec)) = 10;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_100)) = 0x46;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_d4)) = 0x3f800000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_c4)) = 0x3f800000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_b4)) = 0x3f800000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_a4)) = 0x3f800000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_f4)) = 0x30;
  fVar8 = (rtguest::fdiv((float)iVar5, (*(float*)rtguest::G(0x332f60)))) * 1.5f + (*(float*)rtguest::G(0x332f64));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_f0)) = 0x39;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_e8)) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_e4)) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_e0)) = 0xc0a00000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_dc)) = 0xc0a00000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_d8)) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_d0)) = 0x42500000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_cc)) = 0xc0a00000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_c8)) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_c0)) = 0xc0a00000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_bc)) = 0x40a00000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_b8)) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_b0)) = 0x42500000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_ac)) = 0x40a00000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_a8)) = 0;
  (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(auStack_a0)), 0, 0x10));
  (*(float*)rtguest::G((uint32_t)fStack_98)) = fVar8;
  RT_CALLRA(0x24a650, guest::uiArenaRotQuads(param_3,uStack_100,1,uVar1,uVar2,1));
  RT_CALLRA(0x24a658, guest::uiArenaSubmit(param_3));
  return 0;
}


RT_GUEST_FN(0x00249fa0, guest::FUN_00249fa0)

}  // namespace guest
