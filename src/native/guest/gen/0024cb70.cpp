// guest:0x0024cb70 transliterated from Ghidra pseudocode, hand-fixed (build/scratch/w13-orphan/REPORT.md)
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t FUN_0024cb70(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, uint32_t param_5, uint32_t param_6, ...) {
  rtguest::FrameGuard rt_fg(128);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x20) = (uint32_t)(int32_t)param_1;
    *(uint32_t*)rtguest::G(rt_sv+0x24) = (uint32_t)(int32_t)param_6;
    *(uint32_t*)rtguest::G(rt_sv+0x78) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x7c) = 0x00000000u;
  }
  const uint32_t rt_sp = rtguest::frameTop();
    uint32_t iStack_5c = rt_sp + (0x80 - 0x5c);  // sp+0x24
    uint32_t iStack_60 = rt_sp + (0x80 - 0x60);  // sp+0x20
    uint32_t uStack_68 = rt_sp + (0x80 - 0x68);  // sp+0x18
    uint32_t uStack_70 = rt_sp + (0x80 - 0x70);  // sp+0x10
    uint32_t uStack_78 = rt_sp + (0x80 - 0x78);  // sp+0x8
    uint32_t uStack_80 = rt_sp + (0x80 - 0x80);  // sp+0x0
  undefined4 uVar1;
  undefined4 uVar2;
  int iVar3;
  uint uVar4;
  uint uVar5;
  int iVar6;
  long lVar7;
  int iVar8;
uint32_t puStack_58;
  
  iVar6 = (param_4 + param_5) * 8;
  puStack_58 = 0x177ac50 + param_5 * 0x270;  // &gs_controls_car_object (undefined8[]) + 0x4e elements = 0x270 bytes
  uVar1 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x2a9778)));
  uVar2 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x2a977c)));
  (*(int32_t*)rtguest::G((uint32_t)iStack_60)) = param_1;
  (*(int32_t*)rtguest::G((uint32_t)iStack_5c)) = param_6;
  RT_CALLRA(0x24cbf4, guest::uiArenaInit(param_3,0x1000));
  RT_CALLRA(0x24cc04, guest::uiArenaBeginBatch(param_3,0,0));
  RT_CALLRA(0x24cc1c, guest::uiArenaSetTexture(param_3,0x2a9790,0,0));
  RT_CALLRA(0x24cc28, guest::uiArenaDataTag(param_3,1));
  (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(uStack_80)), 0, 0x20));
  (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x80000000a8;
  RT_CALLRA(0x24cc64, guest::uiArenaSprites(param_3,uStack_80,1,uVar1,uVar2,1));
  iVar6 = RT_CALLRA(0x24cc74, guest::CalculateTachometerValue(puStack_58,0,1));
  if (iVar6 / 1000 != 0) {
    iVar8 = (int)param_3;
    if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) & 0x8000) == 0) {
      iVar6 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x1000))) + iVar6 / 1000;
      (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x1000))) = iVar6;
    }
    else {
      iVar6 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x1000)));
    }
    RT_CALLRA(0x24ccf8, guest::uiArenaSetTexture(param_3,0x2a9790,0,0));
    RT_CALLRA(0x24cd04, guest::uiArenaDataTag(param_3,1));
    RT_CALLRA(0x24cd2c, guest::uiArenaSprites(param_3,((iVar6 / 0x14) % 5) * 0x20 + 0x2a97b0,1,uVar1,uVar2,1));
  }
  iVar6 = RT_CALLRA(0x24cd3c, guest::CalculateBoostGaugeValue(puStack_58,0x10,1));
  if (0 < 0x10 - iVar6) {
    RT_CALLRA(0x24cd64, guest::uiArenaSetTexture(param_3,0x2a9790,1,1));
    RT_CALLRA(0x24cd74, guest::uiArenaDataTag(param_3,0x10 - iVar6));
    for (iVar8 = iVar6; iVar8 < 0x10; iVar8 = iVar8 + 1) {
      (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = CONCAT44(iVar8 * -4 + 0x3f,0x12);
      (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) = (uint64_t)(uint32_t)(iVar8 * -4 + 0x3c) << 0x20;
      (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x400000078;
      (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0;
      RT_CALLRA(0x24cddc, guest::uiArenaSprites(param_3,uStack_80,1,uVar1,uVar2,1));
    }
  }
  if (iVar6 < 1) {
    RT_CALLRA(0x24ce0c, guest::uiArenaSetTexture(param_3,0x2a9790,1,((*(int32_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)iStack_60)) + 8))) >> 3) % 3 + 0xb));
    RT_CALLRA(0x24ce18, guest::uiArenaDataTag(param_3,1));
    (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = 0xa00000034;
    (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) = 0xa0;
    (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x1800000018;
    (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0;
    RT_CALLRA(0x24ce84, guest::uiArenaSprites(param_3,uStack_80,1,uVar1,uVar2,1));
  }
  else {
    iVar8 = 0;
    RT_CALLRA(0x24cecc, guest::uiArenaSetTexture(param_3,0x2a9790,1,0));
    RT_CALLRA(0x24ced8, guest::uiArenaDataTag(param_3,iVar6));
    if (0 < iVar6) {
      iVar3 = 0;
      do {
        iVar8 = iVar8 + 1;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = CONCAT44(0x3f - iVar3,0x12);
        (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) = (uint64_t)(uint32_t)(0x3c - iVar3) << 0x20;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x400000078;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0;
        RT_CALLRA(0x24cf3c, guest::uiArenaSprites(param_3,uStack_80,1,uVar1,uVar2,1));
        iVar3 = iVar8 * 4;
      } while (iVar8 < iVar6);
    }
  }
  iVar6 = 0;
  uVar4 = RT_CALLRA(0x24cf50, guest::CalculateSpeedometerDigits(puStack_58));
  iVar8 = 0x74;
  do {
    RT_CALLRA(0x24cf78, guest::uiArenaSetTexture(param_3,0x2a9790,1,9));
    RT_CALLRA(0x24cf84, guest::uiArenaDataTag(param_3,1));
    uVar5 = uVar4 & 0xf;
    uVar4 = (int)uVar4 >> 4;
    (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = CONCAT44(0x3f,iVar8);
    iVar8 = iVar8 + -0x15;
    (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) = CONCAT44(0x46,uVar5 * 0x18);
    (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x1a00000018;
    (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0;
    RT_CALLRA(0x24cfe0, guest::uiArenaSprites(param_3,uStack_80,1,uVar1,uVar2,1));
    if (uVar4 == 0) break;
    iVar6 = iVar6 + 1;
  } while (iVar6 < 3);
  iVar6 = RT_CALLRA(0x24d008, guest::GetVehicleGear(puStack_58,0,1));
  RT_CALLRA(0x24d020, guest::uiArenaSetTexture(param_3,0x2a9790,1,10));
  RT_CALLRA(0x24d02c, guest::uiArenaDataTag(param_3,1));
  lVar7 = 0;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = 0x5100000014;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) = CONCAT44(0x60,iVar6 << 5);
  (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x2000000020;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0;
  RT_CALLRA(0x24d080, guest::uiArenaSprites(param_3,uStack_80,1,uVar1,uVar2,1));
  if ((*(int32_t*)rtguest::G((uint32_t)iStack_5c)) != 0) {
    lVar7 = (int32_t)RT_CALLRA(0x24d098, guest::GetVehicleTransmissionMode(puStack_58,0,0));
  }
  if (lVar7 == 1) {
    RT_CALLRA(0x24d0cc, guest::uiArenaSetTexture(param_3,0x2a9790,1,((*(int32_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)iStack_60)) + 8))) >> 3 & 1U) + 6));
    RT_CALLRA(0x24d0d8, guest::uiArenaDataTag(param_3,1));
    (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = 0xe0000008d;
    (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) = 0x80;
    (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x2000000018;
    (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0;
    RT_CALLRA(0x24d13c, guest::uiArenaSprites(param_3,uStack_80,1,uVar1,uVar2,1));
    RT_CALLRA(0x24d184, guest::uiArenaSetTexture(param_3,0x2a9790,1,5));
    RT_CALLRA(0x24d190, guest::uiArenaDataTag(param_3,1));
  }
  else {
    if ((1 < lVar7) || (lVar7 != 0)) {
      RT_CALLRA(0x24d208, guest::uiArenaSetTexture(param_3,0x2a9790,1,5));
      RT_CALLRA(0x24d214, guest::uiArenaDataTag(param_3,1));
      (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = 0xe0000008d;
      (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) = 0x80;
      (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x2000000018;
      (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0;
      RT_CALLRA(0x24d1f4, guest::uiArenaSprites(param_3,uStack_80,1,uVar1,uVar2,1));
      RT_CALLRA(0x24d298, guest::uiArenaSetTexture(param_3,0x2a9790,1,((*(int32_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)iStack_60)) + 8))) >> 3 & 1U) + 6));
      RT_CALLRA(0x24d2a4, guest::uiArenaDataTag(param_3,1));
      (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = 0x500000008d;
      (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) = 0x80;
      (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x2000000018;
      (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0x200000000;
      RT_CALLRA(0x24d278, guest::uiArenaSprites(param_3,uStack_80,1,uVar1,uVar2,1));
      goto LAB_0024d3a4;
    }
    RT_CALLRA(0x24d32c, guest::uiArenaSetTexture(param_3,0x2a9790,1,5));
    RT_CALLRA(0x24d338, guest::uiArenaDataTag(param_3,2));
    (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = 0xe0000008d;
    (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) = 0x80;
    (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x2000000018;
    (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0;
    RT_CALLRA(0x24d308, guest::uiArenaSprites(param_3,uStack_80,1,uVar1,uVar2,1));
  }
  (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = 0x500000008d;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) = 0x80;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x2000000018;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0x200000000;
  RT_CALLRA(0x24d39c, guest::uiArenaSprites(param_3,uStack_80,1,uVar1,uVar2,1));
LAB_0024d3a4:
  RT_CALLRA(0x24d3a4, guest::uiArenaSubmit(param_3));
  return 0;
}


RT_GUEST_FN(0x0024cb70, guest::FUN_0024cb70)

}  // namespace guest
