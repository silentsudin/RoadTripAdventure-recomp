// guest:0x0024d3e0 transliterated from Ghidra pseudocode, hand-fixed (build/scratch/w13-orphan/REPORT.md)
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t FUN_0024d3e0(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, uint32_t param_5, uint32_t param_6, ...) {
  rtguest::FrameGuard rt_fg(160);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x50, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x58, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x60, 8);
    rtguest::rtSpillS(rt_fg, 7, rt_sv, 0x88, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x40) = (uint32_t)(int32_t)param_1;
    *(uint32_t*)rtguest::G(rt_sv+0x44) = (uint32_t)(int32_t)param_6;
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x68, 8);
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0x70, 8);
    rtguest::rtSpillS(rt_fg, 5, rt_sv, 0x78, 8);
    rtguest::rtSpillS(rt_fg, 6, rt_sv, 0x80, 8);
    rtguest::rtSpillS(rt_fg, 8, rt_sv, 0x90, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x98) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x9c) = 0x00000000u;
    rtguest::setS32(2, (uint32_t)param_3);  // walk: = param_3
  }
  const uint32_t rt_sp = rtguest::frameTop();
    uint32_t iStack_5c = rt_sp + (0xa0 - 0x5c);  // sp+0x44
    uint32_t iStack_60 = rt_sp + (0xa0 - 0x60);  // sp+0x40
    uint32_t iStack_7c = rt_sp + (0xa0 - 0x7c);  // sp+0x24
    uint32_t iStack_80 = rt_sp + (0xa0 - 0x80);  // sp+0x20
    uint32_t uStack_64 = rt_sp + (0xa0 - 0x64);  // sp+0x3c
    uint32_t uStack_68 = rt_sp + (0xa0 - 0x68);  // sp+0x38
    uint32_t uStack_6c = rt_sp + (0xa0 - 0x6c);  // sp+0x34
    uint32_t uStack_70 = rt_sp + (0xa0 - 0x70);  // sp+0x30
    uint32_t uStack_74 = rt_sp + (0xa0 - 0x74);  // sp+0x2c
    uint32_t uStack_78 = rt_sp + (0xa0 - 0x78);  // sp+0x28
    uint32_t uStack_88 = rt_sp + (0xa0 - 0x88);  // sp+0x18
    uint32_t uStack_90 = rt_sp + (0xa0 - 0x90);  // sp+0x10
    uint32_t uStack_98 = rt_sp + (0xa0 - 0x98);  // sp+0x8
    uint32_t uStack_a0 = rt_sp + (0xa0 - 0xa0);  // sp+0x0
  undefined4 uVar1;
  undefined4 uVar2;
  uint uVar3;
  uint uVar4;
  long lVar5;
  int iVar6;
  int iVar7;
uint32_t puVar8;
  
  iVar6 = (param_4 + param_5) * 8;
  puVar8 = 0x177ac50 + param_5 * 0x270;  // &gs_controls_car_object (undefined8[]) + 0x4e elements = 0x270 bytes
  uVar1 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x2a9850)));
  uVar2 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x2a9854)));
  (*(int32_t*)rtguest::G((uint32_t)iStack_60)) = param_1;
  (*(int32_t*)rtguest::G((uint32_t)iStack_5c)) = param_6;
  RT_CALLRA(0x24d45c, guest::uiArenaInit(param_3,0x1000));
  RT_CALLRA(0x24d46c, guest::uiArenaBeginBatch(param_3,0,0));
  lVar5 = (int32_t)RT_CALLRA(0x24d484, guest::CalculateBoostGaugeValue(puVar8,0xd,1));
  RT_CALLRA(0x24d4a0, guest::uiArenaSetTexture(param_3,0x2a9868,2,0xd - (int)lVar5));
  RT_CALLRA(0x24d4ac, guest::uiArenaDataTag(param_3,1));
  (*(uint64_t*)rtguest::G((uint32_t)uStack_a0)) = 0x3000000047;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_98)) = 0;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_90)) = 0x380000002e;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_88)) = 0;
  RT_CALLRA(0x24d510, guest::uiArenaSprites(param_3,uStack_a0,1,uVar1,uVar2,1));
  RT_CALLRA(0x24d524, guest::uiArenaSetTexture(param_3,0x2a9868,0,0));
  RT_CALLRA(0x24d530, guest::uiArenaDataTag(param_3,1));
  (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(uStack_a0)), 0, 0x20));
  (*(uint64_t*)rtguest::G((uint32_t)uStack_90)) = 0x8000000080;
  RT_CALLRA(0x24d568, guest::uiArenaSprites(param_3,uStack_a0,1,uVar1,uVar2,1));
  if (lVar5 == 0) {
    RT_CALLRA(0x24d584, guest::uiArenaSetTexture(param_3,0x2a9868,0,0));
    RT_CALLRA(0x24d590, guest::uiArenaDataTag(param_3,1));
    (*(uint64_t*)rtguest::G((uint32_t)uStack_a0)) = 0x4000000017;
    (*(uint64_t*)rtguest::G((uint32_t)uStack_98)) = 0x4000000080;
    (*(uint64_t*)rtguest::G((uint32_t)uStack_90)) = 0x240000002e;
    (*(uint64_t*)rtguest::G((uint32_t)uStack_88)) = 0;
    RT_CALLRA(0x24d5f4, guest::uiArenaSprites(param_3,uStack_a0,1,uVar1,uVar2,1));
  }
  uVar3 = RT_CALLRA(0x24d600, guest::CalculateSpeedometerDigits(puVar8));
  iVar7 = 0;
  iVar6 = 0x51;
  do {
    RT_CALLRA(0x24d628, guest::uiArenaSetTexture(param_3,0x2a9868,1,0));
    RT_CALLRA(0x24d634, guest::uiArenaDataTag(param_3,1));
    uVar4 = uVar3 & 0xf;
    uVar3 = (int)uVar3 >> 4;
    (*(uint64_t*)rtguest::G((uint32_t)uStack_a0)) = CONCAT44(0x14,iVar6);
    iVar6 = iVar6 + -0x10;
    (*(uint64_t*)rtguest::G((uint32_t)uStack_98)) = (uint32_t)(uVar4 << 4);
    (*(uint64_t*)rtguest::G((uint32_t)uStack_90)) = 0x1800000010;
    (*(uint64_t*)rtguest::G((uint32_t)uStack_88)) = 0;
    RT_CALLRA(0x24d688, guest::uiArenaSprites(param_3,uStack_a0,1,uVar1,uVar2,1));
    if (uVar3 == 0) break;
    iVar7 = iVar7 + 1;
  } while (iVar7 < 3);
  lVar5 = (int32_t)RT_CALLRA(0x24d6b0, guest::GetVehicleGear(puVar8,0,0));
  RT_CALLRA(0x24d6c8, guest::uiArenaSetTexture(param_3,0x2a9868,0,0));
  if (lVar5 == 0) {
    RT_CALLRA(0x24d6d8, guest::uiArenaDataTag(param_3,1));
    RT_CALLRA(0x24d6f8, guest::uiArenaSprites(param_3,0x2a9898,1,uVar1,uVar2,1));
  }
  else {
    RT_CALLRA(0x24d708, guest::uiArenaDataTag(param_3,lVar5));
    RT_CALLRA(0x24d728, guest::uiArenaSprites(param_3,0x2a98b8,lVar5,uVar1,uVar2,1));
  }
  if ((*(int32_t*)rtguest::G((uint32_t)iStack_5c)) != 0) {
    lVar5 = (int32_t)RT_CALLRA(0x24d740, guest::GetVehicleTransmissionMode(puVar8,0,0));
    if (lVar5 == 1) {
      RT_CALLRA(0x24d7a0, guest::uiArenaSetTexture(param_3,0x2a9868,1,((*(int32_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)iStack_60)) + 8))) >> 3) % 3 + 1));
      RT_CALLRA(0x24d7ac, guest::uiArenaDataTag(param_3,1));
      (*(uint64_t*)rtguest::G((uint32_t)uStack_a0)) = 0x2600000021;
      (*(uint64_t*)rtguest::G((uint32_t)uStack_98)) = 0xa8;
      (*(uint64_t*)rtguest::G((uint32_t)uStack_90)) = 0x1800000010;
      (*(uint64_t*)rtguest::G((uint32_t)uStack_88)) = 0;
      RT_CALLRA(0x24d810, guest::uiArenaSprites(param_3,uStack_a0,1,uVar1,uVar2,1));
    }
    else if ((1 < lVar5) || (lVar5 != 0)) {
      RT_CALLRA(0x24d84c, guest::uiArenaSetTexture(param_3,0x2a9868,1,((*(int32_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)iStack_60)) + 8))) >> 3) % 3 + 1));
      RT_CALLRA(0x24d858, guest::uiArenaDataTag(param_3,1));
      (*(uint64_t*)rtguest::G((uint32_t)uStack_a0)) = 0x2600000021;
      (*(uint64_t*)rtguest::G((uint32_t)uStack_98)) = 0xa8;
      (*(uint64_t*)rtguest::G((uint32_t)uStack_90)) = 0x1800000010;
      (*(uint64_t*)rtguest::G((uint32_t)uStack_88)) = 0x200000000;
      RT_CALLRA(0x24d8bc, guest::uiArenaSprites(param_3,uStack_a0,1,uVar1,uVar2,1));
    }
  }
  lVar5 = (int32_t)RT_CALLRA(0x24d8cc, guest::CalculateTachometerValue(puVar8,10,1));
  if (lVar5 != 0) {
    if (1 < lVar5) {
      iVar6 = (int)lVar5 + -1;
      RT_CALLRA(0x24d8fc, guest::uiArenaSetTexture(param_3,0x2a9868,0,0));
      RT_CALLRA(0x24d908, guest::uiArenaDataTag(param_3,iVar6));
      RT_CALLRA(0x24d928, guest::uiArenaSprites(param_3,0x2a9978,iVar6,uVar1,uVar2,1));
    }
    if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)iStack_60)) + 8))) & 1) != 0) {
      RT_CALLRA(0x24d950, guest::uiArenaSetTexture(param_3,0x2a9868,0,0));
      RT_CALLRA(0x24d95c, guest::uiArenaDataTag(param_3,1));
      iVar6 = (int)lVar5 * 0x20;
      (*(int32_t*)rtguest::G((uint32_t)iStack_80)) = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x002a9958 + iVar6))) + -4;
      (*(int32_t*)rtguest::G((uint32_t)iStack_7c)) = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x002a995c + iVar6))) + -7;
      (*(uint32_t*)rtguest::G((uint32_t)uStack_78)) = 0xc0;
      (*(uint32_t*)rtguest::G((uint32_t)uStack_70)) = 0x1a;
      (*(uint32_t*)rtguest::G((uint32_t)uStack_74)) = 0x60;
      (*(uint32_t*)rtguest::G((uint32_t)uStack_6c)) = 0x1e;
      (*(uint32_t*)rtguest::G((uint32_t)uStack_68)) = 0;
      (*(uint32_t*)rtguest::G((uint32_t)uStack_64)) = 0;
      (*(uint64_t*)rtguest::G((uint32_t)uStack_a0)) = CONCAT44((*(int32_t*)rtguest::G((uint32_t)iStack_7c)),(*(int32_t*)rtguest::G((uint32_t)iStack_80)));
      (*(uint64_t*)rtguest::G((uint32_t)uStack_98)) = 0x60000000c0;
      (*(uint64_t*)rtguest::G((uint32_t)uStack_90)) = 0x1e0000001a;
      (*(uint64_t*)rtguest::G((uint32_t)uStack_88)) = 0;
      RT_CALLRA(0x24da10, guest::uiArenaSprites(param_3,uStack_a0,1,uVar1,uVar2,1));
    }
  }
  RT_CALLRA(0x24da18, guest::uiArenaSubmit(param_3));
  return 0;
}


RT_GUEST_FN(0x0024d3e0, guest::FUN_0024d3e0)

}  // namespace guest
