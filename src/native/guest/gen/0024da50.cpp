// guest:0x0024da50 transliterated from Ghidra pseudocode, hand-fixed (build/scratch/w13-orphan/REPORT.md)
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t FUN_0024da50(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, uint32_t param_5, uint32_t param_6, ...) {
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
  uint uVar3;
  uint uVar4;
  int iVar5;
  long lVar6;
  int iVar7;
  uint uVar8;
  int iVar9;
uint32_t puStack_58;
  
  iVar7 = (param_4 + param_5) * 8;
  iVar9 = 0;
  puStack_58 = 0x177ac50 + param_5 * 0x270;  // &gs_controls_car_object (undefined8[]) + 0x4e elements = 0x270 bytes
  uVar1 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x2a9ab8)));
  uVar2 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x2a9abc)));
  (*(int32_t*)rtguest::G((uint32_t)iStack_60)) = param_1;
  (*(int32_t*)rtguest::G((uint32_t)iStack_5c)) = param_6;
  RT_CALLRA(0x24dae0, guest::uiArenaInit(param_3,0x1000));
  RT_CALLRA(0x24dafc, guest::uiArenaBeginBatch(param_3,0,0));
  RT_CALLRA(0x24db10, guest::uiArenaSetTexture(param_3,0x2a9ad0,0,0));
  RT_CALLRA(0x24db1c, guest::uiArenaDataTag(param_3,1));
  (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = 4;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) = 0x68;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x8000000098;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0;
  RT_CALLRA(0x24db80, guest::uiArenaSprites(param_3,uStack_80,1,uVar1,uVar2,1));
  RT_CALLRA(0x24db94, guest::uiArenaSetTexture(param_3,0x2a9ad0,1,6));
  RT_CALLRA(0x24dba0, guest::uiArenaDataTag(param_3,1));
  (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = 0x700000002c;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) = 0x6900000000;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x140000006e;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0;
  RT_CALLRA(0x24dc04, guest::uiArenaSprites(param_3,uStack_80,1,uVar1,uVar2,1));
  RT_CALLRA(0x24dc18, guest::uiArenaSetTexture(param_3,0x2a9ad0,1,5));
  RT_CALLRA(0x24dc24, guest::uiArenaDataTag(param_3,1));
  (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = 0x7200000065;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) = 0x2000000030;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x1000000010;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0;
  RT_CALLRA(0x24dc88, guest::uiArenaSprites(param_3,uStack_80,1,uVar1,uVar2,1));
  uVar3 = RT_CALLRA(0x24dc90, guest::CalculateSpeedometerDigits(puStack_58));
  iVar7 = 0x51;
  do {
    RT_CALLRA(0x24dca8, guest::uiArenaSetTexture(param_3,0x2a9ad0,1,5));
    RT_CALLRA(0x24dcb4, guest::uiArenaDataTag(param_3,1));
    uVar4 = (uVar3 & 0xf) + 1;
    uVar8 = uVar4 >> 2;
    uVar3 = (int)uVar3 >> 4;
    (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) = CONCAT44(uVar8 << 4,(uVar4 + uVar8 * -4) * 0x10);
    (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = CONCAT44(0x72,iVar7);
    (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x1000000010;
    (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0;
    RT_CALLRA(0x24dd14, guest::uiArenaSprites(param_3,uStack_80,1,uVar1,uVar2,1));
    if (uVar3 == 0) break;
    iVar9 = iVar9 + 1;
    iVar7 = iVar7 + -0xe;
  } while (iVar9 < 3);
  lVar6 = (int32_t)RT_CALLRA(0x24dd3c, guest::GetVehicleGear(puStack_58,0,1));
  iVar9 = (uint32_t)(lVar6 != 0) + (int)lVar6;
  RT_CALLRA(0x24dd58, guest::uiArenaSetTexture(param_3,0x2a9ad0,1,5));
  RT_CALLRA(0x24dd64, guest::uiArenaDataTag(param_3,1));
  iVar7 = iVar9;
  if (iVar9 < 0) {
    iVar7 = iVar9 + 3;
  }
  (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = 0x7200000078;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) = CONCAT44((iVar7 >> 2) << 4,(iVar9 + (iVar7 >> 2) * -4) * 0x10);
  (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x1000000010;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0;
  RT_CALLRA(0x24ddc8, guest::uiArenaSprites(param_3,uStack_80,1,uVar1,uVar2,1));
  if ((*(int32_t*)rtguest::G((uint32_t)iStack_5c)) != 0) {
    lVar6 = (int32_t)RT_CALLRA(0x24dde0, guest::GetVehicleTransmissionMode(puStack_58,0,0));
    if (lVar6 == 1) {
      RT_CALLRA(0x24de1c, guest::uiArenaSetTexture(param_3,0x2a9ad0,0,0));
      RT_CALLRA(0x24de28, guest::uiArenaDataTag(param_3,1));
      (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = 0x80000000a;
      (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) = 0x4f;
      (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x1000000010;
      (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0;
      RT_CALLRA(0x24de8c, guest::uiArenaSprites(param_3,uStack_80,1,uVar1,uVar2,1));
    }
    else if ((1 < lVar6) || (lVar6 != 0)) {
      RT_CALLRA(0x24deac, guest::uiArenaSetTexture(param_3,0x2a9ad0,0,0));
      RT_CALLRA(0x24deb8, guest::uiArenaDataTag(param_3,1));
      (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = 0x1b0000000a;
      (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) = 0x130000004f;
      (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x1000000010;
      (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0;
      RT_CALLRA(0x24df1c, guest::uiArenaSprites(param_3,uStack_80,1,uVar1,uVar2,1));
    }
  }
  iVar7 = RT_CALLRA(0x24df2c, guest::CalculateTachometerValue(puStack_58,0xe,1));
  if (iVar7 != 0) {
    iVar9 = 0;
    RT_CALLRA(0x24df50, guest::uiArenaSetTexture(param_3,0x2a9ad0,1,4));
    RT_CALLRA(0x24df5c, guest::uiArenaDataTag(param_3,iVar7));
    if (0 < iVar7) {
      iVar5 = 0;
      do {
        iVar9 = iVar9 + 1;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = CONCAT44(0x6c - iVar5,0x38);
        (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) = CONCAT44(0x68 - iVar5,0xa0);
        (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x800000060;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0;
        RT_CALLRA(0x24dfc8, guest::uiArenaSprites(param_3,uStack_80,1,uVar1,uVar2,1));
        iVar5 = iVar9 * 8;
      } while (iVar9 < iVar7);
    }
  }
  iVar7 = RT_CALLRA(0x24dfe4, guest::CalculateBoostGaugeValue(puStack_58,0xd,1));
  if (iVar7 == 0) {
    iVar7 = 0xd;
    RT_CALLRA(0x24e004, guest::uiArenaSetTexture(param_3,0x2a9ad0,1,((*(int32_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)iStack_60)) + 8))) >> 1) % 3 + 1));
  }
  else {
    RT_CALLRA(0x24e04c, guest::uiArenaSetTexture(param_3,0x2a9ad0,1,0));
  }
  RT_CALLRA(0x24e058, guest::uiArenaDataTag(param_3,iVar7));
  iVar9 = 0;
  if (0 < iVar7) {
    iVar5 = 0;
    do {
      iVar9 = iVar9 + 1;
      (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = CONCAT44(100 - iVar5,0x20);
      (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) = CONCAT44(0x60 - iVar5,0x48);
      (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x800000050;
      (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0;
      RT_CALLRA(0x24e0c8, guest::uiArenaSprites(param_3,uStack_80,1,uVar1,uVar2,1));
      iVar5 = iVar9 * 8;
    } while (iVar9 < iVar7);
  }
  RT_CALLRA(0x24e0ec, guest::uiArenaSetTexture(param_3,0x2a9ad0,0,0));
  RT_CALLRA(0x24e0f8, guest::uiArenaDataTag(param_3,1));
  (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = 0x1f00000000;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) = 0;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x6e00000046;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0;
  RT_CALLRA(0x24e15c, guest::uiArenaSprites(param_3,uStack_80,1,uVar1,uVar2,1));
  RT_CALLRA(0x24e164, guest::uiArenaSubmit(param_3));
  return 0;
}


RT_GUEST_FN(0x0024da50, guest::FUN_0024da50)

}  // namespace guest
