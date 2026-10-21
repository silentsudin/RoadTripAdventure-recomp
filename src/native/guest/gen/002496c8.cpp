// guest:0x002496c8 transliterated from Ghidra pseudocode, hand-fixed (build/scratch/w13-orphan/REPORT.md)
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t FUN_002496c8(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, uint32_t param_5, uint32_t param_6, uint32_t param_7, ...) {
  rtguest::FrameGuard rt_fg(256);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0xa0, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0xa8, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0xb8, 8);
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0xc0, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x90) = (uint32_t)(int32_t)param_2;
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0xb0, 8);
    rtguest::rtSpillS(rt_fg, 5, rt_sv, 0xc8, 8);
    rtguest::rtSpillS(rt_fg, 6, rt_sv, 0xd0, 8);
    rtguest::rtSpillS(rt_fg, 7, rt_sv, 0xd8, 8);
    rtguest::rtSpillS(rt_fg, 8, rt_sv, 0xe0, 8);
    *(uint32_t*)rtguest::G(rt_sv+0xe8) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0xec) = 0x00000000u;
    rtguest::rtSpillF(rt_fg, 4, rt_sv, 0xf0);
    *(uint32_t*)rtguest::G(rt_sv+0x94) = (uint32_t)(int32_t)param_6;
    rtguest::setS32(0, (uint32_t)param_1);  // walk: = param_1
    rtguest::gS[1] = 0x0000000000000000ull;  // walk const
    rtguest::setS32(4, (uint32_t)param_3);  // walk: = param_3
  }
  const uint32_t rt_sp = rtguest::frameTop();
    uint32_t fStack_78 = rt_sp + (0x100 - 0x78);  // sp+0x88
    uint32_t iStack_68 = rt_sp + (0x100 - 0x68);  // sp+0x98
    uint32_t iStack_6c = rt_sp + (0x100 - 0x6c);  // sp+0x94
    uint32_t uStack_100 = rt_sp + (0x100 - 0x100);  // sp+0x0
    uint32_t uStack_70 = rt_sp + (0x100 - 0x70);  // sp+0x90
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
    uint32_t uStack_e0 = rt_sp + (0x100 - 0xe0);  // sp+0x20
    uint32_t uStack_e8 = rt_sp + (0x100 - 0xe8);  // sp+0x18
    uint32_t uStack_f0 = rt_sp + (0x100 - 0xf0);  // sp+0x10
    uint32_t uStack_f8 = rt_sp + (0x100 - 0xf8);  // sp+0x8
  undefined4 uVar1;
  undefined4 uVar2;
  bool bVar3;
uint32_t puVar4;
uint32_t puVar5;
uint32_t puVar6;
  uint uVar7;
  long lVar8;
uint32_t puVar9;
  int iVar10;
  undefined8 uVar11;
  undefined8 uVar12;
  undefined8 uVar13;
  uint uVar14;
  int iVar15;
  int iVar16;
  float fVar17;
    uint32_t auStack_80 = rt_sp + (0x100 - 0x80);  // sp+0x80
uint32_t puStack_64;
  
  iVar10 = (param_4 + param_5) * 8;
  puStack_64 = 0x177ac50 + param_5 * 0x270;  // &gs_controls_car_object (undefined8[]) + 0x4e elements = 0x270 bytes
  bVar3 = false;
  uVar1 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x2a8ff0)));
  uVar2 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x2a8ff4)));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_70)) = param_2;
  (*(int32_t*)rtguest::G((uint32_t)iStack_6c)) = param_6;
  (*(int32_t*)rtguest::G((uint32_t)iStack_68)) = param_7;
  RT_CALLRA(0x249758, guest::uiArenaInit(param_3,0x1000));
  RT_CALLRA(0x249768, guest::uiArenaBeginBatch(param_3,0,0));
  lVar8 = (int32_t)RT_CALLRA(0x249778, guest::CalculateBoostGaugeValue(puStack_64,0,1));
  if (lVar8 < 0x18000) {
    bVar3 = ((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) & 0x20) != 0;
  }
  uVar11 = 3;
  if (!bVar3) {
    uVar11 = 0;
  }
  RT_CALLRA(0x2497b8, guest::uiArenaSetTexture(param_3,0x2a9008,1,uVar11));
  puVar4 = uStack_e0;
  puVar5 = 0x00327820;
  do {
    puVar6 = puVar5;
    puVar9 = puVar4;
    uVar11 = (*(uint64_t*)rtguest::G((uint32_t)puVar6 + (uint32_t)(1) * sizeof(uint64_t)));
    uVar12 = (*(uint64_t*)rtguest::G((uint32_t)puVar6 + (uint32_t)(2) * sizeof(uint64_t)));
    uVar13 = (*(uint64_t*)rtguest::G((uint32_t)puVar6 + (uint32_t)(3) * sizeof(uint64_t)));
    (*(uint64_t*)rtguest::G((uint32_t)puVar9)) = (*(uint64_t*)rtguest::G((uint32_t)puVar6));
    (*(uint64_t*)rtguest::G((uint32_t)puVar9 + (uint32_t)(1) * sizeof(uint64_t))) = uVar11;
    (*(uint64_t*)rtguest::G((uint32_t)puVar9 + (uint32_t)(2) * sizeof(uint64_t))) = uVar12;
    (*(uint64_t*)rtguest::G((uint32_t)puVar9 + (uint32_t)(3) * sizeof(uint64_t))) = uVar13;
    puVar4 = puVar9 + 0x20;  // undefined8* element step: 4*8
    puVar5 = puVar6 + 0x20;  // undefined8* element step: 4*8
  } while (puVar6 + 0x20 != (uint32_t)0x00327880);
  uVar11 = (*(uint64_t*)rtguest::G((uint32_t)puVar6 + (uint32_t)(5) * sizeof(uint64_t)));
  (*(uint64_t*)rtguest::G((uint32_t)puVar9 + (uint32_t)(4) * sizeof(uint64_t))) = 0;
  (*(uint64_t*)rtguest::G((uint32_t)puVar9 + (uint32_t)(5) * sizeof(uint64_t))) = uVar11;
  RT_CALLRA(0x249830, guest::uiArenaRotQuads(param_3,uStack_e0,1,uVar1,uVar2,1));
  (*(float*)rtguest::G((uint32_t)fStack_78)) = -(float)(int)lVar8 * 3.8146973e-06f * (*(float*)rtguest::G(0x332f4c));
  if (lVar8 == 0) {
    uVar11 = 5;
  }
  else {
    uVar11 = 2;
    if (bVar3) {
      uVar11 = 4;
    }
  }
  RT_CALLRA(0x249880, guest::uiArenaSetTexture(param_3,0x2a9008,1,uVar11));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_d8)) = 0x40;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_e0 + 4)) = 0x46;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_d4)) = 0x30;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_e0 + 0)) = 0x46;
  iVar16 = 0;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_d0)) = 0x14;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_cc)) = 0x3c;
  iVar15 = 0x59;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_c8)) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_c4)) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_c0)) = 0x42380000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_bc)) = 0xc1f00000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_b8)) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_b4)) = 0x3f800000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_ac)) = 0xc1f00000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_b0)) = 0x42840000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_a8)) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_a4)) = 0x3f800000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_a0)) = 0x42380000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_9c)) = 0x41f00000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_98)) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_94)) = 0x3f800000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_90)) = 0x42840000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_8c)) = 0x41f00000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_84)) = 0x3f800000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_88)) = 0;
  (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(auStack_80)), 0, 0x10));
  RT_CALLRA(0x249968, guest::uiArenaRotQuads(param_3,uStack_e0,1,uVar1,uVar2,1));
  RT_CALLRA(0x24997c, guest::uiArenaSetTexture(param_3,0x2a9008,0,0));
  RT_CALLRA(0x249988, guest::uiArenaDataTag(param_3,1));
  (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(uStack_100)), 0, 0x20));
  (*(uint64_t*)rtguest::G((uint32_t)uStack_f0)) = 0x800000008c;
  RT_CALLRA(0x2499c4, guest::uiArenaSprites(param_3,uStack_100,1,uVar1,uVar2,1));
  iVar10 = RT_CALLRA(0x2499d4, guest::GetVehicleGear(puStack_64,0,0));
  RT_CALLRA(0x2499ec, guest::uiArenaSetTexture(param_3,0x2a9008,0,0));
  RT_CALLRA(0x2499f8, guest::uiArenaDataTag(param_3,1));
  (*(uint64_t*)rtguest::G((uint32_t)uStack_100)) = 0x3c00000061;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_f8)) = CONCAT44((iVar10 / 5) * 0x10 + 0x50,(iVar10 % 5) * 0x14 + 0x8c);
  (*(uint64_t*)rtguest::G((uint32_t)uStack_f0)) = 0xe00000014;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_e8)) = 0;
  RT_CALLRA(0x249a74, guest::uiArenaSprites(param_3,uStack_100,1,uVar1,uVar2,1));
  uVar7 = RT_CALLRA(0x249a7c, guest::CalculateSpeedometerDigits(puStack_64));
  do {
    RT_CALLRA(0x249a9c, guest::uiArenaSetTexture(param_3,0x2a9008,0,0));
    RT_CALLRA(0x249aa8, guest::uiArenaDataTag(param_3,1));
    uVar14 = uVar7 & 0xf;
    uVar7 = (int)uVar7 >> 4;
    (*(uint64_t*)rtguest::G((uint32_t)uStack_100)) = CONCAT44(0x5e,iVar15);
    iVar15 = iVar15 + -0x11;
    (*(uint64_t*)rtguest::G((uint32_t)uStack_f8)) = CONCAT44((uVar14 / 7) * 0x17,(uVar14 % 7) * 0x10 + 0x8c);
    (*(uint64_t*)rtguest::G((uint32_t)uStack_f0)) = 0x1700000010;
    (*(uint64_t*)rtguest::G((uint32_t)uStack_e8)) = 0;
    RT_CALLRA(0x249b28, guest::uiArenaSprites(param_3,uStack_100,1,uVar1,uVar2,1));
    if (uVar7 == 0) break;
    iVar16 = iVar16 + 1;
  } while (iVar16 < 3);
  if ((*(int32_t*)rtguest::G((uint32_t)iStack_68)) != 0) {
    RT_CALLRA(0x249b60, guest::uiArenaSetTexture(param_3,0x2a9008,1,0xc));
    RT_CALLRA(0x249b70, guest::uiArenaDataTag(param_3,1));
    (*(uint64_t*)rtguest::G((uint32_t)uStack_100)) = 0x6400000014;
    (*(uint64_t*)rtguest::G((uint32_t)uStack_f8)) = 0;
    (*(uint64_t*)rtguest::G((uint32_t)uStack_f0)) = 0x260000005e;
    (*(uint64_t*)rtguest::G((uint32_t)uStack_e8)) = 0;
    RT_CALLRA(0x249bd4, guest::uiArenaSprites(param_3,uStack_100,1,uVar1,uVar2,1));
    iVar10 = RT_CALLRA(0x249be8, guest::GetRaceTimeRank((*(uint32_t*)rtguest::G((uint32_t)uStack_70))));
    RT_CALLRA(0x249c04, guest::uiArenaSetTexture(param_3,0x2a9008,0,0));
    iVar15 = 4;
    RT_CALLRA(0x249c14, guest::uiArenaDataTag(param_3,1));
    (*(uint64_t*)rtguest::G((uint32_t)uStack_100)) = 0x7100000016;
    (*(uint64_t*)rtguest::G((uint32_t)uStack_f8)) = CONCAT44(0x31,iVar10 * 0x18 + 0x8c);
    (*(uint64_t*)rtguest::G((uint32_t)uStack_f0)) = 0x1800000018;
    (*(uint64_t*)rtguest::G((uint32_t)uStack_e8)) = 0;
    RT_CALLRA(0x249c70, guest::uiArenaSprites(param_3,uStack_100,1,uVar1,uVar2,1));
    RT_CALLRA(0x249c88, guest::uiArenaSetTexture(param_3,0x2a9008,1,0xc));
    RT_CALLRA(0x249c94, guest::uiArenaDataTag(param_3,5));
    uVar7 = RT_CALLRA(0x249c9c, guest::FormatRaceTimeBCD((*(uint32_t*)rtguest::G((uint32_t)uStack_70))));
    iVar10 = 0x5b;
    do {
      uVar14 = uVar7 & 0xf;
      uVar7 = (int)uVar7 >> 4;
      iVar15 = iVar15 + -1;
      (*(uint64_t*)rtguest::G((uint32_t)uStack_100)) = CONCAT44(0x7a,iVar10);
      (*(uint64_t*)rtguest::G((uint32_t)uStack_f8)) = CONCAT44(0x28,uVar14 << 3);
      (*(uint64_t*)rtguest::G((uint32_t)uStack_f0)) = 0x800000008;
      (*(uint64_t*)rtguest::G((uint32_t)uStack_e8)) = 0;
      RT_CALLRA(0x249cf4, guest::uiArenaSprites(param_3,uStack_100,1,uVar1,uVar2,1));
      iVar10 = iVar10 + -9;
    } while (-1 < iVar15);
  }
  if ((*(int32_t*)rtguest::G((uint32_t)iStack_6c)) != 0) {
    lVar8 = (int32_t)RT_CALLRA(0x249d14, guest::GetVehicleTransmissionMode(puStack_64,0,0));
    if (lVar8 == 0) {
      uVar11 = 0xc;
    }
    else {
      uVar11 = 0xe;
      if (lVar8 != 1) {
        uVar11 = 0xd;
      }
    }
    RT_CALLRA(0x249d48, guest::uiArenaSetTexture(param_3,0x2a9008,1,uVar11));
    RT_CALLRA(0x249d54, guest::uiArenaDataTag(param_3,1));
    (*(uint64_t*)rtguest::G((uint32_t)uStack_100)) = 0x3b0000001d;
    (*(uint64_t*)rtguest::G((uint32_t)uStack_f8)) = 0x3000000010;
    (*(uint64_t*)rtguest::G((uint32_t)uStack_f0)) = 0x1b00000010;
    (*(uint64_t*)rtguest::G((uint32_t)uStack_e8)) = 0;
    RT_CALLRA(0x249db8, guest::uiArenaSprites(param_3,uStack_100,1,uVar1,uVar2,1));
  }
  iVar10 = RT_CALLRA(0x249dd0, guest::CalculateTachometerValue(puStack_64,0,1));
  fVar17 = rtguest::fdiv((float)iVar10, (*(float*)rtguest::G(0x332f50))) + (*(float*)rtguest::G(0x332f54));
  RT_CALLRA(0x249e08, guest::uiArenaSetTexture(param_3,0x2a9008,1,0xc));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_e0 + 4)) = 0x44;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_d8)) = 3;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_d4)) = 0x35;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_d0)) = 8;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_cc)) = 0x3b;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_e0 + 0)) = 0x46;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_c8)) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_c4)) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_c0)) = 0xc0800000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_bc)) = 0xc26c0000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_b8)) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_b4)) = 0x3f800000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_ac)) = 0xc26c0000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_b0)) = 0x40800000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_a8)) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_a4)) = 0x3f800000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_a0)) = 0xc0800000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_9c)) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_98)) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_94)) = 0x3f800000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_90)) = 0x40800000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_84)) = 0x3f800000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_8c)) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_88)) = 0;
  (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(auStack_80)), 0, 0x10));
  (*(float*)rtguest::G((uint32_t)fStack_78)) = fVar17;
  RT_CALLRA(0x249ed8, guest::uiArenaRotQuads(param_3,uStack_e0,1,uVar1,uVar2,1));
  RT_CALLRA(0x249eec, guest::uiArenaSetTexture(param_3,0x2a9008,1,0xc));
  RT_CALLRA(0x249ef8, guest::uiArenaDataTag(param_3,1));
  (*(uint64_t*)rtguest::G((uint32_t)uStack_100)) = 0x4000000040;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_f8)) = 0x5000000010;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_f0)) = 0xc0000000c;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_e8)) = 0;
  RT_CALLRA(0x249f5c, guest::uiArenaSprites(param_3,uStack_100,1,uVar1,uVar2,1));
  RT_CALLRA(0x249f64, guest::uiArenaSubmit(param_3));
  return 0;
}


RT_GUEST_FN(0x002496c8, guest::FUN_002496c8)

}  // namespace guest
