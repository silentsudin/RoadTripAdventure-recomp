// guest:0x0024ba78 transliterated from Ghidra pseudocode, hand-fixed (build/scratch/w13-orphan/REPORT.md)
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t FUN_0024ba78(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, uint32_t param_5, uint32_t param_6, ...) {
  rtguest::FrameGuard rt_fg(128);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x20) = (uint32_t)(int32_t)param_6;
    *(uint32_t*)rtguest::G(rt_sv+0x78) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x7c) = 0x00000000u;
  }
  const uint32_t rt_sp = rtguest::frameTop();
    uint32_t iStack_60 = rt_sp + (0x80 - 0x60);  // sp+0x20
    uint32_t uStack_68 = rt_sp + (0x80 - 0x68);  // sp+0x18
    uint32_t uStack_70 = rt_sp + (0x80 - 0x70);  // sp+0x10
    uint32_t uStack_78 = rt_sp + (0x80 - 0x78);  // sp+0x8
    uint32_t uStack_80 = rt_sp + (0x80 - 0x80);  // sp+0x0
  undefined4 uVar1;
  undefined4 uVar2;
  uint uVar3;
  long lVar4;
  uint uVar5;
uint32_t piVar6;
  uint uVar7;
  int iVar8;
  int iVar9;
uint32_t puVar10;
  
  iVar8 = (param_4 + param_5) * 8;
  puVar10 = 0x177ac50 + param_5 * 0x270;  // &gs_controls_car_object (undefined8[]) + 0x4e elements = 0x270 bytes
  uVar1 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x2a9478)));
  uVar2 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x2a947c)));
  (*(int32_t*)rtguest::G((uint32_t)iStack_60)) = param_6;
  RT_CALLRA(0x24baf8, guest::uiArenaInit(param_3,0x1000));
  RT_CALLRA(0x24bb10, guest::uiArenaBeginBatch(param_3,0,0));
  RT_CALLRA(0x24bb24, guest::uiArenaSetTexture(param_3,0x2a9490,1,8));
  RT_CALLRA(0x24bb30, guest::uiArenaDataTag(param_3,5));
  RT_CALLRA(0x24bb50, guest::uiArenaSprites(param_3,0x2a94c0,5,uVar1,uVar2,1));
  lVar4 = (int32_t)RT_CALLRA(0x24bb60, guest::CalculateTachometerValue(puVar10,9,1));
  if (7 < lVar4) {
    lVar4 = 7;
  }
  RT_CALLRA(0x24bb80, guest::uiArenaSetTexture(param_3,0x2a9490,1,lVar4));
  RT_CALLRA(0x24bb8c, guest::uiArenaDataTag(param_3,2));
  RT_CALLRA(0x24bbac, guest::uiArenaSprites(param_3,0x2a9560,2,uVar1,uVar2,1));
  RT_CALLRA(0x24bbc0, guest::uiArenaSetTexture(param_3,0x2a9490,0,0));
  RT_CALLRA(0x24bbcc, guest::uiArenaDataTag(param_3,4));
  RT_CALLRA(0x24bbec, guest::uiArenaSprites(param_3,0x2a95a0,4,uVar1,uVar2,1));
  if (lVar4 == 7) {
    RT_CALLRA(0x24bc08, guest::uiArenaSetTexture(param_3,0x2a9490,0,0));
    RT_CALLRA(0x24bc14, guest::uiArenaDataTag(param_3,1));
    piVar6 = (uint32_t)((int)param_3 + 0x1000);
    if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) & 0x8000) == 0) {
      iVar8 = (*(int32_t*)rtguest::G((uint32_t)piVar6)) >> 1;
      (*(int32_t*)rtguest::G((uint32_t)piVar6)) = (*(int32_t*)rtguest::G((uint32_t)piVar6)) + 1;
      if (7 < iVar8) {
        (*(int32_t*)rtguest::G((uint32_t)piVar6)) = 0;
        iVar8 = 0;
      }
    }
    else {
      iVar8 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_3 + 0x1000))) >> 1;
    }
    switch(iVar8) {
    case 0:
    case 4:
      (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = 0x730000003f;
      break;
    case 1:
    case 3:
      (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = 0x6d00000054;
      break;
    case 2:
      (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = 0x6000000063;
      break;
    case 5:
    case 7:
      (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = 0x6d0000002b;
      break;
    case 6:
    default:
      (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = 0x600000001c;
    }
    (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0;
    (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x1f0000001b;
    (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) = 0x3c00000057;
    RT_CALLRA(0x24be1c, guest::uiArenaSprites(param_3,uStack_80,1,uVar1,uVar2,1));
  }
  iVar9 = 0;
  uVar3 = RT_CALLRA(0x24be28, guest::CalculateSpeedometerDigits(puVar10));
  iVar8 = 0x57;
  do {
    RT_CALLRA(0x24be50, guest::uiArenaSetTexture(param_3,0x2a9490,1,10));
    RT_CALLRA(0x24be5c, guest::uiArenaDataTag(param_3,1));
    uVar7 = uVar3 & 0xf;
    uVar5 = uVar7 >> 3;
    uVar3 = (int)uVar3 >> 4;
    (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) = CONCAT44(uVar5 * 0x18 + 0x48,(uVar7 + uVar5 * -8) * 0x14 + 0x48);
    (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = CONCAT44(0x37,iVar8);
    (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x1800000014;
    (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0;
    RT_CALLRA(0x24bed4, guest::uiArenaSprites(param_3,uStack_80,1,uVar1,uVar2,1));
    if (uVar3 == 0) break;
    iVar9 = iVar9 + 1;
    iVar8 = iVar8 + -0x14;
  } while (iVar9 < 3);
  iVar8 = RT_CALLRA(0x24befc, guest::GetVehicleGear(puVar10,0,1));
  RT_CALLRA(0x24bf14, guest::uiArenaSetTexture(param_3,0x2a9490,0,0));
  RT_CALLRA(0x24bf20, guest::uiArenaDataTag(param_3,1));
  (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = 0x1a00000041;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) = CONCAT44((iVar8 / 3) * 0x12,(iVar8 % 3) * 0x16 + 0x32);
  (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x1200000016;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0;
  RT_CALLRA(0x24bfac, guest::uiArenaSprites(param_3,uStack_80,1,uVar1,uVar2,1));
  if ((*(int32_t*)rtguest::G((uint32_t)iStack_60)) == 0) goto LAB_0024c294;
  lVar4 = (int32_t)RT_CALLRA(0x24bfc4, guest::GetVehicleTransmissionMode(puVar10,0,0));
  RT_CALLRA(0x24bfdc, guest::uiArenaSetTexture(param_3,0x2a9490,1,9));
  RT_CALLRA(0x24bfe8, guest::uiArenaDataTag(param_3,2));
  if (lVar4 != 1) {
    if (lVar4 < 2) {
      if (lVar4 == 0) {
        (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = 0x1b0000001d;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) = 0x61000000b0;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x1600000018;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0;
        RT_CALLRA(0x24c084, guest::uiArenaSprites(param_3,uStack_80,1,uVar1,uVar2,1));
        (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = 0x1b00000063;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) = 0x61000000b0;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x1600000018;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0x100000000;
        RT_CALLRA(0x24c0e8, guest::uiArenaSprites(param_3,uStack_80,1,uVar1,uVar2,1));
        goto LAB_0024c294;
      }
    }
    else if (lVar4 == 2) {
      (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = 0x1b0000001d;
      (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) = 0x6100000098;
      (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x1600000018;
      (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0;
      RT_CALLRA(0x24c154, guest::uiArenaSprites(param_3,uStack_80,1,uVar1,uVar2,1));
      (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = 0x1b00000063;
      (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) = 0x6100000098;
      (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x1600000018;
      (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0x100000000;
      RT_CALLRA(0x24c1b8, guest::uiArenaSprites(param_3,uStack_80,1,uVar1,uVar2,1));
      goto LAB_0024c294;
    }
  }
  (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = 0x1b0000001d;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) = 0x61000000c8;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x1600000018;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0;
  RT_CALLRA(0x24c224, guest::uiArenaSprites(param_3,uStack_80,1,uVar1,uVar2,1));
  (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = 0x1b00000063;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) = 0x61000000c8;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x1600000018;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0x100000000;
  RT_CALLRA(0x24c288, guest::uiArenaSprites(param_3,uStack_80,1,uVar1,uVar2,1));
LAB_0024c294:
  iVar8 = RT_CALLRA(0x24c298, guest::CalculateBoostGaugeValue(puVar10,0xb,1));
  RT_CALLRA(0x24c2b4, guest::uiArenaSetTexture(param_3,0x2a9490,2,0xb - iVar8));
  RT_CALLRA(0x24c2c0, guest::uiArenaDataTag(param_3,2));
  RT_CALLRA(0x24c2e0, guest::uiArenaSprites(param_3,0x2a9620,2,uVar1,uVar2,1));
  RT_CALLRA(0x24c2e8, guest::uiArenaSubmit(param_3));
  return 0;
}


RT_GUEST_FN(0x0024ba78, guest::FUN_0024ba78)

}  // namespace guest
