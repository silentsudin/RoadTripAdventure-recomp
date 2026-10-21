// guest:0x0024b3a0 transliterated from Ghidra pseudocode, hand-fixed (build/scratch/w13-orphan/REPORT.md)
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t FUN_0024b3a0(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, uint32_t param_5, uint32_t param_6, ...) {
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
  int iVar6;
  int iVar7;
uint32_t puVar8;
  
  iVar6 = (param_4 + param_5) * 8;
  puVar8 = 0x177ac50 + param_5 * 0x270;  // &gs_controls_car_object (undefined8[]) + 0x4e elements = 0x270 bytes
  uVar1 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x2a9260)));
  uVar2 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x2a9264)));
  (*(int32_t*)rtguest::G((uint32_t)iStack_60)) = param_6;
  RT_CALLRA(0x24b420, guest::uiArenaInit(param_3,0x1000));
  RT_CALLRA(0x24b430, guest::uiArenaBeginBatch(param_3,0,0));
  RT_CALLRA(0x24b444, guest::uiArenaSetTexture(param_3,0x2a9278,3,0));
  RT_CALLRA(0x24b450, guest::uiArenaDataTag(param_3,1));
  (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = 0x9500000005;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) = 0;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x240000008c;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0;
  RT_CALLRA(0x24b4b4, guest::uiArenaSprites(param_3,uStack_80,1,uVar1,uVar2,1));
  iVar6 = RT_CALLRA(0x24b4c4, guest::CalculateBoostGaugeValue(puVar8,0xf,1));
  RT_CALLRA(0x24b4dc, guest::uiArenaSetTexture(param_3,0x2a9278,2,0xf - iVar6));
  RT_CALLRA(0x24b4e8, guest::uiArenaDataTag(param_3,1));
  (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = 0x7000000a0;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) = 0x200000000;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x8a00000024;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0;
  RT_CALLRA(0x24b54c, guest::uiArenaSprites(param_3,uStack_80,1,uVar1,uVar2,1));
  lVar4 = (int32_t)RT_CALLRA(0x24b55c, guest::CalculateTachometerValue(puVar8,10,1));
  if (lVar4 != 0) {
    RT_CALLRA(0x24b578, guest::uiArenaSetTexture(param_3,0x2a9278,1,0));
    RT_CALLRA(0x24b584, guest::uiArenaDataTag(param_3,lVar4));
    RT_CALLRA(0x24b5a4, guest::uiArenaSprites(param_3,0x2a92b8,lVar4,uVar1,uVar2,1));
  }
  RT_CALLRA(0x24b5b8, guest::uiArenaSetTexture(param_3,0x2a9278,0,0));
  RT_CALLRA(0x24b5c8, guest::uiArenaDataTag(param_3,4));
  RT_CALLRA(0x24b5e8, guest::uiArenaSprites(param_3,0x2a93f8,4,uVar1,uVar2,1));
  uVar3 = RT_CALLRA(0x24b5f0, guest::CalculateSpeedometerDigits(puVar8));
  iVar7 = 0;
  iVar6 = 0x6a;
  do {
    RT_CALLRA(0x24b614, guest::uiArenaSetTexture(param_3,0x2a9278,0,0));
    RT_CALLRA(0x24b620, guest::uiArenaDataTag(param_3,1));
    uVar5 = uVar3 & 0xf;
    uVar3 = (int)uVar3 >> 4;
    (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = CONCAT44(0xa4,iVar6);
    iVar6 = iVar6 + -0xc;
    (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) = CONCAT44(0x71,uVar5 * 0xc + 0xc);
    (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0xe0000000c;
    (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0;
    RT_CALLRA(0x24b684, guest::uiArenaSprites(param_3,uStack_80,1,uVar1,uVar2,1));
    if (uVar3 == 0) break;
    iVar7 = iVar7 + 1;
  } while (iVar7 < 3);
  RT_CALLRA(0x24b6b0, guest::uiArenaSetTexture(param_3,0x2a9278,0,6));
  RT_CALLRA(0x24b6bc, guest::uiArenaDataTag(param_3,1));
  (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = 0x5800000060;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) = 0x4800000098;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x3600000036;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0;
  RT_CALLRA(0x24b720, guest::uiArenaSprites(param_3,uStack_80,1,uVar1,uVar2,1));
  if ((*(int32_t*)rtguest::G((uint32_t)iStack_60)) != 0) {
    lVar4 = (int32_t)RT_CALLRA(0x24b738, guest::GetVehicleTransmissionMode(puVar8,0,0));
    if (lVar4 == 1) {
      RT_CALLRA(0x24b760, guest::uiArenaDataTag(param_3,1));
      (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = 0x5800000073;
      (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) = 0x28000000a8;
      (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0xf00000010;
      (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0;
      RT_CALLRA(0x24b7c4, guest::uiArenaSprites(param_3,uStack_80,1,uVar1,uVar2,1));
    }
    else if ((lVar4 < 2) && (lVar4 == 0)) {
      RT_CALLRA(0x24b7d0, guest::uiArenaDataTag(param_3,1));
      (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = 0x6b00000060;
      (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) = 0x38000000a8;
      (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x100000000f;
      (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0;
      RT_CALLRA(0x24b834, guest::uiArenaSprites(param_3,uStack_80,1,uVar1,uVar2,1));
      RT_CALLRA(0x24b84c, guest::uiArenaDataTag(param_3,1));
      (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = 0x6b00000087;
      (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) = 0x38000000b8;
      (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x100000000f;
      (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0;
      RT_CALLRA(0x24b8b0, guest::uiArenaSprites(param_3,uStack_80,1,uVar1,uVar2,1));
    }
    else {
      RT_CALLRA(0x24b8c0, guest::uiArenaDataTag(param_3,1));
      (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = 0x7f00000073;
      (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) = 0x28000000b8;
      (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0xf00000010;
      (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0;
      RT_CALLRA(0x24b924, guest::uiArenaSprites(param_3,uStack_80,1,uVar1,uVar2,1));
    }
  }
  RT_CALLRA(0x24b940, guest::uiArenaSetTexture(param_3,0x2a9278,0,10));
  RT_CALLRA(0x24b94c, guest::uiArenaDataTag(param_3,1));
  (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = 0x970000009c;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) = 0x4000000078;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x1d0000001d;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0;
  RT_CALLRA(0x24b9b0, guest::uiArenaSprites(param_3,uStack_80,1,uVar1,uVar2,1));
  iVar6 = RT_CALLRA(0x24b9c0, guest::GetVehicleGear(puVar8,0,1));
  RT_CALLRA(0x24b9d8, guest::uiArenaSetTexture(param_3,0x2a9278,0,8));
  RT_CALLRA(0x24b9e4, guest::uiArenaDataTag(param_3,1));
  (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = 0xa0000000a3;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) = CONCAT44(0x40,iVar6 << 4);
  (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0xa00000010;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0;
  RT_CALLRA(0x24ba38, guest::uiArenaSprites(param_3,uStack_80,1,uVar1,uVar2,1));
  RT_CALLRA(0x24ba40, guest::uiArenaSubmit(param_3));
  return 0;
}


RT_GUEST_FN(0x0024b3a0, guest::FUN_0024b3a0)

}  // namespace guest
