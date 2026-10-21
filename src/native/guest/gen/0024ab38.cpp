// guest:0x0024ab38 transliterated from Ghidra pseudocode, hand-fixed (build/scratch/w13-orphan/REPORT.md)
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t FUN_0024ab38(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, uint32_t param_5, uint32_t param_6, uint32_t param_7, ...) {
  rtguest::FrameGuard rt_fg(256);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0xb0, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x90) = (uint32_t)(int32_t)param_2;
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0xa0, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0xa8, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0xb8, 8);
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0xc0, 8);
    rtguest::rtSpillS(rt_fg, 5, rt_sv, 0xc8, 8);
    rtguest::rtSpillS(rt_fg, 6, rt_sv, 0xd0, 8);
    rtguest::rtSpillS(rt_fg, 7, rt_sv, 0xd8, 8);
    rtguest::rtSpillS(rt_fg, 8, rt_sv, 0xe0, 8);
    *(uint32_t*)rtguest::G(rt_sv+0xe8) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0xec) = 0x00000000u;
    rtguest::rtSpillF(rt_fg, 4, rt_sv, 0xf0);
    *(uint32_t*)rtguest::G(rt_sv+0x94) = (uint32_t)(int32_t)param_6;
    rtguest::setS32(2, (uint32_t)param_3);  // walk: = param_3
  }
  const uint32_t rt_sp = rtguest::frameTop();
    uint32_t fStack_78 = rt_sp + (0x100 - 0x78);  // sp+0x88
    uint32_t iStack_68 = rt_sp + (0x100 - 0x68);  // sp+0x98
    uint32_t iStack_6c = rt_sp + (0x100 - 0x6c);  // sp+0x94
    uint32_t iStack_70 = rt_sp + (0x100 - 0x70);  // sp+0x90
    uint32_t uStack_100 = rt_sp + (0x100 - 0x100);  // sp+0x0
    uint32_t uStack_84 = rt_sp + (0x100 - 0x84);  // sp+0x7c
    uint32_t uStack_88 = rt_sp + (0x100 - 0x88);  // sp+0x78
    uint32_t uStack_8c = rt_sp + (0x100 - 0x8c);  // sp+0x74
    uint32_t uStack_90 = rt_sp + (0x100 - 0x90);  // sp+0x70
    uint32_t uStack_94 = rt_sp + (0x100 - 0x94);  // sp+0x6c
    uint32_t uStack_98 = rt_sp + (0x100 - 0x98);  // sp+0x68
    uint32_t uStack_9c = rt_sp + (0x100 - 0x9c);  // sp+0x64
    uint32_t uStack_a0 = rt_sp + (0x100 - 0xa0);  // sp+0x60
    uint32_t uStack_a4 = rt_sp + (0x100 - 0xa4);  // sp+0x5c
    uint32_t uStack_a8 = rt_sp + (0x100 - 0xa8);  // sp+0x58
    uint32_t uStack_ac = rt_sp + (0x100 - 0xac);  // sp+0x54
    uint32_t uStack_b0 = rt_sp + (0x100 - 0xb0);  // sp+0x50
    uint32_t uStack_b4 = rt_sp + (0x100 - 0xb4);  // sp+0x4c
    uint32_t uStack_b8 = rt_sp + (0x100 - 0xb8);  // sp+0x48
    uint32_t uStack_bc = rt_sp + (0x100 - 0xbc);  // sp+0x44
    uint32_t uStack_c0 = rt_sp + (0x100 - 0xc0);  // sp+0x40
    uint32_t uStack_c4 = rt_sp + (0x100 - 0xc4);  // sp+0x3c
    uint32_t uStack_c8 = rt_sp + (0x100 - 0xc8);  // sp+0x38
    uint32_t uStack_cc = rt_sp + (0x100 - 0xcc);  // sp+0x34
    uint32_t uStack_d0 = rt_sp + (0x100 - 0xd0);  // sp+0x30
    uint32_t uStack_d4 = rt_sp + (0x100 - 0xd4);  // sp+0x2c
    uint32_t uStack_d8 = rt_sp + (0x100 - 0xd8);  // sp+0x28
    uint32_t uStack_dc = rt_sp + (0x100 - 0xdc);  // sp+0x24
    uint32_t uStack_e0 = rt_sp + (0x100 - 0xe0);  // sp+0x20
    uint32_t uStack_e8 = rt_sp + (0x100 - 0xe8);  // sp+0x18
    uint32_t uStack_f0 = rt_sp + (0x100 - 0xf0);  // sp+0x10
    uint32_t uStack_f8 = rt_sp + (0x100 - 0xf8);  // sp+0x8
  undefined4 uVar1;
  undefined4 uVar2;
  float fVar3;
uint32_t puVar4;
uint32_t puVar5;
uint32_t puVar6;
  uint uVar7;
  uint uVar8;
  long lVar9;
uint32_t puVar10;
  undefined8 uVar11;
  int iVar12;
  undefined8 uVar13;
  undefined8 uVar14;
  int iVar15;
    uint32_t auStack_80 = rt_sp + (0x100 - 0x80);  // sp+0x80
uint32_t puStack_64;
  
  iVar12 = (param_4 + param_5) * 8;
  puStack_64 = 0x177ac50 + param_5 * 0x270;  // &gs_controls_car_object (undefined8[]) + 0x4e elements = 0x270 bytes
  uVar1 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x2a9218)));
  uVar2 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x2a921c)));
  (*(int32_t*)rtguest::G((uint32_t)iStack_70)) = param_2;
  (*(int32_t*)rtguest::G((uint32_t)iStack_6c)) = param_6;
  (*(int32_t*)rtguest::G((uint32_t)iStack_68)) = param_7;
  RT_CALLRA(0x24abc0, guest::uiArenaInit(param_3,0x1000));
  RT_CALLRA(0x24abd0, guest::uiArenaBeginBatch(param_3,0,0));
  lVar9 = (int32_t)RT_CALLRA(0x24abd8, guest::GetRaceTimeRank((*(int32_t*)rtguest::G((uint32_t)iStack_70))));
  if (lVar9 == 0) {
    RT_CALLRA(0x24abf8, guest::uiArenaSetTexture(param_3,0x2a9230,2,0));
    RT_CALLRA(0x24ac0c, guest::uiArenaSetColour(param_3,0x80ffffff));
    RT_CALLRA(0x24ac18, guest::uiArenaDataTag(param_3,1));
    (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(uStack_100)), 0, 0x20));
    (*(uint64_t*)rtguest::G((uint32_t)uStack_f0)) = 0x8000000084;
    RT_CALLRA(0x24ac54, guest::uiArenaSprites(param_3,uStack_100,1,uVar1,uVar2,1));
  }
  RT_CALLRA(0x24ac6c, guest::uiArenaSetTexture(param_3,0x2a9230,1,0xf));
  puVar4 = 0x00327910;
  puVar5 = (uint32_t)uStack_e0;
  do {
    puVar10 = puVar5;
    puVar6 = puVar4;
    uVar11 = (*(uint64_t*)rtguest::G((uint32_t)puVar6 + (uint32_t)(1) * sizeof(uint64_t)));
    uVar13 = (*(uint64_t*)rtguest::G((uint32_t)puVar6 + (uint32_t)(2) * sizeof(uint64_t)));
    uVar14 = (*(uint64_t*)rtguest::G((uint32_t)puVar6 + (uint32_t)(3) * sizeof(uint64_t)));
    (*(uint64_t*)rtguest::G((uint32_t)puVar10)) = (*(uint64_t*)rtguest::G((uint32_t)puVar6));
    (*(uint64_t*)rtguest::G((uint32_t)puVar10 + (uint32_t)(1) * sizeof(uint64_t))) = uVar11;
    (*(uint64_t*)rtguest::G((uint32_t)puVar10 + (uint32_t)(2) * sizeof(uint64_t))) = uVar13;
    (*(uint64_t*)rtguest::G((uint32_t)puVar10 + (uint32_t)(3) * sizeof(uint64_t))) = uVar14;
    puVar4 = puVar6 + 0x20;  // undefined8* element step: 4*8
    puVar5 = puVar10 + 0x20;  // undefined8* element step: 4*8
  } while (puVar6 + 0x20 != (uint32_t)0x00327970);
  uVar11 = (*(uint64_t*)rtguest::G((uint32_t)puVar6 + (uint32_t)(5) * sizeof(uint64_t)));
  (*(uint64_t*)rtguest::G((uint32_t)puVar10 + (uint32_t)(4) * sizeof(uint64_t))) = 0;
  (*(uint64_t*)rtguest::G((uint32_t)puVar10 + (uint32_t)(5) * sizeof(uint64_t))) = uVar11;
  RT_CALLRA(0x24ace0, guest::uiArenaRotQuads(param_3,uStack_e0,1,uVar1,uVar2,1));
  fVar3 = (*(float*)rtguest::G(0x332f70));
  lVar9 = (int32_t)RT_CALLRA(0x24acf4, guest::CalculateBoostGaugeValue(puStack_64,0,1));
  RT_CALLRA(0x24ad34, guest::uiArenaSetTexture(param_3,0x2a9230,1,4));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_d4)) = 0x48;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_d0)) = 0x58;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_cc)) = 0x28;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_e0)) = 0x3a;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_dc)) = 0x3f;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_d8)) = 0xa0;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_c8)) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_c4)) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_c0)) = 0xc2300000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_bc)) = 0xc2640000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_b8)) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_b4)) = 0x3f800000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_ac)) = 0xc2640000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_b0)) = 0x42300000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_a8)) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_a4)) = 0x3f800000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_a0)) = 0xc2300000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_9c)) = 0xc1880000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_98)) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_94)) = 0x3f800000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_90)) = 0x42300000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_8c)) = 0xc1880000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_84)) = 0x3f800000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_88)) = 0;
  (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(auStack_80)), 0, 0x10));
  (*(float*)rtguest::G((uint32_t)fStack_78)) = fVar3 - (float)(int)lVar9 * 3.8146973e-06f * fVar3;
  RT_CALLRA(0x24ae04, guest::uiArenaRotQuads(param_3,uStack_e0,1,uVar1,uVar2,1));
  if (lVar9 == 0) {
    RT_CALLRA(0x24ae20, guest::uiArenaSetTexture(param_3,0x2a9230,1,0xf));
    RT_CALLRA(0x24ae2c, guest::uiArenaDataTag(param_3,1));
    (*(uint64_t*)rtguest::G((uint32_t)uStack_100)) = 0x90000002e;
    (*(uint64_t*)rtguest::G((uint32_t)uStack_f8)) = 0x3800000060;
    (*(uint64_t*)rtguest::G((uint32_t)uStack_f0)) = 0x1200000018;
    (*(uint64_t*)rtguest::G((uint32_t)uStack_e8)) = 0;
    RT_CALLRA(0x24ae90, guest::uiArenaSprites(param_3,uStack_100,1,uVar1,uVar2,1));
  }
  lVar9 = (int32_t)RT_CALLRA(0x24aea0, guest::CalculateTachometerValue(puStack_64,0,1));
  if (lVar9 == 0) {
    iVar12 = 5;
  }
  else {
    iVar12 = (int)param_3;
    if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)iStack_70)) + 0x28))) & 0x8000) == 0) {
      iVar15 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x1000))) + (int)lVar9 / 1000;
      (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x1000))) = iVar15;
    }
    else {
      iVar15 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x1000)));
    }
    iVar12 = (iVar15 >> 4) % 9 + 6;
  }
  RT_CALLRA(0x24af28, guest::uiArenaSetTexture(param_3,0x2a9230,1,iVar12));
  RT_CALLRA(0x24af38, guest::uiArenaDataTag(param_3,1));
  (*(uint64_t*)rtguest::G((uint32_t)uStack_100)) = 0x1e00000018;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_f8)) = 0xa8;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_f0)) = 0x4400000044;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_e8)) = 0;
  RT_CALLRA(0x24af9c, guest::uiArenaSprites(param_3,uStack_100,1,uVar1,uVar2,1));
  RT_CALLRA(0x24afb0, guest::uiArenaSetTexture(param_3,0x2a9230,0,1));
  iVar15 = 0;
  RT_CALLRA(0x24afc0, guest::uiArenaDataTag(param_3,1));
  (*(uint64_t*)rtguest::G((uint32_t)uStack_100)) = 3;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_f8)) = 0x84;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_f0)) = 0x800000007c;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_e8)) = 0;
  RT_CALLRA(0x24b024, guest::uiArenaSprites(param_3,uStack_100,1,uVar1,uVar2,1));
  iVar12 = RT_CALLRA(0x24b038, guest::GetVehicleGear(puStack_64,0,0));
  RT_CALLRA(0x24b054, guest::uiArenaSetTexture(param_3,0x2a9230,1,1));
  RT_CALLRA(0x24b060, guest::uiArenaDataTag(param_3,1));
  (*(uint64_t*)rtguest::G((uint32_t)uStack_100)) = 0x5c00000056;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_f0)) = 0x1000000018;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_f8)) = CONCAT44(0x18,iVar12 * 0x18);
  (*(uint64_t*)rtguest::G((uint32_t)uStack_e8)) = 0;
  RT_CALLRA(0x24b0bc, guest::uiArenaSprites(param_3,uStack_100,1,uVar1,uVar2,1));
  uVar7 = RT_CALLRA(0x24b0c4, guest::CalculateSpeedometerDigits(puStack_64));
  iVar12 = 0x42;
  do {
    RT_CALLRA(0x24b0e0, guest::uiArenaSetTexture(param_3,0x2a9230,1,0));
    RT_CALLRA(0x24b0ec, guest::uiArenaDataTag(param_3,1));
    uVar8 = uVar7 & 0xf;
    uVar7 = (int)uVar7 >> 4;
    (*(uint64_t*)rtguest::G((uint32_t)uStack_100)) = CONCAT44(0x2c,iVar12);
    (*(uint64_t*)rtguest::G((uint32_t)uStack_f8)) = (uint32_t)(uVar8 << 4);
    (*(uint64_t*)rtguest::G((uint32_t)uStack_f0)) = 0x1600000010;
    (*(uint64_t*)rtguest::G((uint32_t)uStack_e8)) = 0;
    RT_CALLRA(0x24b13c, guest::uiArenaSprites(param_3,uStack_100,1,uVar1,uVar2,1));
    if (uVar7 == 0) break;
    iVar15 = iVar15 + 1;
    iVar12 = iVar12 + -0x10;
  } while (iVar15 < 3);
  if ((*(int32_t*)rtguest::G((uint32_t)iStack_68)) != 0) {
    RT_CALLRA(0x24b178, guest::uiArenaSetTexture(param_3,0x2a9230,1,2));
    RT_CALLRA(0x24b188, guest::uiArenaDataTag(param_3,5));
    iVar15 = 4;
    uVar7 = RT_CALLRA(0x24b198, guest::FormatRaceTimeBCD((*(int32_t*)rtguest::G((uint32_t)iStack_70))));
    iVar12 = 0x33;
    do {
      uVar8 = uVar7 & 0xf;
      uVar7 = (int)uVar7 >> 4;
      iVar15 = iVar15 + -1;
      (*(uint64_t*)rtguest::G((uint32_t)uStack_100)) = CONCAT44(0x54,iVar12);
      (*(uint64_t*)rtguest::G((uint32_t)uStack_f8)) = CONCAT44(0x28,uVar8 << 3);
      (*(uint64_t*)rtguest::G((uint32_t)uStack_f0)) = 0xa00000008;
      (*(uint64_t*)rtguest::G((uint32_t)uStack_e8)) = 0;
      RT_CALLRA(0x24b1f0, guest::uiArenaSprites(param_3,uStack_100,1,uVar1,uVar2,1));
      iVar12 = iVar12 + -8;
    } while (-1 < iVar15);
  }
  if ((*(int32_t*)rtguest::G((uint32_t)iStack_6c)) != 0) {
    lVar9 = (int32_t)RT_CALLRA(0x24b210, guest::GetVehicleTransmissionMode(puStack_64,0,0));
    RT_CALLRA(0x24b22c, guest::uiArenaSetTexture(param_3,0x2a9230,1,3));
    RT_CALLRA(0x24b238, guest::uiArenaDataTag(param_3,1));
    if (lVar9 == 1) {
      (*(uint64_t*)rtguest::G((uint32_t)uStack_100)) = 0x230000005d;
      (*(uint64_t*)rtguest::G((uint32_t)uStack_f8)) = 0x2800000080;
    }
    else if ((lVar9 < 2) && (lVar9 == 0)) {
      (*(uint64_t*)rtguest::G((uint32_t)uStack_100)) = 0x3100000069;
      (*(uint64_t*)rtguest::G((uint32_t)uStack_f8)) = 0x4000000080;
    }
    else {
      (*(uint64_t*)rtguest::G((uint32_t)uStack_100)) = 0x430000005d;
      (*(uint64_t*)rtguest::G((uint32_t)uStack_f8)) = 0x5800000080;
    }
    (*(uint64_t*)rtguest::G((uint32_t)uStack_e8)) = 0;
    (*(uint64_t*)rtguest::G((uint32_t)uStack_f0)) = 0x1800000020;
    RT_CALLRA(0x24b358, guest::uiArenaSprites(param_3,uStack_100,1,uVar1,uVar2,1));
  }
  RT_CALLRA(0x24b360, guest::uiArenaSubmit(param_3));
  return 0;
}


RT_GUEST_FN(0x0024ab38, guest::FUN_0024ab38)

}  // namespace guest
