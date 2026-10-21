// guest:0x0024a6a8 transliterated from Ghidra pseudocode, hand-fixed (build/scratch/w13-orphan/REPORT.md)
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t FUN_0024a6a8(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, uint32_t param_5, uint32_t param_6, ...) {
  rtguest::FrameGuard rt_fg(256);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0xa0, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0xa8, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0xb0, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0xb8, 8);
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0xc0, 8);
    rtguest::rtSpillS(rt_fg, 7, rt_sv, 0xd8, 8);
    rtguest::rtSpillS(rt_fg, 8, rt_sv, 0xe0, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x90) = (uint32_t)(int32_t)param_6;
    rtguest::rtSpillS(rt_fg, 5, rt_sv, 0xc8, 8);
    rtguest::rtSpillS(rt_fg, 6, rt_sv, 0xd0, 8);
    *(uint32_t*)rtguest::G(rt_sv+0xe8) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0xec) = 0x00000000u;
    rtguest::rtSpillF(rt_fg, 4, rt_sv, 0xf0);
    rtguest::gS[0] = 0x00000000002a9078ull;  // walk const
    rtguest::setS32(2, (uint32_t)param_3);  // walk: = param_3
    rtguest::gS[3] = 0x0000000000000007ull;  // walk const
    rtguest::gS[4] = 0x000000000000007bull;  // walk const
    rtguest::gS[8] = 0x00000000002b0000ull;  // walk const
  }
  const uint32_t rt_sp = rtguest::frameTop();
    uint32_t iStack_70 = rt_sp + (0x100 - 0x70);  // sp+0x90
    uint32_t uStack_100 = rt_sp + (0x100 - 0x100);  // sp+0x0
    uint32_t uStack_78 = rt_sp + (0x100 - 0x78);  // sp+0x88
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
  uint uVar3;
  long lVar4;
  int iVar5;
  uint uVar6;
  int iVar7;
  int iVar8;
uint32_t puVar9;
  undefined4 uVar10;
  undefined4 uVar11;
    uint32_t auStack_80 = rt_sp + (0x100 - 0x80);  // sp+0x80
  
  iVar5 = (param_4 + param_5) * 8;
  iVar8 = 0x7b;
  puVar9 = 0x177ac50 + param_5 * 0x270;  // &gs_controls_car_object (undefined8[]) + 0x4e elements = 0x270 bytes
  uVar1 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x2a9060)));
  uVar2 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x2a9064)));
  (*(int32_t*)rtguest::G((uint32_t)iStack_70)) = param_6;
  RT_CALLRA(0x24a734, guest::uiArenaInit(param_3,0x1000));
  RT_CALLRA(0x24a744, guest::uiArenaBeginBatch(param_3,0,0));
  RT_CALLRA(0x24a758, guest::uiArenaSetTexture(param_3,0x2a9078,0,0));
  RT_CALLRA(0x24a764, guest::uiArenaDataTag(param_3,2));
  RT_CALLRA(0x24a784, guest::uiArenaSprites(param_3,0x2a9098,2,uVar1,uVar2,1));
  iVar5 = RT_CALLRA(0x24a794, guest::GetVehicleGear(puVar9,0,0));
  iVar7 = 2;
  RT_CALLRA(0x24a7b0, guest::uiArenaSetTexture(param_3,0x2a9078,0,0));
  RT_CALLRA(0x24a7bc, guest::uiArenaDataTag(param_3,1));
  (*(uint64_t*)rtguest::G((uint32_t)uStack_100)) = 0x4100000033;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_f8)) = CONCAT44(0x1e,iVar5 * 10 + 0xaa);
  (*(uint64_t*)rtguest::G((uint32_t)uStack_f0)) = 0x140000000a;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_e8)) = 0;
  RT_CALLRA(0x24a81c, guest::uiArenaSprites(param_3,uStack_100,1,uVar1,uVar2,1));
  uVar3 = RT_CALLRA(0x24a824, guest::CalculateSpeedometerDigits(puVar9));
  do {
    RT_CALLRA(0x24a840, guest::uiArenaSetTexture(param_3,0x2a9078,0,0));
    iVar7 = iVar7 + -1;
    RT_CALLRA(0x24a850, guest::uiArenaDataTag(param_3,1));
    uVar6 = uVar3 & 0xf;
    (*(uint64_t*)rtguest::G((uint32_t)uStack_100)) = CONCAT44(0x32,iVar8);
    iVar8 = iVar8 + -0x14;
    uVar3 = (int)uVar3 >> 4;
    (*(uint64_t*)rtguest::G((uint32_t)uStack_f8)) = CONCAT44((uVar6 / 7) * 0x1a,(uVar6 % 7) * 0x14 + 0x6e);
    (*(uint64_t*)rtguest::G((uint32_t)uStack_f0)) = 0x1a00000014;
    (*(uint64_t*)rtguest::G((uint32_t)uStack_e8)) = 0;
    RT_CALLRA(0x24a8dc, guest::uiArenaSprites(param_3,uStack_100,1,uVar1,uVar2,1));
  } while (-1 < iVar7);
  lVar4 = (int32_t)RT_CALLRA(0x24a8f4, guest::CalculateTachometerValue(puVar9,10,1));
  if (lVar4 != 0) {
    RT_CALLRA(0x24a910, guest::uiArenaSetTexture(param_3,0x2a9078,0,0));
    RT_CALLRA(0x24a91c, guest::uiArenaDataTag(param_3,lVar4));
    RT_CALLRA(0x24a93c, guest::uiArenaSprites(param_3,0x2a90d8,lVar4,uVar1,uVar2,1));
  }
  if ((*(int32_t*)rtguest::G((uint32_t)iStack_70)) != 0) {
    lVar4 = (int32_t)RT_CALLRA(0x24a95c, guest::GetVehicleTransmissionMode(puVar9,0,0));
    uVar10 = 0x3f000000;
    if ((lVar4 == 0) || (uVar10 = (*(uint32_t*)rtguest::G(0x332f6c)), uVar11 = (*(uint32_t*)rtguest::G(0x332f68)), lVar4 != 1)) {
      uVar11 = uVar10;
    }
    RT_CALLRA(0x24a98c, guest::uiArenaSetTexture(param_3,0x2a9078,0,0));
    (*(uint32_t*)rtguest::G((uint32_t)uStack_d4)) = 0x1e;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_d0)) = 8;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_cc)) = 0x12;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_dc)) = 0x12;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_e0)) = 0x59;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_d8)) = 0xf1;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_c8)) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_c4)) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_c0)) = 0xc0800000;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_bc)) = 0xc1700000;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_b8)) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_b4)) = 0x3f800000;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_ac)) = 0xc1700000;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_b0)) = 0x40800000;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_a8)) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_a4)) = 0x3f800000;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_a0)) = 0xc0800000;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_9c)) = 0x40400000;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_98)) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_94)) = 0x3f800000;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_90)) = 0x40800000;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_8c)) = 0x40400000;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_84)) = 0x3f800000;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_88)) = 0;
    (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(auStack_80)), 0, 0x10));
    (*(uint32_t*)rtguest::G((uint32_t)uStack_78)) = uVar11;
    RT_CALLRA(0x24aa58, guest::uiArenaRotQuads(param_3,uStack_e0,1,uVar1,uVar2,1));
  }
  iVar5 = RT_CALLRA(0x24aa68, guest::CalculateBoostGaugeValue(puVar9,0xe,1));
  RT_CALLRA(0x24aa80, guest::uiArenaSetTexture(param_3,0x2a9078,1,0xe - iVar5));
  RT_CALLRA(0x24aa8c, guest::uiArenaDataTag(param_3,1));
  (*(uint64_t*)rtguest::G((uint32_t)uStack_100)) = 0x6600000050;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_f8)) = 0;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_f0)) = 0xc0000003a;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_e8)) = 0;
  RT_CALLRA(0x24aaf0, guest::uiArenaSprites(param_3,uStack_100,1,uVar1,uVar2,1));
  RT_CALLRA(0x24aaf8, guest::uiArenaSubmit(param_3));
  return 0;
}


RT_GUEST_FN(0x0024a6a8, guest::FUN_0024a6a8)

}  // namespace guest
