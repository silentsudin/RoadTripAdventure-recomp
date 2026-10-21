// guest:0x0024c320 transliterated from Ghidra pseudocode, hand-fixed (build/scratch/w13-orphan/REPORT.md)
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t FUN_0024c320(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, uint32_t param_5, uint32_t param_6, ...) {
  rtguest::FrameGuard rt_fg(288);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0xe8) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0xec) = 0x00000000u;
    *(uint32_t*)rtguest::G(rt_sv+0x90) = (uint32_t)(int32_t)param_6;
  }
  const uint32_t rt_sp = rtguest::frameTop();
    uint32_t fStack_98 = rt_sp + (0x120 - 0x98);  // sp+0x88
    uint32_t iStack_90 = rt_sp + (0x120 - 0x90);  // sp+0x90
    uint32_t uStack_100 = rt_sp + (0x120 - 0x100);  // sp+0x20
    uint32_t uStack_108 = rt_sp + (0x120 - 0x108);  // sp+0x18
    uint32_t uStack_110 = rt_sp + (0x120 - 0x110);  // sp+0x10
    uint32_t uStack_118 = rt_sp + (0x120 - 0x118);  // sp+0x8
    uint32_t uStack_120 = rt_sp + (0x120 - 0x120);  // sp+0x0
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
    uint32_t uStack_f0 = rt_sp + (0x120 - 0xf0);  // sp+0x30
  undefined4 uVar1;
  undefined4 uVar2;
uint32_t puVar3;
uint32_t puVar4;
uint32_t puVar5;
  uint uVar6;
  long lVar7;
  long lVar8;
uint32_t puVar9;
  uint uVar10;
  undefined8 uVar11;
  int iVar12;
  undefined8 uVar13;
  undefined8 uVar14;
  int iVar15;
uint32_t puVar16;
  undefined4 uVar17;
  float fVar18;
  undefined4 uVar19;
    uint32_t aiStack_f8 = rt_sp + (0x120 - 0xf8);  // sp+0x28
    uint32_t auStack_a0 = rt_sp + (0x120 - 0xa0);  // sp+0x80
  
  iVar12 = (param_4 + param_5) * 8;
  puVar16 = 0x177ac50 + param_5 * 0x270;  // &gs_controls_car_object (undefined8[]) + 0x4e elements = 0x270 bytes
  uVar1 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x2a9660)));
  uVar2 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x2a9664)));
  (*(int32_t*)rtguest::G((uint32_t)iStack_90)) = param_6;
  lVar7 = (int32_t)RT_CALLRA(0x24c3b4, guest::GetVehicleGear(puVar16,0,0));
  RT_CALLRA(0x24c3c4, guest::uiArenaInit(param_3,0x1000));
  RT_CALLRA(0x24c3d4, guest::uiArenaBeginBatch(param_3,0,0));
  RT_CALLRA(0x24c3ec, guest::uiArenaSetTexture(param_3,0x2a9678,1,3));
  puVar3 = uStack_100;
  puVar4 = 0x00327cc0;
  do {
    puVar5 = puVar4;
    puVar9 = puVar3;
    uVar11 = (*(uint64_t*)rtguest::G((uint32_t)puVar5 + (uint32_t)(1) * sizeof(uint64_t)));
    uVar13 = (*(uint64_t*)rtguest::G((uint32_t)puVar5 + (uint32_t)(2) * sizeof(uint64_t)));
    uVar14 = (*(uint64_t*)rtguest::G((uint32_t)puVar5 + (uint32_t)(3) * sizeof(uint64_t)));
    (*(uint64_t*)rtguest::G((uint32_t)puVar9)) = (*(uint64_t*)rtguest::G((uint32_t)puVar5));
    (*(uint64_t*)rtguest::G((uint32_t)puVar9 + (uint32_t)(1) * sizeof(uint64_t))) = uVar11;
    (*(uint64_t*)rtguest::G((uint32_t)puVar9 + (uint32_t)(2) * sizeof(uint64_t))) = uVar13;
    (*(uint64_t*)rtguest::G((uint32_t)puVar9 + (uint32_t)(3) * sizeof(uint64_t))) = uVar14;
    puVar3 = puVar9 + 0x20;  // undefined8* element step: 4*8
    puVar4 = puVar5 + 0x20;  // undefined8* element step: 4*8
  } while (puVar5 + 0x20 != (uint32_t)0x00327d20);
  uVar11 = (*(uint64_t*)rtguest::G((uint32_t)puVar5 + (uint32_t)(5) * sizeof(uint64_t)));
  (*(uint64_t*)rtguest::G((uint32_t)puVar9 + (uint32_t)(4) * sizeof(uint64_t))) = 0;
  (*(uint64_t*)rtguest::G((uint32_t)puVar9 + (uint32_t)(5) * sizeof(uint64_t))) = uVar11;
  RT_CALLRA(0x24c464, guest::uiArenaRotQuads(param_3,uStack_100,1,uVar1,uVar2,1));
  lVar8 = (int32_t)RT_CALLRA(0x24c474, guest::CalculateBoostGaugeValue(puVar16,0,1));
  fVar18 = (float)(int)lVar8 * 3.8146973e-06f * (*(float*)rtguest::G(0x332f74));
  RT_CALLRA(0x24c4b4, guest::uiArenaSetTexture(param_3,0x2a9678,1,2));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_100 + 0)) = 0x40;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_100 + 4)) = 0x40;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_f0 + 4)) = 0x4c;
  (*(int32_t*)rtguest::G((uint32_t)aiStack_f8 + (uint32_t)(0) * 4)) = 0x8c;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_f0 + 0)) = 0x5e;
  (*(int32_t*)rtguest::G((uint32_t)aiStack_f8 + (uint32_t)(1) * 4)) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_e8)) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_e4)) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_e0)) = 0xc2680000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_dc)) = 0xc2680000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_d8)) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_d4)) = 0x3f800000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_d0)) = 0x42100000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_cc)) = 0xc2680000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_c8)) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_c4)) = 0x3f800000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_c0)) = 0xc2680000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_bc)) = 0x41900000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_b8)) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_b4)) = 0x3f800000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_b0)) = 0x42100000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_ac)) = 0x41900000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_a4)) = 0x3f800000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_a8)) = 0;
  (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(auStack_a0)), 0, 0x10));
  (*(float*)rtguest::G((uint32_t)fStack_98)) = fVar18;
  RT_CALLRA(0x24c57c, guest::uiArenaRotQuads(param_3,uStack_100,1,uVar1,uVar2,1));
  RT_CALLRA(0x24c590, guest::uiArenaSetTexture(param_3,0x2a9678,1,0));
  RT_CALLRA(0x24c59c, guest::uiArenaDataTag(param_3,1));
  if (lVar8 == 0) {
    (*(uint64_t*)rtguest::G((uint32_t)uStack_120)) = 0x800000009;
    (*(uint64_t*)rtguest::G((uint32_t)uStack_118)) = 0;
    (*(uint64_t*)rtguest::G((uint32_t)uStack_110)) = 0x3800000040;
  }
  else {
    (*(uint64_t*)rtguest::G((uint32_t)uStack_120)) = 0x80000000a;
    (*(uint64_t*)rtguest::G((uint32_t)uStack_118)) = 0x48;
    (*(uint64_t*)rtguest::G((uint32_t)uStack_110)) = 0x3200000038;
  }
  (*(uint64_t*)rtguest::G((uint32_t)uStack_108)) = 0;
  RT_CALLRA(0x24c658, guest::uiArenaSprites(param_3,uStack_120,1,uVar1,uVar2,1));
  RT_CALLRA(0x24c670, guest::uiArenaSetTexture(param_3,0x2a9678,0,0));
  RT_CALLRA(0x24c67c, guest::uiArenaDataTag(param_3,1));
  (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(uStack_120)), 0, 0x20));
  (*(uint64_t*)rtguest::G((uint32_t)uStack_110)) = 0x8000000084;
  RT_CALLRA(0x24c6b8, guest::uiArenaSprites(param_3,uStack_120,1,uVar1,uVar2,1));
  iVar12 = RT_CALLRA(0x24c6c8, guest::CalculateTachometerValue(puVar16,0,0));
  if (lVar7 == 0) {
    iVar12 = -iVar12;
  }
  iVar15 = (int)param_3;
  if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) & 0x8000) == 0) {
    fVar18 = (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0x1000))) + ((float)iVar12 / (*(float*)rtguest::G(0x332f78))) * 0.125f;
    (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0x1000))) = fVar18;
  }
  else {
    fVar18 = (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0x1000)));
  }
  RT_CALLRA(0x24c73c, guest::uiArenaSetTexture(param_3,0x2a9678,0,0));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_100 + 4)) = 0x41;
  (*(int32_t*)rtguest::G((uint32_t)aiStack_f8 + (uint32_t)(1) * 4)) = 0x17;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_f0 + 4)) = 0x54;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_100 + 0)) = 0x41;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_f0 + 0)) = 0x54;
  (*(int32_t*)rtguest::G((uint32_t)aiStack_f8 + (uint32_t)(0) * 4)) = 0x8f;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_e8)) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_e4)) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_e0)) = 0xc2280000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_dc)) = 0xc2280000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_d8)) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_d4)) = 0x3f800000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_d0)) = 0x42280000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_cc)) = 0xc2280000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_c8)) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_c4)) = 0x3f800000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_c0)) = 0xc2280000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_bc)) = 0x42280000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_b8)) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_b4)) = 0x3f800000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_ac)) = 0x42280000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_a4)) = 0x3f800000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_b0)) = 0x42280000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_a8)) = 0;
  (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(auStack_a0)), 0, 0x10));
  (*(float*)rtguest::G((uint32_t)fStack_98)) = fVar18;
  RT_CALLRA(0x24c7fc, guest::uiArenaRotQuads(param_3,uStack_100,1,uVar1,uVar2,1));
  RT_CALLRA(0x24c810, guest::uiArenaSetTexture(param_3,0x2a9678,0,0));
  if (lVar7 == 0) {
    RT_CALLRA(0x24c820, guest::uiArenaDataTag(param_3,1));
    RT_CALLRA(0x24c850, guest::uiArenaSprites(param_3,0x2a9758,1,uVar1,uVar2,1));
  }
  else {
    RT_CALLRA(0x24c860, guest::uiArenaDataTag(param_3,lVar7));
    RT_CALLRA(0x24c880, guest::uiArenaSprites(param_3,(6 - (int)lVar7) * 0x20 + 0x2a9698,lVar7,uVar1,uVar2,1));
  }
  uVar6 = RT_CALLRA(0x24c890, guest::CalculateSpeedometerDigits(puVar16));
  uVar19 = 0x42280000;
  iVar12 = 0;
  RT_CALLRA(0x24c8c8, guest::uiArenaSetTexture(param_3,0x2a9678,1,1));
  uVar17 = 0x42580000;
  do {
    uVar10 = uVar6 & 0xf;
    uVar6 = (int)uVar6 >> 4;
    (*(int32_t*)rtguest::G((uint32_t)aiStack_f8 + (uint32_t)(0) * 4)) = uVar10 * 0xc;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_f0 + 4)) = 0x10;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_100 + 0)) = 0x41;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_100 + 4)) = 0x41;
    (*(int32_t*)rtguest::G((uint32_t)aiStack_f8 + (uint32_t)(1) * 4)) = 0x40;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_f0 + 0)) = 0xc;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_e8)) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_e4)) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_e0)) = 0xc1000000;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_d8)) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_d4)) = 0x3f800000;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_d0)) = 0x41000000;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_c8)) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_c4)) = 0x3f800000;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_c0)) = 0xc1000000;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_b8)) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_b4)) = 0x3f800000;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_b0)) = 0x41000000;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_a8)) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_a4)) = 0x3f800000;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_dc)) = uVar19;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_cc)) = uVar19;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_bc)) = uVar17;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_ac)) = uVar17;
    (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(auStack_a0)), 0, 0x10));
    fVar18 = (float)iVar12;
    iVar12 = iVar12 + 1;
    (*(float*)rtguest::G((uint32_t)fStack_98)) = fVar18 * (*(float*)rtguest::G(0x332f7c)) + (*(float*)rtguest::G(0x332f80));
    RT_CALLRA(0x24c9ac, guest::uiArenaRotQuads(param_3,uStack_100,1,uVar1,uVar2,1));
    if (uVar6 == 0) break;
  } while (iVar12 < 3);
  if ((*(int32_t*)rtguest::G((uint32_t)iStack_90)) != 0) {
    RT_CALLRA(0x24c9dc, guest::uiArenaSetTexture(param_3,0x2a9678,0,0));
    RT_CALLRA(0x24c9e8, guest::uiArenaDataTag(param_3,1));
    lVar7 = (int32_t)RT_CALLRA(0x24c9f8, guest::GetVehicleTransmissionMode(puVar16,0,0));
    if (lVar7 == 1) {
      (*(uint64_t*)rtguest::G((uint32_t)uStack_120)) = 0x170000005b;
      (*(uint64_t*)rtguest::G((uint32_t)uStack_118)) = 0x1000000e7;
      (*(uint64_t*)rtguest::G((uint32_t)uStack_110)) = 0x1600000016;
    }
    else if ((lVar7 < 2) && (lVar7 == 0)) {
      (*(uint64_t*)rtguest::G((uint32_t)uStack_120)) = 0x2700000064;
      (*(uint64_t*)rtguest::G((uint32_t)uStack_118)) = 0x19000000e9;
      (*(uint64_t*)rtguest::G((uint32_t)uStack_110)) = 0x1600000016;
    }
    else {
      (*(uint64_t*)rtguest::G((uint32_t)uStack_120)) = 0x3c00000067;
      (*(uint64_t*)rtguest::G((uint32_t)uStack_118)) = 0x32000000ec;
      (*(uint64_t*)rtguest::G((uint32_t)uStack_110)) = 0x1400000014;
    }
    (*(uint64_t*)rtguest::G((uint32_t)uStack_108)) = 0;
    RT_CALLRA(0x24cb18, guest::uiArenaSprites(param_3,uStack_120,1,uVar1,uVar2,1));
  }
  RT_CALLRA(0x24cb20, guest::uiArenaSubmit(param_3));
  return 0;
}


RT_GUEST_FN(0x0024c320, guest::FUN_0024c320)

}  // namespace guest
